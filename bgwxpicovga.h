#ifndef BGWXPICOVGA_H
#define BGWXPICOVGA_H
//  #include "pico/bgwxscanvideo.h"


//mandelbrot
#ifdef FRAMEBUFFER_ENABLED
#define FRAMEBUFFER_DOUBLE
#endif

#ifdef FRAMEBUFFER_DOUBLE

// #include "pico/scanvideo.h"
// #include "pico/scanvideo/composable_scanline.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
// #include "pico/multicore.h"
#define FRAMEBUFFER_WIDTH SCREEN_WIDTH
#define FRAMEBUFFER_HEIGHT SCREEN_HEIGHT
#define PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS SCREEN_WIDTH
#define mandel

#define FRAMEBUFFER_DOUBLE_WIDTH (FRAMEBUFFER_WIDTH / 2)
#define FRAMEBUFFER_DOUBLE_HEIGHT (FRAMEBUFFER_HEIGHT / 2)

// static uint16_t framebuffer[FRAMEBUFFER_DOUBLE_HEIGHT][FRAMEBUFFER_DOUBLE_WIDTH];
#endif

#ifdef mandelbrot-compile
#include "mandelbrot_compile.c"
#endif

#ifdef DEBUG_VGA_WORKFLOW
#include <Arduino.h>
#endif

#ifndef VGA_MODE
#define VGA_MODE vga_mode_640x480_60  //default
#endif
// #ifdef DPI_MODE_ENABLED
// #include "pico/pico_scanvideo_dpi.c"
// #endif
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "m_helper.h"

#ifdef TEST_BAR
#include "test_bar.h"
#endif

// #ifdef FRAMEBUFFER_ENABLED
// #include "framebuffer_vga.h"
// #endif
// #ifdef FRAMEBUFFER_DOUBLE
// #include "framebuffer_vga.h"
// #endif

typedef struct callbackHandler {
  typedef void (*callbackType)();
  callbackType callbackHandle;
} callbackHandle;


class bgwxpicovga {
public:
  bgwxpicovga();
  ~bgwxpicovga();
  scanvideo_scanline_buffer_t *dest_member;
  void r_loop(bool block);
  typedef void (*bufferCallback)(uint16_t *colour_buf, int scanline);
  bufferCallback bufferCallbackFn = nullptr;
  void setBufferCallback(bufferCallback cb) {
    bufferCallbackFn = cb;
  }
#ifdef FRAMEBUFFER_DOUBLE
  void fillFramebufferWithColor(uint16_t color);
  void drawFramebufferLine(scanvideo_scanline_buffer_t *dest);
  // void timer_send_buffer(uint8_t r, uint8_t g, uint8_t b);
  void timer_send_buffer(struct scanvideo_scanline_buffer *buffer);
  static int64_t m_timerCallback(alarm_id_t alarm_id, void *user_data);
  void m_rawScanlineFinish(struct scanvideo_scanline_buffer *dest);
  static uint16_t framebuffer[FRAMEBUFFER_DOUBLE_HEIGHT][FRAMEBUFFER_DOUBLE_WIDTH];
  void exampleDrawDiagonal();
  void setPixelXY(int x, int y, uint16_t color);
  void core1_func();
  void m_fillFramebufferWithColor(uint16_t color);
  uint32_t vga_temp_buffer[FRAMEBUFFER_DOUBLE_WIDTH];
#endif
  // unsigned long _y;
  static uint y;
  uint32_t last_frame_num = 0;
  // void setPixelXY(int, int, int);
  // --- In render_loop(), comment out all Mandelbrot logic ---

  void __time_critical_func(rend_loop)() {
    render_loop();
  }
  // --- Mandelbrot scanline generator ---
  static inline void m_scanline(uint16_t *line_buffer, uint length) {
    // ... Mandelbrot math... removed
  }
  int64_t timer_callback(alarm_id_t alarm_id, void *user_data) {
    struct scanvideo_scanline_buffer *buffer =
      scanvideo_begin_scanline_generation(false);
    while (buffer) {
      // m_fillScanlineBuffer(buffer);
      scanvideo_end_scanline_generation(buffer);
      buffer = scanvideo_begin_scanline_generation(false);
    }
    return 100;
  }
  // typedef void (*LineCallbackType)();

  mutex_t frame_mutex;
  bool params_ready;
  bool core0_started;
  void colorSet(uint8_t red_set, uint8_t green_set, uint8_t blue_set);
  uint32_t fb_count;
  uint32_t fb_count_send;
  void loopCore0();
  void loopCore1();
  bool core_0_started;
  void begin_core_0();
  void begin_core_1();
  void render_loop();
  // void setPixelXY(int x, int y, uint16_t color);
  // uint32_t last_frame_num;
  void minimal_loop(bool dualcore);
  uint16_t *m_user_framebuffer;
  size_t m_user_framebuffer_len;
  uint32_t *user_buffer;
  size_t user_buffer_len;
  uint32_t *next_user_buffer;
  size_t next_user_buffer_len;
  void video_loop(bool block);
  void sendUserBuffer(const uint16_t *user_buf, int buf_len);
  const uint16_t *buff;
  uint16_t buff_len;
  bool execute_buffer(scanvideo_scanline_buffer_t *scanbuf);
  uint16_t getRGB(uint8_t r, uint8_t g, uint8_t b);
  uint16_t getRed(uint8_t value);
  uint16_t getGreen(uint8_t value);
  uint16_t getBlue(uint8_t value);
  uint16_t getScanline();
  uint32_t getFrame();
  uint32_t getNextscanline();

  uint32_t waitScanline(uint32_t scanline_id);
  void update_vga_display(scanvideo_scanline_buffer_t *dest);
  uint32_t *getBuffer();
  void sendVGA();
  // void __time_critical_func(render_scanline)(struct scanvideo_scanline_buffer *dest);
  // int __time_critical_func(r_loop)();
  void imageSource(uint16_t *colour_buf, int scanline);
  // int vga_main();
  void frame_update_logic();
  uint16_t *raw_scanline_prepare(struct scanvideo_scanline_buffer *dest, uint width);
  void raw_scanline_finish(struct scanvideo_scanline_buffer *dest);
  void flash_bulk_read(uint32_t *rxbuf, uint32_t flash_offs, size_t len, uint dma_chan);
  void render_scanline(struct scanvideo_scanline_buffer *dest, int core_num);
  void sendBuffer();
  void drawBuf();
  void storeBuffer(uint32_t *buffer_input, size_t buffer_input_len);
  void storeBuffer2(uint16_t *buffer_input, size_t buffer_input_len, int scanline_id_handle);
  void m_framebuffer_store(uint16_t *user_input, size_t user_input_len);
  bool dbgSwitch;
  void debug_switch(bool switching) {
    dbgSwitch = switching;
  }
  void minimal_loop_backup();
  bool placeholder = false;
  bool stream_mode = false;
  void streamEnabled(bool enable) {
    stream_mode = enable;
  }
  bool is_stream_mode_enabled() const {
    return stream_mode;
  }
  void debugLoop(long period);
  void callbackBridge();
  void drawLine(uint16_t r_val, uint16_t g_val, uint16_t b_val);
  void write(uint16_t rtx, uint16_t gtx, uint16_t btx);
  void run();
  void wait();
  void waitFrame(bool toggle);
  bool waitFrameEnabled;
  bool waitFrameGet();
  int getInstanceCount();
  int instance_count;
  bool run_block;
  uint32_t last_frame;
  void begin();
  // void drawBuf(); // Already defined above, remove duplicate declaration
  void setup(int instanceNumber);
  typedef void (*LineCallbackType)(uint16_t line);
  LineCallbackType lineCallback = nullptr;
  void setLineCallback(LineCallbackType cb) {
    lineCallback = cb;
  };
  typedef void (*FrameCallbackType)(uint32_t frame);
  void setFrameCallback(FrameCallbackType fcb) {
    frameCallback = fcb;
  };
  FrameCallbackType frameCallback = nullptr;
  // ...other members...
private:
  void libraryCallback(scanvideo_scanline_buffer_t *scanline){
    // if (userCallback) {
    //   userCallback();
    // }
  };
  // std::vector<uint32_t> user_scanline_buffer;
  // int scanline_width = VGA_MODE.width;
  LineCallbackType userCallback = nullptr;
  bool lineBuffer = false;
  LineCallbackType callbackHandle = nullptr;
  // protected:jj
  // Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
};

#ifdef FRAMEBUFFER_ENABLED
#ifndef FRAMEBUFFER_SINGLE
#define FRAMEBUFFER_DOUBLE
#endif
#endif
#include "framebuffer_vga.h"
#ifdef FRAMEBUFFER_DOUBLE
#include "framebuffer_vga.h"
#endif

#define MIN_COLOR_RUN 3
bool lineBuffer = false;

// Fill one scanline buffer with a solid color chosen by the bar index
// This function is called for each scanline to generate the test pattern
static void drawTestBar(scanvideo_scanline_buffer_t *dest) {
  uint32_t *buf = dest->data;                               // Pointer to the scanline buffer
  int line = scanvideo_scanline_number(dest->scanline_id);  // Get the scanline number (row)

  // Fallback: original color bar pattern
  // Change NUM_BARS or bar_colors[] to adjust the test pattern:
  // NUM_BARS controls the number of horizontal bars
  // bar_colors[] controls the color of each bar
  int bar_index = (line * NUM_BARS) / VGA_MODE.height;
  uint16_t color = bar_colors[bar_index % NUM_BARS];
  int width = VGA_MODE.width;

  buf[0] = COMPOSABLE_COLOR_RUN | ((uint32_t)color << 16);               // Run of color pixels
  buf[1] = (uint32_t)(width - 3) | ((uint32_t)COMPOSABLE_RAW_1P << 16);  // Fills the rest of the line
  buf[2] = 0 | ((uint32_t)COMPOSABLE_EOL_ALIGN << 16);                   // End of line
  dest->data_used = 3;
  dest->status = SCANLINE_OK;  // Mark scanline as ready
}


void bgwxpicovga::debugLoop(long period) {
  long millisPrev = 0;
  long millisDelay = period;  // the delay period
  while (dbgSwitch) {

    // Block until the scanvideo engine needs a new scanline
    scanvideo_scanline_buffer_t *buf = scanvideo_begin_scanline_generation(true);
    // Detect a new frame; only one core should run the per-frame logic
    mutex_enter_blocking(&frame_mutex);
    libraryCallback(buf);
    uint32_t frame = scanvideo_frame_number(buf->scanline_id);
    if (frame != last_frame) {
      last_frame = frame;
      if (frameCallback) {
        frameCallback(frame);  // Fill user_buffer first!
      }
      execute_buffer(buf);  // Now use the freshly filled buffer
    }
    mutex_exit(&frame_mutex);
    // #ifndef TEST_BAR
    // // drawLine(buf);
    // #else
    drawTestBar(buf);
    // #endif
    scanvideo_end_scanline_generation(buf);
    // block=false;
    if (millis() > millisPrev + millisDelay) {
      dbgSwitch = !dbgSwitch;
      millisPrev = millis();
      return;
    }
  }
}

// Output a scanline directly to scanvideo (no framebuffer)
static inline void drawScanlineDirect(scanvideo_scanline_buffer_t *dest) {
  uint32_t *buf = dest->data;
  int line = scanvideo_scanline_number(dest->scanline_id);
  for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
    // Example: horizontal gradient
    uint16_t color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(x % 256, line % 256, 0);
    buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
  }
  buf[FRAMEBUFFER_WIDTH] = COMPOSABLE_EOL_ALIGN << 16;
  dest->data_used = FRAMEBUFFER_WIDTH + 1;
  dest->status = SCANLINE_OK;
}

// static inline uint32_t store_xy_framebuffer(uint16_t color, int pixel_x)
// {

//   uint16_t *xy_framebuffer;
//     for (int x = 0; x < VGA_MODE.width; ++x) {
//     if (x == pixel_x && line == pixel_y) {
//       buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
//     } else {
//       buf[x] = COMPOSABLE_RAW_1P | (0 << 16);  // Black
//     }
//   }
// }
// Output a scanline with a single pixel at (x, y) in the given color, black elsewhere
static inline void sendSinglePixelScanline(scanvideo_scanline_buffer_t *dest, int pixel_x, uint16_t color) {
  int line = scanvideo_scanline_number(dest->scanline_id);
  uint32_t *buf = dest->data;
  for (int x = 0; x < VGA_MODE.width; ++x) {
    // if (x == pixel_x && line == pixel_y) {
    if (x == pixel_x) {
      buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
    } else {
      buf[x] = COMPOSABLE_RAW_1P | (0 << 16);  // Black
    }
  }
  buf[VGA_MODE.width] = COMPOSABLE_EOL_ALIGN << 16;
  dest->data_used = VGA_MODE.width + 1;
  dest->status = SCANLINE_OK;
}


// Add a pixel to the framebuffer at (x, y) with a given color
static inline void sendPixel(int x, int y, uint16_t color) {
#ifdef FRAMEBUFFER_ENABLED
  // setPixelXY(x, y, color);  // Provided by framebuffer_vga.h
#else
  // No framebuffer enabled; do nothing or implement alternative
#endif
}

// Render a scanline from the framebuffer to the VGA scanline buffer
void bgwxpicovga::update_vga_display(scanvideo_scanline_buffer_t *dest) {
#ifdef FRAMEBUFFER_ENABLED
  drawFramebufferLine(dest);  // Provided by framebuffer_vga.h
#else
  // Example: Fill scanline with black if no framebuffer
  uint32_t *buf = dest->data;
  for (int x = 0; x < VGA_MODE.width; ++x) {
    buf[x] = COMPOSABLE_RAW_1P | (0 << 16);
  }
  buf[VGA_MODE.width] = COMPOSABLE_EOL_ALIGN << 16;
  dest->data_used = VGA_MODE.width + 1;
  dest->status = SCANLINE_OK;
#endif
}

// Ported from
// https://github.com/raspberrypi/pico-playground/blob/master/scanvideo/flash_stream/flash_stream.c
// Implements flash image scanline rendering for Pico VGA

#include "hardware/clocks.h"
#include "hardware/structs/dma.h"
#include "hardware/structs/ssi.h"
#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#define FLASH_IMAGE_BASE 0x1003c000
#define FLASH_IMAGE_SCANLINE_SIZE (640 * sizeof(uint16_t))
#define FLASH_IMAGE_SIZE (FLASH_IMAGE_SCANLINE_SIZE * 480)
#define FLASH_N_IMAGES 3
#define FRAMES_PER_IMAGE 300

#define VGA_MODE vga_mode_640x480_60

const uint16_t *img_base = (const uint16_t *)FLASH_IMAGE_BASE;

void __time_critical_func(frame_update_logic)() {
  // static uint slideshow_ctr = 0;
  // static uint image_index = 0;
  // if (++slideshow_ctr >= FRAMES_PER_IMAGE) {
  //   slideshow_ctr = 0;
  //   image_index = (image_index + 1) % FLASH_N_IMAGES;
  //   img_base =
  //     (const uint16_t *)(FLASH_IMAGE_BASE + FLASH_IMAGE_SIZE * image_index);
  // }
}

static inline uint16_t *raw_scanline_prepare(struct scanvideo_scanline_buffer *dest, uint width) {
  assert(width >= 3);
  assert(width % 2 == 0);
  dest->data[0] = COMPOSABLE_RAW_RUN | (width + 1 - 3 << 16);
  dest->data[width / 2 + 2] = 0x0000u | (COMPOSABLE_EOL_ALIGN << 16);
  dest->data_used = width / 2 + 2;
  assert(dest->data_used <= dest->data_max);
  return (uint16_t *)&dest->data[1];
}

static inline void raw_scanline_finish(struct scanvideo_scanline_buffer *dest) {
  uint32_t first = dest->data[0];
  uint32_t second = dest->data[1];
  dest->data[0] = (first & 0x0000ffffu) | ((second & 0x0000ffffu) << 16);
  dest->data[1] = (second & 0xffff0000u) | ((first & 0xffff0000u) >> 16);
  dest->status = SCANLINE_OK;
}

// void bgwxpicovga::prepareBuffer(struct scanvideo_scanline_buffer *vga_out, uint16_t *pixel_buffer_input) {
//   uint32_t *prepare;
//   raw_scanline_prepare(*prepare, VGA_MODE.width);

//   raw_scanline_finish(*vga_out);
// }


void __no_inline_not_in_flash_func(flash_bulk_read)(uint32_t *rxbuf,
                                                    uint32_t flash_offs,
                                                    size_t len, uint dma_chan) {
  ssi_hw->ssienr = 0;
  ssi_hw->ctrlr1 = len - 1;
  ssi_hw->dmacr = SSI_DMACR_TDMAE_BITS | SSI_DMACR_RDMAE_BITS;
  ssi_hw->ssienr = 1;

  dma_hw->ch[dma_chan].read_addr = (uint32_t)&ssi_hw->dr0;
  dma_hw->ch[dma_chan].write_addr = (uint32_t)rxbuf;
  dma_hw->ch[dma_chan].transfer_count = len;
  dma_hw->ch[dma_chan].ctrl_trig = DMA_CH0_CTRL_TRIG_BSWAP_BITS | DREQ_XIP_SSIRX << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB | dma_chan << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB | DMA_CH0_CTRL_TRIG_INCR_WRITE_BITS | DMA_CH0_CTRL_TRIG_DATA_SIZE_VALUE_SIZE_WORD << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB | DMA_CH0_CTRL_TRIG_EN_BITS;

  ssi_hw->dr0 = (flash_offs << 8) | 0xa0;

  while (dma_hw->ch[dma_chan].ctrl_trig & DMA_CH0_CTRL_TRIG_BUSY_BITS)
    tight_loop_contents();

  ssi_hw->ssienr = 0;
  ssi_hw->ctrlr1 = 0;
  ssi_hw->dmacr = 0;
  ssi_hw->ssienr = 1;
}


void bgwxpicovga::imageSource(uint16_t *colour_buf, int scanline) {
  if (bgwxpicovga::bufferCallbackFn) {
    bgwxpicovga::bufferCallbackFn(colour_buf, scanline);
  } else {
    // Default: read from flash
    flash_bulk_read((uint32_t *)colour_buf,
                    (uint32_t)img_base + scanline * FLASH_IMAGE_SCANLINE_SIZE,
                    FLASH_IMAGE_SCANLINE_SIZE / sizeof(uint32_t), 11);
  }
}

// void bgwxpicovga::render_scanline(struct scanvideo_scanline_buffer *dest) {
// #ifdef DEBUG_VGA_WORKFLOW
//   SPAM_GUARD_PRINT("render_scanline: entered");
// #endif
//   int l = scanvideo_scanline_number(dest->scanline_id);
//   uint16_t *colour_buf = raw_scanline_prepare(dest, VGA_MODE.width);
//   imageSource(colour_buf, l);
//   raw_scanline_finish(dest);
// }

// int bgwxpicovga::r_loop() {
// #ifdef DEBUG_VGA_WORKFLOW
//   SPAM_GUARD_PRINT("r_loop: entered");
// #endif
//   // static uint32_t last_frame_num = 0;
//   while (true) {
//     struct scanvideo_scanline_buffer *scanline_buffer =
//       scanvideo_begin_scanline_generation(true);
//     uint32_t frame_num = scanvideo_frame_number(scanline_buffer->scanline_id);
//     if (frame_num != last_frame_num) {
//       last_frame_num = frame_num;
//       frame_update_logic();
//     }
//     render_scanline(scanline_buffer);
//     scanvideo_end_scanline_generation(scanline_buffer);
//   }
// }

// int vga_main(void) {
//   scanvideo_setup(&VGA_MODE);
//   scanvideo_timing_enable(true);
//   r_loop();
//   return 0;
// }

// int main(void) {
//   set_sys_clock_khz(200000, true);
//   // setup_default_uart();
// #ifdef PICO_SMPS_MODE_PIN
//   gpio_init(PICO_SMPS_MODE_PIN);
//   gpio_set_dir(PICO_SMPS_MODE_PIN, GPIO_OUT);
//   gpio_put(PICO_SMPS_MODE_PIN, 1);
// #endif
//   return vga_main();
// }

// Include necessary Pico SDK headers for VGA and multicore support
#include "hardware/clocks.h"                     // For clock configuration (not directly used here, but often needed for timing)
#include "pico/multicore.h"                      // For multicore synchronization (used for dual-core rendering)
#include "pico/scanvideo.h"                      // Main scanvideo API for VGA output
#include "pico/scanvideo/composable_scanline.h"  // For composable scanline encoding macros
#include "pico/sync.h"                           // For mutexes and semaphores
#ifdef BGW_FRAMEBUFFER_ENABLED
#define VGA_MODE vga_mode_640x480_60  //default
#include "framebuffer_vga.h"
#endif

#ifndef VGA_MODE
#define VGA_MODE vga_mode_640x480_60  //default
#endif

#include "pico/scanvideo.h"
#include "pico/scanvideo/composable_scanline.h"

// Mutex to guard per-frame state updates when both cores render concurrently
// Semaphore to signal Core 1 that scanvideo setup on Core 0 is complete
static semaphore_t core1_start_sem;
// Tracks the last rendered frame number to detect frame transitions
// static volatile uint32_t last_frame = 0;


// Helper: Set a single pixel at (x, y) in a scanline buffer
// dest: pointer to scanvideo_scanline_buffer_t for the current scanline
// x: pixel column (0 ... VGA_MODE.width-1)
// color: 16-bit pixel color (use getRGB)
static inline void setPixel(scanvideo_scanline_buffer_t *dest, int x, uint16_t color) {
  uint32_t *buf = dest->data;
  // Each pixel must be encoded as a RAW_1P token at the correct position.
  // For a single pixel, you can use: COMPOSABLE_RAW_1P | (color << 16)
  // Example: set pixel at position x (for demonstration, assumes buffer is empty)
  buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
  // You must also ensure the scanline ends with a COMPOSABLE_EOL_ALIGN token
  // at the correct position (after the last pixel).
}

// Called once per frame (by whichever core arrives first at the new frame)
// Place any per-frame animation or state update logic here (currently unused)
static inline void onNewFrame() {}
// static uint32_t last_frame = 0;
// "Worker thread" for each core
bool lineCallbackCheck = false;

int32_t single_color_scanline(uint32_t *buf, size_t buf_length, int width, uint32_t pixel_output) {
  assert(buf_length >= 2);

  assert(width >= MIN_COLOR_RUN);
  // | jmp color_run | color | count-3 |  buf[0] =
  buf[0] = COMPOSABLE_COLOR_RUN | (pixel_output << 16);
  buf[1] = (width - MIN_COLOR_RUN) | (COMPOSABLE_RAW_1P << 16);
  // note we must end with a black pixel
  buf[2] = 0 | (COMPOSABLE_EOL_ALIGN << 16);

  return 3;
}

void bgwxpicovga::waitFrame(bool toggle) {
  bgwxpicovga::waitFrameEnabled = toggle;
}

bool bgwxpicovga::waitFrameGet() {
  return bgwxpicovga::waitFrameEnabled;
}
void bgwxpicovga::wait() {
  run_block = false;
}

void render_scanline(struct scanvideo_scanline_buffer *dest, int core) {
  uint32_t *buf = dest->data;
  size_t buf_length = dest->data_max;
  int l = scanvideo_scanline_number(dest->scanline_id);
  uint16_t bgcolor = (uint16_t)l << 2;
  dest->data_used = single_color_scanline(buf, buf_length, VGA_MODE.width, bgcolor);
  dest->status = SCANLINE_OK;
}

// typedef struct _render_{
//   callbackHandler lineCallback;
// } render;



// Helper function: Build a 16-bit pixel value from 8-bit RGB components (5:5:5 layout)
uint16_t bgwxpicovga::getRGB(uint8_t r, uint8_t g, uint8_t b) {
  return PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b);
}
uint16_t bgwxpicovga::getRed(uint8_t value) {
  return PICO_SCANVIDEO_PIXEL_FROM_RGB8(value, 0, 0);
}
uint16_t bgwxpicovga::getGreen(uint8_t value) {
  return PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, value, 0);
}
uint16_t bgwxpicovga::getBlue(uint8_t value) {
  return PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 0, value);
}


// static inline uint16_t scanvideo_frame_number(uint32_t scanline_id) {
//     return (uint16_t) (scanline_id >> 16u);
// }

// static inline uint16_t scanvideo_scanline_number(uint32_t scanline_id) {
//     return (uint16_t) scanline_id;
// }

// static inline scanvideo_mode_t scanvideo_get_mode();
// static inline uint32_t scanvideo_get_next_scanline_id();
// static inline bool scanvideo_in_vblank();
// static inline bool scanvideo_in_hblank();
// static inline void scanvideo_wait_for_vblank();
// static inline uint32_t scanvideo_wait_for_scanline_complete(uint32_t scanline_id);

// Helper: Get scanline number from buffer
uint16_t bgwxpicovga::getScanline() {
  return scanvideo_scanline_number(dest_member->scanline_id);
}
uint32_t bgwxpicovga::getFrame() {
  return scanvideo_frame_number(dest_member->scanline_id);
}
#ifdef DPI_MODE_ENABLED
uint32_t bgwxpicovga::getNextscanline() {
  return scanvideo_get_next_scanline_id();
}

// uint32_t bgwxpicovga::waitScanline(uint32_t scanline_id) {
//   return scanvideo_wait_for_scanline_complete(dest_member->scanline_id);
// }
#endif
// Helper: Get pointer to scanline buffer
uint32_t *bgwxpicovga::getBuffer() {
  return dest_member->data;
}

// Helper: Mark scanline as ready
void bgwxpicovga::sendVGA() {
  // Mark scanline as ready
  dest_member->status = SCANLINE_OK;
}


// void bgwxpicovga::video_loop(bool block) {
// #ifdef DEBUG_VGA_WORKFLOW
//   SPAM_GUARD_PRINT("video_loop: entered");
// #endif
//   int core_num = get_core_num();
//   // printf("Rendering on core %d\n", core_num);
//   while (block) {
//     struct scanvideo_scanline_buffer *scanline_buffer = scanvideo_begin_scanline_generation(true);
//     mutex_enter_blocking(&frame_mutex);
//     uint32_t frame_num = scanvideo_frame_number(scanline_buffer->scanline_id);
//     // Note that with multiple cores we may have got here not for the first
//     // scanline, however one of the cores will do this logic first before either
//     // does the actual generation
//     if (frame_num != last_frame) {
//       last_frame = frame_num;
//       if (frameCallback) {
//         frameCallback(frame);      // Fill user_buffer first!
//       }
//       execute_buffer(scanbuf);       // Now use the freshly filled buffer
//     }
//     mutex_exit(&frame_mutex);
//     //      render_scanline          start//
//     // sendSinglePixelScanline(scanline_buffer, );

//     //render_scanline stop//

//     // Release the rendered buffer into the wild
//     scanvideo_end_scanline_generation(scanline_buffer);
//   }
// }
// typedef struct _render_{
//   renderCallbackType renderCallback;
// } render;


// void bgwxpicovga::setLineCallback(LineCallbackType cb) {
//   callbackHandle = cb;
//   // return true;
// }
struct semaphore video_setup_complete;

bool core_setup_func(bool toggle) {
  if (toggle) {
    return true;
  } else {
    return false;
  }
}

void bgwxpicovga::core1_func() {
}

void bgwxpicovga::setup(int instanceNumber) {
}
int instance_count;
int bgwxpicovga::getInstanceCount() {
  return instance_count;
}

void bgwxpicovga::begin_core_0() {
// core_setup_func(false);
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
  instance_count++;
  // setup(bgwxpicovga::instance_count);
  mutex_init(&frame_mutex);  // Initialize mutex for frame sync
  sem_init(&video_setup_complete, 0, 1);
  // if (core) {
  // sem_init(&core1_start_sem, 0, 1);  // Initialize semaphore for core sync
  // }
  // Core 1 will wait for us to finish video setup, and then start rendering
  //  multicore_launch_core1(core1_func);
  hard_assert(VGA_MODE.width + 4 <= PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS * 2);
  scanvideo_setup(&VGA_MODE);     // Set up scanvideo with selected VGA mode
  scanvideo_timing_enable(true);  // Enable VGA timing
                                  // if (core) {
  frame_update_logic();
  sem_release(&video_setup_complete);
  // core_setup_func(true);
  // core0_started = true;
  // sem_release(&core1_start_sem);  // Let Core 1 start rendering
  // core_setup_func(true);
  // }
  // run_block = true;
}


void bgwxpicovga::begin_core_1() {

  sem_acquire_blocking(&video_setup_complete);
  printf("CORE 1 go\n");
  SerialDbg.print("[CORE 1] Starting VGA on core: ");
  SerialDbg.println("");
#if ALARM_POOL_ON_CORE1
#if PICO_ON_DEVICE
  alarm_pool_add_alarm_in_us(alarm_pool_create(0, 3), 100, timer_callback, NULL, true);
#endif
#endif
  // while (!core0_started) {
  //   ;
  // }
  // while (!core_setup_func) {
  //   SPAM_GUARD_PRINT("core1 waiting for core0 setup...");
  // }
}

void bgwxpicovga::begin() {
#if SYSCLOCK_125mhz
  set_sys_clock_khz(125000, true);
#endif
  delay(50);
// SerialDbg.begin(115200);
#if DUAL_CORE
  bool core_0_starting = false;
  int core_number = get_core_num();
  SerialDbg.println(core_number) if (core_number == 0) {
    begin_core_0();
  }
  else if (core_number == 1) {
    while (!core_0_started) {
      ;
    }
    begin_core_1();
  }
#else
  begin_core_0();
#endif
}


// void bgwxpicovga::write(uint16_t rtx, uint16_t gtx, uint16_t btx) {

//   uint16_t color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(rtx, gtx, btx);
//   scanvideo_scanline_buffer_t *vga_pos = scanvideo_begin_scanline_generation(true);
//   // int width = VGA_MODE.width;
//   // buf[0] = COMPOSABLE_COLOR_RUN | ((uint32_t)color << 16);
//   // buf[1] = (uint32_t)(width - 3) | ((uint32_t)COMPOSABLE_RAW_1P << 16);
//   // buf[2] = 0 | ((uint32_t)COMPOSABLE_EOL_ALIGN << 16);
//   // uint16_t tx[3];
//   // tx[0]=rtx;
//   // tx[1]=gtx;
//   // tx[2]=btx;
//   uint16_t tx = PICO_SCANVIDEO_PIXEL_FROM_RGB8(rtx, gtx, btx);
//   vga_pos->data[0] = color;  //
//   vga_pos->data_used = 1;
//   vga_pos->status = SCANLINE_OK;
//   scanvideo_end_scanline_generation(vga_pos);
// }

void bgwxpicovga::write(uint16_t rtx, uint16_t gtx, uint16_t btx) {
  scanvideo_scanline_buffer_t *vga_pos = scanvideo_begin_scanline_generation(true);
  uint32_t *buf = vga_pos->data;
  int width = VGA_MODE.width;
  uint16_t color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(rtx, gtx, btx);
  int pixel_x = 33;  //placeholder value
  for (int x = 0; x < width; x++) {
    if (x == pixel_x) {
      buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
    } else {
      buf[x] = COMPOSABLE_RAW_1P | (0 << 16);  // Black
    }
  }
  buf[width] = COMPOSABLE_EOL_ALIGN << 16;
  vga_pos->data_used = width + 1;
  vga_pos->status = SCANLINE_OK;
  scanvideo_end_scanline_generation(vga_pos);
}
void bgwxpicovga::storeBuffer(uint32_t *buffer_input, size_t buffer_input_len) {
}
void bgwxpicovga::m_framebuffer_store(uint16_t *buffer_input, size_t buffer_input_len) {
  m_user_framebuffer = buffer_input;
  m_user_framebuffer_len = buffer_input_len;
}
// void bgwxpicovga::m_framebuffer_store()
// {

// }
void bgwxpicovga::storeBuffer2(uint16_t *buffer_input, size_t buffer_input_len, int scanline_id_handle) {
  int width = VGA_MODE.width;
  // int pixel_x = 33;  //placeholder value
  for (int x = 0; x < width; x++) {

    // if (x == pixel_x) {
    //   buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
    // } else {
    //   buf[x] = COMPOSABLE_RAW_1P | (0 << 16);  // Black
    // }
    next_user_buffer[x] = COMPOSABLE_RAW_1P | ((uint32_t)buffer_input[x] << 16);
    // Serial.print("user_buffer[x] = ");
    //     Serial.println(user_buffer[x]);
  }

  next_user_buffer[width] = COMPOSABLE_EOL_ALIGN << 16;
  next_user_buffer_len = buffer_input_len;
}

bool bgwxpicovga::execute_buffer(scanvideo_scanline_buffer_t *scanbuf) {
  // Debug print: show buffer pointers
  if (user_buffer != nullptr) {
#ifdef DEBUG_VGA_WORKFLOW
    char dbg_buf[128];
    snprintf(dbg_buf, sizeof(dbg_buf), "execute_buffer: user_buffer=%p, next_user_buffer=%p", user_buffer, next_user_buffer);
    SPAM_GUARD_PRINT(dbg_buf);
#endif
    scanbuf->data = user_buffer;
    scanbuf->data_used = VGA_MODE.width + 1;
    scanbuf->status = SCANLINE_OK;
// Debug print: buffer assignment
#ifdef DEBUG_VGA_WORKFLOW
    snprintf(dbg_buf, sizeof(dbg_buf), "execute_buffer: assigned scanbuf->data=%p", scanbuf->data);
    SPAM_GUARD_PRINT(dbg_buf);
#endif
    user_buffer = nullptr;  // Clear user_buffer after us
#ifdef DEBUG_VGA_WORKFLOW
    SPAM_GUARD_PRINT("execute_buffer: next_user_buffer set from user_buffer");
#endif
  } else {
    next_user_buffer = nullptr;
#ifdef DEBUG_VGA_WORKFLOW
    SPAM_GUARD_PRINT("execute_buffer: next_user_buffer set to nullptr");
#endif
  }
  return true;
}

void bgwxpicovga::run() {
  scanvideo_scanline_buffer_t *scanbuf = scanvideo_begin_scanline_generation(true);
  dest_member = scanbuf;
// Print scanline id and buffer state before processing
#ifdef DEBUG_VGA_WORKFLOW
  char buf[128];
  // snprintf(buf, sizeof(buf), "run: scanline_id=%d", scanbuf->scanline_id);
  // SPAM_GUARD_PRINT(buf);
  snprintf(buf, sizeof(buf), "run: user_buffer=%p, next_user_buffer=%p", user_buffer, next_user_buffer);
  SPAM_GUARD_PRINT(buf);
#endif
  // Detect a new frame; only one core should run the per-frame logic
  mutex_enter_blocking(&frame_mutex);
  uint32_t frame = scanvideo_frame_number(scanbuf->scanline_id);
  if (frame != last_frame) {
    if (next_user_buffer != nullptr) {
      user_buffer = next_user_buffer;  // Make the new buffer available to the user callback
    }
    last_frame = frame;
    execute_buffer(scanbuf);
    // while (execute_buffer(scanbuf)) {
    //   ;
    // }
    if (frameCallback) {
#ifdef DEBUG_VGA_WORKFLOW
      SPAM_GUARD_PRINT("run: new frame detected, frameCallback called");
#endif
      frameCallback(frame);
    }
    // libraryCallback(scanbuf);
  }
#ifdef DEBUG_VGA_WORKFLOW
  SPAM_GUARD_PRINT("run: execute_buffer called");
#endif
  mutex_exit(&frame_mutex);
  scanvideo_end_scanline_generation(scanbuf);
#ifdef DEBUG_VGA_WORKFLOW
  SPAM_GUARD_PRINT("run: scanvideo_end_scanline_generation called");
#endif
}
// -------------------------------------------------------------------------
// Rendering loop (runs on both Core 0 and Core 1)
// -------------------------------------------------------------------------
// Fill one scanline buffer with a solid color chosen by the bar index.

void bgwxpicovga::drawLine(uint16_t r_val, uint16_t g_val, uint16_t b_val) {
#ifdef DEBUG_VGA_WORKFLOW
  SPAM_GUARD_PRINT("drawLine: entered");
#endif
#ifdef DEBUG_VGA_WORKFLOW
  SPAM_GUARD_PRINT("write: entered");
#endif
#ifdef DEBUG_VGA_WORKFLOW
  SPAM_GUARD_PRINT("run: entered");
#endif
  scanvideo_scanline_buffer_t *dest = scanvideo_begin_scanline_generation(true);
  uint32_t *buf = dest->data;
  int line = scanvideo_scanline_number(dest->scanline_id);
  // const int number_bars = 8;
  // static uint16_t bar_colors[number_bars] = {
  //   0,                                              // Black
  //   PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 0, 0),        // Red
  //   PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 255, 0),      // Green
  //   PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 0, 255),      // Blue
  //   PICO_SCANVIDEO_PIXEL_FROM_RGB8(255, 255, 0),    // Yellow
  //   PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 255, 255),    // Cyan
  //   PICO_SCANVIDEO_PIXEL_FROM_RGB8(255, 0, 255),    // Magenta
  //   PICO_SCANVIDEO_PIXEL_FROM_RGB8(255, 255, 255),  // White
  // };

  // Map the scanline row to one of the color bars
  // int bar_index = (line * number_bars) / VGA_MODE.height;
  // uint16_t color = bar_colors[bar_index % number_bars];

  // Encode a solid-color scanline using the composable COLOR_RUN token.
  // Layout:  | COLOR_RUN | color | (width-3) | RAW_1P | 0 (black) | EOL ||
  // The trailing black pixel is required before the sync blanking interval.
  uint16_t color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(r_val, g_val, b_val);
  int width = VGA_MODE.width;
  buf[0] = COMPOSABLE_COLOR_RUN | ((uint32_t)color << 16);
  buf[1] = (uint32_t)(width - 3) | ((uint32_t)COMPOSABLE_RAW_1P << 16);
  buf[2] = 0 | ((uint32_t)COMPOSABLE_EOL_ALIGN << 16);
  dest->data_used = 3;
  dest->status = SCANLINE_OK;
  scanvideo_end_scanline_generation(dest);
}

// void sendUserBuffer(const uint16_t* user_buf, int buf_len);
// Send a user-provided buffer (raw pixel values) to the scanvideo scanline buffer
void bgwxpicovga::sendUserBuffer(const uint16_t *buff, int buf_len) {
#ifdef DEBUG_VGA_WORKFLOW
  SPAM_GUARD_PRINT("sendUserBuffer: entered");
#endif

  // scanvideo_scanline_buffer_t *vga_pos = scanvideo_begin_scanline_generation(true);
  // uint32_t *buf = vga_pos->data;
  // for (int x = 0; x < buf_len; ++x) {
  //   buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)user_buf[x] << 16);
  // }
  // buf[buf_len] = COMPOSABLE_EOL_ALIGN << 16;
  // vga_pos->data_used = buf_len + 1;
  // vga_pos->status = SCANLINE_OK;
  // scanvideo_end_scanline_generation(vga_pos);
}




// int bgwxpicovga::video_loop() {
//   // TODO: Implement video_loop logic
//   return 0;
// }
// int bgwxpicovga::vga_main() {
//   // TODO: Implement vga_main logic
//   return 0;
// }
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

uint16_t *bgwxpicovga::raw_scanline_prepare(struct scanvideo_scanline_buffer *dest, uint width) {
  return ::raw_scanline_prepare(dest, width);
}
void bgwxpicovga::raw_scanline_finish(struct scanvideo_scanline_buffer *dest) {
  ::raw_scanline_finish(dest);
}
void bgwxpicovga::flash_bulk_read(uint32_t *rxbuf, uint32_t flash_offs, size_t len, uint dma_chan) {
  // TODO: Implement flash_bulk_read logic
}
void bgwxpicovga::render_scanline(struct scanvideo_scanline_buffer *dest, int core_num) {
  // TODO: Implement render_scanline logic
}
void bgwxpicovga::sendBuffer() {
  // TODO: Implement sendBuffer logic
}


void bgwxpicovga::minimal_loop(bool dualcore) {
  // static uint32_t last_frame_num = 0;
  int core_num = get_core_num();
  int width = VGA_MODE.width;
  printf("Rendering on core %d\n", core_num);
  while (true) {
    struct scanvideo_scanline_buffer *scanline_buffer = scanvideo_begin_scanline_generation(true);
    mutex_enter_blocking(&frame_mutex);
    uint32_t frame_num = scanvideo_frame_number(scanline_buffer->scanline_id);
    next_user_buffer = user_buffer;
    // Note that with multiple cores we may have got here not for the first
    // scanline, however one of the cores will do this logic first before either
    // does the actual generation
    if (frame_num != last_frame_num) {
      last_frame_num = frame_num;
      frameCallback(frame_num);
      // frame_update_logic();
    }
    mutex_exit(&frame_mutex);
    //      render_scanline          start//
    execute_buffer(scanline_buffer);
    //render_scanline stop//

    // Release the rendered buffer into the wild
    scanvideo_end_scanline_generation(scanline_buffer);
  }
}


void bgwxpicovga::minimal_loop_backup() {
  // static uint32_t last_frame_num = 0;
  int core_num = get_core_num();
  int width = VGA_MODE.width;
  printf("Rendering on core %d\n", core_num);
  while (true) {
    struct scanvideo_scanline_buffer *scanline_buffer = scanvideo_begin_scanline_generation(true);
    mutex_enter_blocking(&frame_mutex);
    uint32_t frame_num = scanvideo_frame_number(scanline_buffer->scanline_id);
    // Note that with multiple cores we may have got here not for the first
    // scanline, however one of the cores will do this logic first before either
    // does the actual generation
    if (frame_num != last_frame_num) {
      last_frame_num = frame_num;
      frame_update_logic();
    }
    mutex_exit(&frame_mutex);
    //      render_scanline          start//
    struct scanvideo_scanline_buffer *dest = scanline_buffer;
    uint32_t *buf = dest->data;
    size_t buf_length = dest->data_max;
    int line_number = scanvideo_scanline_number(dest->scanline_id);
    uint16_t colorpixels = (uint16_t)line_number << 2;
    // dest->data_used = single_color_scanline(buf, buf_length, VGA_MODE.width, colorpixels);

    //convert uint32
    uint32_t pixel_output = colorpixels;

    //  single_color_scanline        start//
    assert(buf_length >= 2);
    assert(width >= MIN_COLOR_RUN);
    // | jmp color_run | color | count-3 |  buf[0] =
    buf[0] = COMPOSABLE_COLOR_RUN | (pixel_output << 16);
    buf[1] = (width - MIN_COLOR_RUN) | (COMPOSABLE_RAW_1P << 16);
    // note we must end with a black pixel
    buf[2] = 0 | (COMPOSABLE_EOL_ALIGN << 16);
    // return 3;
    //      single_color_scanline         stop//
    dest->data_used = 3;
    dest->status = SCANLINE_OK;
    //render_scanline stop//

    // Release the rendered buffer into the wild
    scanvideo_end_scanline_generation(scanline_buffer);
  }
}

void bgwxpicovga::loopCore0() {
  SPAM_GUARD_PRINT("Entered loopCore0...");
  bool run = true;
  while (run) {
    render_loop();
  }
}
void bgwxpicovga::loopCore1() {
#if DUAL_CORE
  SPAM_GUARD_PRINT("Entered loopCore1...");
  bool run = true;
  while (run) {
    render_loop();
  }
#endif
}

void bgwxpicovga::r_loop(bool block) {
  //  static uint32_t last_frame_num = 0;
  while (block) {
    struct scanvideo_scanline_buffer *scanline_buffer = scanvideo_begin_scanline_generation(true);
    uint32_t frame_num = scanvideo_frame_number(scanline_buffer->scanline_id);
    if (frame_num != last_frame_num) {
      last_frame_num = frame_num;
      frame_update_logic();
    }
    render_scanline(scanline_buffer, 0);  // core_num not used in flash_stream logic
    scanvideo_end_scanline_generation(scanline_buffer);
  }
}


// bgwxpicovga member function implementations for framebuffer_vga.h

void bgwxpicovga::setPixelXY(int x, int y, uint16_t color) {
#ifdef FRAMEBUFFER_DOUBLE
  if (x >= 0 && x < FRAMEBUFFER_DOUBLE_WIDTH && y >= 0 && y < FRAMEBUFFER_DOUBLE_HEIGHT) {
    framebuffer[y][x] = color;
  }
#else
  if (x >= 0 && x < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) {
    framebuffer[y][x] = color;
  }
#endif
}

void bgwxpicovga::drawFramebufferLine(scanvideo_scanline_buffer_t *dest) {
#ifdef FRAMEBUFFER_DOUBLE
  int line = scanvideo_scanline_number(dest->scanline_id);
  int y2 = line / 2;
  if (y2 >= FRAMEBUFFER_DOUBLE_HEIGHT) y2 = FRAMEBUFFER_DOUBLE_HEIGHT - 1;
  uint32_t *buf = dest->data;
  for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
    int x2 = x / 2;
    if (x2 >= FRAMEBUFFER_DOUBLE_WIDTH) x2 = FRAMEBUFFER_DOUBLE_WIDTH - 1;
    buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)framebuffer[y2][x2] << 16);
  }
  buf[FRAMEBUFFER_WIDTH] = COMPOSABLE_EOL_ALIGN << 16;
  dest->data_used = FRAMEBUFFER_WIDTH + 1;
  dest->status = SCANLINE_OK;
#else
  int line = scanvideo_scanline_number(dest->scanline_id);
  uint32_t *buf = dest->data;
  for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
    buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)framebuffer[line][x] << 16);
  }
  buf[FRAMEBUFFER_WIDTH] = COMPOSABLE_EOL_ALIGN << 16;
  dest->data_used = FRAMEBUFFER_WIDTH + 1;
  dest->status = SCANLINE_OK;
#endif
}

// void bgwxpicovga::exampleDrawDiagonal() {
//   for (int i = 0; i < FRAMEBUFFER_WIDTH && i < FRAMEBUFFER_HEIGHT; ++i) {
//     setPixelXY(i, i, PICO_SCANVIDEO_PIXEL_FROM_RGB8(255, 255, 255));
//   }
// }
#ifdef FRAMEBUFFER_DOUBLE
// Unified setPixelXY for doubling mode
static void setPixelXY(int x, int y, uint16_t color) {
  if (x >= 0 && x < FRAMEBUFFER_DOUBLE_WIDTH && y >= 0 && y < FRAMEBUFFER_DOUBLE_HEIGHT) {
    // framebuffer[y][x] = color;
  }
}

// // Unified drawFramebufferLine for doubling mode
// static void bgwxpicovga::drawFramebufferLine(scanvideo_scanline_buffer_t *dest) {
//   int line = scanvideo_scanline_number(dest->scanline_id);
//   int y2 = line / 2;  // Map VGA line to framebuffer line
//   if (y2 >= FRAMEBUFFER_DOUBLE_HEIGHT) y2 = FRAMEBUFFER_DOUBLE_HEIGHT - 1;
//   uint32_t *buf = dest->data;
//   for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
//     int x2 = x / 2;  // Map VGA x to framebuffer x
//     if (x2 >= FRAMEBUFFER_DOUBLE_WIDTH) x2 = FRAMEBUFFER_DOUBLE_WIDTH - 1;
//     buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)framebuffer[y2][x2] << 16);
//   }
//   buf[FRAMEBUFFER_WIDTH] = COMPOSABLE_EOL_ALIGN << 16;
//   dest->data_used = FRAMEBUFFER_WIDTH + 1;
//   dest->status = SCANLINE_OK;
// }
// #else
// // static uint16_t framebuffer[FRAMEBUFFER_HEIGHT][FRAMEBUFFER_WIDTH];

// // bgwxpicovga member function implementation
// void bgwxpicovga::setPixelXY(int x, int y, uint16_t color) {
//   if (x >= 0 && x < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) {
//     framebuffer[y][x] = color;
//   }
// }

// void bgwxpicovga::drawFramebufferLine(scanvideo_scanline_buffer_t *dest) {
//   int line = scanvideo_scanline_number(dest->scanline_id);
//   uint32_t *buf = dest->data;
//   for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
//     buf[x] = COMPOSABLE_RAW_1P | ((uint32_t)framebuffer[line][x] << 16);
//   }
//   buf[FRAMEBUFFER_WIDTH] = COMPOSABLE_EOL_ALIGN << 16;
//   dest->data_used = FRAMEBUFFER_WIDTH + 1;
//   dest->status = SCANLINE_OK;
// }
// #endif
// void bgwxpicovga::exampleDrawDiagonal() {
//   for (int i = 0; i < FRAMEBUFFER_WIDTH && i < FRAMEBUFFER_HEIGHT; ++i) {
//     setPixelXY(i, i, PICO_SCANVIDEO_PIXEL_FROM_RGB8(255, 255, 255));
//   }
// }

// --- General helpers from mandelbrot.c ---

// Fixed-point math helpers (ported, but commented out)
/*
#ifndef USE_FLOAT
#define FRAC_BITS 25u
typedef int32_t fixed;
static inline fixed float_to_fixed(float x) {
    return (fixed)(x * (float)(1u << FRAC_BITS));
}
static inline fixed double_to_fixed(double x) {
    return (fixed)(x * (double)(1u << FRAC_BITS));
}
static inline fixed fixed_mult(fixed a, fixed b) {
    int64_t r = ((int64_t)a) * b;
    return (int32_t)(r >> FRAC_BITS);
}
#else
typedef float fixed;
static inline fixed float_to_fixed(float x) { return x; }
static inline fixed double_to_fixed(double x) { return (fixed)x; }
static inline fixed fixed_mult(fixed a, fixed b) { return a * b; }
#endif
*/

// Framem_buffer definition (not Mandelbrot-specific)
// #define M_FRAMEBUFFER_WIDTH 320
// #define M_FRAMEBUFFER_HEIGHT 240
// static uint16_t framebuffer[M_FRAMEBUFFER_WIDTH * M_FRAMEBUFFER_HEIGHT];
#endif
// bgwxpicovga member function implementation
void bgwxpicovga::m_fillFramebufferWithColor(uint16_t color) {
  uint16_t *fb = &framebuffer[0][0];
  for (int i = 0; i < FRAMEBUFFER_DOUBLE_WIDTH * FRAMEBUFFER_DOUBLE_HEIGHT; ++i) {
    fb[i] = color;
  }
}

// //  m_fillscanline_buffer: Fills scanvideo scanline m_buffer from framebuffer
void bgwxpicovga::colorSet(uint8_t red_set, uint8_t green_set, uint8_t blue_set) {
  // Create specific colors
  m_user_framebuffer[fb_count] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(red_set, green_set, blue_set);  // Red gradient
  fb_count++;
  if (fb_count >= SCREEN_WIDTH) {
    fb_count = 0;
    fb_count_send = 0;
  }
  // uint16_t green_color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, green, 0);            // Green gradient
  // uint16_t blue_color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 0, blue);              // Blue gradient
  // uint16_t background_color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(red, green, blue);  // Mixed RGB gradient
}
// void bgwxpicovga::m_fillScanlineBuffer(struct scanvideo_scanline_buffer *buffer) {
void bgwxpicovga::timer_send_buffer(struct scanvideo_scanline_buffer *buffer) {
  // Map VGA scanline to framebuffer row (each framebuffer row is displayed twice
  // for pixel doubling: 320x240 framebuffer -> 640x480 VGA output)
  uint line = scanvideo_scanline_number(buffer->scanline_id);
  int fb_y = (int)(line / 2);
  if (fb_y < 0) fb_y = 0;
  if (fb_y >= FRAMEBUFFER_DOUBLE_HEIGHT) fb_y = FRAMEBUFFER_DOUBLE_HEIGHT - 1;
  uint16_t *pixels = framebuffer[fb_y];

  // Use the raw scanline approach (ported from Mandelbrot timing structure):
  // raw_scanline_prepare sets up a COMPOSABLE_RAW_RUN covering FRAMEBUFFER_WIDTH
  // pixels and returns a uint16_t pointer where we write pixel data directly.
  uint16_t *buf = ::raw_scanline_prepare(buffer, FRAMEBUFFER_WIDTH);
  for (int x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
    // Read from the 320-wide framebuffer at half the x-coordinate to achieve
    // 2x horizontal scaling (each framebuffer pixel maps to 2 VGA pixels).
    buf[x] = pixels[x / 2];
  }
  // raw_scanline_finish pivots the first two words for the PIO state machine
  // and marks the scanline ready — this is the timing-critical step.
  ::raw_scanline_finish(buffer);
}
// static inline void source_m_fillscanline_buffer(struct scanvideo_scanline_buffer *m_buffer) {
//     static uint32_t postamble[] = {0x0000u | (COMPOSABLE_EOL_ALIGN << 16)};

//     m_buffer->data[0] = 4;
//     m_buffer->data[1] = host_safe_hw_ptr(m_buffer->data + 8);
//     m_buffer->data[2] = 158; // first four pixels are handled separately
//     uint16_t *pixels = framebuffer + scanvideo_scanline_number(m_buffer->scanline_id) * M_FRAMEBUFFER_WIDTH;
//     m_buffer->data[3] = host_safe_hw_ptr(pixels + 4);
//     m_buffer->data[4] = sizeof(postamble) / sizeof(postamble[0]);
//     m_buffer->data[5] = host_safe_hw_ptr(postamble);
//     m_buffer->data[6] = 0;
//     m_buffer->data[7] = 0;
//     m_buffer->data_used = 8;

//     // 3 pixel run followed by main run, consuming the first 4 pixels
//     m_buffer->data[8] = (pixels[0] << 16u) | COMPOSABLE_RAW_RUN;
//     m_buffer->data[9] = (pixels[1] << 16u) | 0;
//     m_buffer->data[10] = (COMPOSABLE_RAW_RUN << 16u) | pixels[2];
//     m_buffer->data[11] = ((M_FRAMEBUFFER_WIDTH - 3) << 16u) | pixels[3];
// }

// m_timer_callback: Handles scanline m_buffer filling in timer context
int64_t bgwxpicovga::m_timerCallback(alarm_id_t alarm_id, void *user_data) {
  SPAM_GUARD_PRINT("m_timerCallback !!");
  // SerialDbg.println("-----------------------------");
  bgwxpicovga *self = static_cast<bgwxpicovga *>(user_data);
  struct scanvideo_scanline_buffer *dest = scanvideo_begin_scanline_generation(false);
  while (dest) {
    // if (self) {
    self->timer_send_buffer(dest);
    // }
    scanvideo_end_scanline_generation(dest);
    dest = scanvideo_begin_scanline_generation(false);
  }
  // SerialDbg.println("-----------------------------");
  return 100;
}

// m_raw_scanline_finish: Finalizes scanline m_buffer for output
void bgwxpicovga::m_rawScanlineFinish(struct scanvideo_scanline_buffer *dest) {
  uint32_t first = dest->data[0];
  uint32_t second = dest->data[1];
  dest->data[0] = (first & 0x0000ffffu) | ((second & 0x0000ffffu) << 16);
  dest->data[1] = (second & 0xffff0000u) | ((first & 0xffff0000u) >> 16);
  dest->status = SCANLINE_OK;
}

void bgwxpicovga::render_loop() {
  int core_num = get_core_num();
  printf("Rendering on core %d\n", core_num);
  while (true) {
    mutex_enter_blocking(&frame_mutex);
    // When all rows of the framebuffer have been filled, reset for next frame
    if (y == FRAMEBUFFER_DOUBLE_HEIGHT) {
      params_ready = false;
      frame_update_logic();
      y = 0;
    }
    uint _y = y++;
    mutex_exit(&frame_mutex);

    // Fill framebuffer row _y with custom user data.
    // If a buffer callback is registered, call it to let the user write pixels
    // into the row. Otherwise the row stays black (zeroed at startup).
    if (bufferCallbackFn) {
      bufferCallbackFn(framebuffer[_y], (int)_y);
    }

#if !PICO_ON_DEVICE
    // On non-device (simulator): drive scanlines directly from render_loop
    struct scanvideo_scanline_buffer *buffer = scanvideo_begin_scanline_generation(true);
    timer_send_buffer(buffer);
    scanvideo_end_scanline_generation(buffer);
#endif
  }
}

bgwxpicovga::bgwxpicovga() {}
bgwxpicovga::~bgwxpicovga() {}
#endif
