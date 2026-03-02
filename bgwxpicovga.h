#ifndef BGWXPICOVGA_H
#define BGWXPICOVGA_H

#ifdef FRAMEBUFFER_ENABLED
#define FRAMEBUFFER_DOUBLE
#endif

#ifdef FRAMEBUFFER_DOUBLE
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FRAMEBUFFER_WIDTH SCREEN_WIDTH
#define FRAMEBUFFER_HEIGHT SCREEN_HEIGHT
#define PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS SCREEN_WIDTH
#define mandel
#define FRAMEBUFFER_DOUBLE_WIDTH (FRAMEBUFFER_WIDTH / 2)
#define FRAMEBUFFER_DOUBLE_HEIGHT (FRAMEBUFFER_HEIGHT / 2)
#endif

#ifdef mandelbrot-compile
#include "mandelbrot_compile.c"
#endif

#ifdef DEBUG_VGA_WORKFLOW
#include <Arduino.h>
#endif

#ifndef VGA_MODE
#define VGA_MODE vga_mode_640x480_60  // default
#endif

#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "m_helper.h"

#ifdef TEST_BAR
#include "test_bar.h"
#endif

class bgwxpicovga {
public:
  bgwxpicovga();
  ~bgwxpicovga();

  scanvideo_scanline_buffer_t *dest_member;

  // Buffer callback: called once per framebuffer row in render_loop().
  // Signature: void cb(uint16_t *row_pixels, int row_y)
  typedef void (*bufferCallback)(uint16_t *colour_buf, int scanline);
  bufferCallback bufferCallbackFn = nullptr;
  void setBufferCallback(bufferCallback cb) {
    bufferCallbackFn = cb;
  }

#ifdef FRAMEBUFFER_DOUBLE
  // Fills one VGA scanline buffer from the framebuffer (called by m_timerCallback).
  void timer_send_buffer(struct scanvideo_scanline_buffer *buffer);
  // Alarm callback: drains the scanvideo engine's scanline queue each tick.
  static int64_t m_timerCallback(alarm_id_t alarm_id, void *user_data);
  // 320x240 pixel framebuffer; each pixel is displayed 2x2 on the 640x480 output.
  static uint16_t framebuffer[FRAMEBUFFER_DOUBLE_HEIGHT][FRAMEBUFFER_DOUBLE_WIDTH];
#endif

  // Current framebuffer row being rendered (shared between cores under mutex).
  static uint y;

  mutex_t frame_mutex;
  bool params_ready;
  bool core_0_started;

  void begin_core_0();
  void begin_core_1();

  // Main rendering loop: fills each framebuffer row via bufferCallbackFn.
  void render_loop();

  // User framebuffer pointer (set via m_framebuffer_store).
  uint16_t *m_user_framebuffer;
  size_t m_user_framebuffer_len;

  void sendUserBuffer(const uint16_t *user_buf, int buf_len);

  uint16_t getRGB(uint8_t r, uint8_t g, uint8_t b);
  uint16_t getScanline();
  uint32_t getFrame();

  void m_framebuffer_store(uint16_t *user_input, size_t user_input_len);
  void frame_update_logic();
  void begin();

  typedef void (*LineCallbackType)(uint16_t line);
  LineCallbackType lineCallback = nullptr;
  void setLineCallback(LineCallbackType cb) {
    lineCallback = cb;
  }

  typedef void (*FrameCallbackType)(uint32_t frame);
  void setFrameCallback(FrameCallbackType fcb) {
    frameCallback = fcb;
  }
  FrameCallbackType frameCallback = nullptr;
};

// Static member definitions
#ifdef FRAMEBUFFER_DOUBLE
uint16_t bgwxpicovga::framebuffer[FRAMEBUFFER_DOUBLE_HEIGHT][FRAMEBUFFER_DOUBLE_WIDTH];
uint bgwxpicovga::y = 0;
#endif

#ifdef FRAMEBUFFER_ENABLED
#ifndef FRAMEBUFFER_SINGLE
#define FRAMEBUFFER_DOUBLE
#endif
#endif
#include "framebuffer_vga.h"
#ifdef FRAMEBUFFER_DOUBLE
#include "framebuffer_vga.h"
#endif

#include "hardware/clocks.h"
#include "pico/multicore.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/sync.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

// Semaphore used to synchronise Core 0 setup with Core 1 start.
struct semaphore video_setup_complete;

// Prepare a raw-pixel scanline buffer.
// Returns a uint16_t pointer into dest->data where pixel values should be written.
// width must be >= 3 and even.
static inline uint16_t *raw_scanline_prepare(struct scanvideo_scanline_buffer *dest, uint width) {
  assert(width >= 3);
  assert(width % 2 == 0);
  dest->data[0] = COMPOSABLE_RAW_RUN | (width + 1 - 3 << 16);
  dest->data[width / 2 + 2] = 0x0000u | (COMPOSABLE_EOL_ALIGN << 16);
  dest->data_used = width / 2 + 2;
  assert(dest->data_used <= dest->data_max);
  return (uint16_t *)&dest->data[1];
}

// Finalize a raw-pixel scanline buffer: pivots the first two words for the PIO
// state machine and marks the scanline ready.
static inline void raw_scanline_finish(struct scanvideo_scanline_buffer *dest) {
  uint32_t first = dest->data[0];
  uint32_t second = dest->data[1];
  dest->data[0] = (first & 0x0000ffffu) | ((second & 0x0000ffffu) << 16);
  dest->data[1] = (second & 0xffff0000u) | ((first & 0xffff0000u) >> 16);
  dest->status = SCANLINE_OK;
}

uint16_t bgwxpicovga::getRGB(uint8_t r, uint8_t g, uint8_t b) {
  return PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b);
}

uint16_t bgwxpicovga::getScanline() {
  return scanvideo_scanline_number(dest_member->scanline_id);
}

uint32_t bgwxpicovga::getFrame() {
  return scanvideo_frame_number(dest_member->scanline_id);
}

void bgwxpicovga::sendUserBuffer(const uint16_t *buff, int buf_len) {
}

void bgwxpicovga::frame_update_logic() {
  if (!params_ready) {
    params_ready = true;
  }
  if (lineCallback) {
    lineCallback(getScanline());
  }
  if (frameCallback) {
    frameCallback(getFrame());
  }
}

void bgwxpicovga::m_framebuffer_store(uint16_t *buffer_input, size_t buffer_input_len) {
  m_user_framebuffer = buffer_input;
  m_user_framebuffer_len = buffer_input_len;
}

void bgwxpicovga::begin_core_0() {
#ifdef mandel
  uint base_freq;
#if !ALARM_POOL_ON_CORE1
#if PICO_ON_DEVICE
  add_alarm_in_us(100, m_timerCallback, NULL, true);
#endif
#endif
#if PICO_SCANVIDEO_48MHZ
  base_freq = 48000;
#else
  base_freq = 50000;
#endif
#if PICO_ON_DEVICE
#if TURBO_BOOST
  vreg_set_voltage(VREG_VOLTAGE_1_30);
  sleep_ms(10);
  set_sys_clock_khz(base_freq * 6, true);
#else
  set_sys_clock_khz(base_freq * 3, true);
#endif
#endif
#endif
  mutex_init(&frame_mutex);
  sem_init(&video_setup_complete, 0, 1);
  hard_assert(VGA_MODE.width + 4 <= PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS * 2);
  scanvideo_setup(&VGA_MODE);
  scanvideo_timing_enable(true);
  frame_update_logic();
  sem_release(&video_setup_complete);
}

void bgwxpicovga::begin_core_1() {
  sem_acquire_blocking(&video_setup_complete);
  printf("CORE 1 go\n");
  SerialDbg.print("[CORE 1] Starting VGA on core: ");
  SerialDbg.println("");
#if ALARM_POOL_ON_CORE1
#if PICO_ON_DEVICE
  alarm_pool_add_alarm_in_us(alarm_pool_create(0, 3), 100, m_timerCallback, NULL, true);
#endif
#endif
}

void bgwxpicovga::begin() {
#if SYSCLOCK_125mhz
  set_sys_clock_khz(125000, true);
#endif
  delay(50);
#if DUAL_CORE
  int core_number = get_core_num();
  if (core_number == 0) {
    begin_core_0();
  } else if (core_number == 1) {
    while (!core_0_started) {
      ;
    }
    begin_core_1();
  }
#else
  begin_core_0();
#endif
}

// Fills one VGA scanline from the class framebuffer using the raw-run timing
// structure ported from the Mandelbrot example.
// Each 320-wide framebuffer row is displayed twice vertically (2x scale) and
// each pixel is repeated horizontally, giving a 640x480 output from 320x240 data.
void bgwxpicovga::timer_send_buffer(struct scanvideo_scanline_buffer *buffer) {
  uint line = scanvideo_scanline_number(buffer->scanline_id);
  int fb_y = (int)(line / 2);
  if (fb_y < 0) fb_y = 0;
  if (fb_y >= FRAMEBUFFER_DOUBLE_HEIGHT) fb_y = FRAMEBUFFER_DOUBLE_HEIGHT - 1;
  uint16_t *pixels = framebuffer[fb_y];

  // raw_scanline_prepare/finish are the timing-critical helpers inherited from
  // the Mandelbrot example; they must not be moved to flash.
  uint16_t *buf = ::raw_scanline_prepare(buffer, FRAMEBUFFER_WIDTH);
  for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
    // Read from the 320-wide framebuffer at half the x-coordinate to achieve
    // 2x horizontal scaling (each framebuffer pixel maps to 2 VGA pixels).
    buf[x] = pixels[x / 2];
  }
  ::raw_scanline_finish(buffer);
}

// Alarm callback: drains all pending scanvideo scanline requests.
// Registered in begin_core_0() so the PIO engine is always fed on time.
int64_t bgwxpicovga::m_timerCallback(alarm_id_t alarm_id, void *user_data) {
  bgwxpicovga *self = static_cast<bgwxpicovga *>(user_data);
  struct scanvideo_scanline_buffer *dest = scanvideo_begin_scanline_generation(false);
  while (dest) {
    self->timer_send_buffer(dest);
    scanvideo_end_scanline_generation(dest);
    dest = scanvideo_begin_scanline_generation(false);
  }
  return 100;
}

// Main rendering loop: iterates over every framebuffer row and calls
// bufferCallbackFn so the user can write custom pixel data per row.
// Call this from loop() or loop1(); it runs forever.
void bgwxpicovga::render_loop() {
  int core_num = get_core_num();
  printf("Rendering on core %d\n", core_num);
  while (true) {
    mutex_enter_blocking(&frame_mutex);
    // When all rows have been filled, reset for the next frame.
    if (y == FRAMEBUFFER_DOUBLE_HEIGHT) {
      params_ready = false;
      frame_update_logic();
      y = 0;
    }
    uint _y = y++;
    mutex_exit(&frame_mutex);

    // Let the user fill framebuffer row _y with custom pixel data.
    // If no callback is registered the row stays zeroed (black).
    if (bufferCallbackFn) {
      bufferCallbackFn(framebuffer[_y], (int)_y);
    }

#if !PICO_ON_DEVICE
    // Simulator path: push the scanline immediately.
    struct scanvideo_scanline_buffer *buffer = scanvideo_begin_scanline_generation(true);
    timer_send_buffer(buffer);
    scanvideo_end_scanline_generation(buffer);
#endif
  }
}

bgwxpicovga::bgwxpicovga() {}
bgwxpicovga::~bgwxpicovga() {}
#endif
