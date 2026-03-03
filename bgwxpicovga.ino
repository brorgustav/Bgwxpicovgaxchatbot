#define DEBUG_VGA (true)
#define FRAMEBUFFER_ENABLED
#define SerialDbg Serial
#define SYSCLOCK_125mhz true
#define DUAL_CORE false

#define mandelbrot -compile

// #define DPI_MODE_ENABLED
#if DEBUG_VGA
#define SPAM_GUARD_PRINT(msg)                                                  \
  do {                                                                         \
    static unsigned long _lastPrint = 0;                                       \
    static char _lastMsg[128] = "";                                            \
    unsigned long _now = millis();                                             \
    if ((_now - _lastPrint > 100) || strcmp(_lastMsg, msg)) {                  \
      SerialDbg.println(msg);                                                  \
      strncpy(_lastMsg, msg, sizeof(_lastMsg) - 1);                            \
      _lastMsg[sizeof(_lastMsg) - 1] = '\0';                                   \
      _lastPrint = _now;                                                       \
    }                                                                          \
  } while (0)
#else
do {
  return;
}
#endif
// Select VGA mode: 640x480 @ 60Hz (standard VGA timing)
#define VGA_MODE vga_mode_640x480_60
//  #define FRAMEBUFFER //if defined = real framebuffer, no doubling
#define FRAMEBUFFER_ENABLED
#define FRAMEBUFFER_DOUBLE
#define TEST_BAR
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define VGA_CORE 0
#include "bgwxpicovga.h"
// Include the mandlebuffer structure
#include "mandlebuffer.h"
//  set_sys_clock_khz(200000, true); ??

/*
VGA ENGINE WORKFLOW
    Begin scanline generation.
    Get a scanline buffer.
    Fill it with pixel data.
    End the scanline generation.

        ## compile
arduino-cli compile --fqbn rp2040:rp2040:raspberrypi_pico:freq=125
bgwxpicovga/bgwxpicovga.ino

        ## UPLOAD
arduino-cli upload -p /dev/cu.usbmodem101 --fqbn
rp2040:rp2040:raspberrypi_pico:freq=125 bgwxpicovga/bgwxpicovga.ino

      ########################################################################################
      ########################################################################################
      ## IMPORTANT: Make sure you run the CPU at 125mhz
                     otherwise the timings
                     will be wrong
      ########################################################################################
      ########################################################################################

      #setFrameCallback(myScanlineRenderer); // Set the scanline renderer
callback
 * bgwxpicovga - VGA output for Raspberry Pi Pico 1 with Pimoroni VGA Demo
Base
 *
 * Uses the pico-extras scanvideo library (pico_scanvideo_dpi) to generate
 * VGA video signals via PIO and DMA. Both CPU cores are used for scanline
 * generation to keep up with the display refresh rate.
 *
 * Target hardware:
 *   - Raspberry Pi Pico 1 (RP2040)
 *   - Pimoroni VGA Demo Base (same pinout as Raspberry Pi RP2040 VGA
reference)
 *
 * Pin assignments and more details in the comment block above.
 *
 * Arduino core: earlephilhower/arduino-pico
 *   https://github.com/earlephilhower/arduino-pico
 *
 * pico-extras scanvideo library:
 *
https://github.com/raspberrypi/pico-extras/tree/master/src/common/pico_scanvideo
 vga_timing_648x480_60_alt1;

extern const scanvideo_mode_t vga_mode_160x120_60; // 3d monster maze
anyone :-) extern const scanvideo_mode_t vga_mode_213x160_60; extern const
scanvideo_mode_t vga_mode_320x240_60; extern const scanvideo_mode_t
vga_mode_640x480_60; extern const scanvideo_mode_t vga_mode_800x600_54;
extern const scanvideo_mode_t vga_mode_800x600_60;
extern const scanvideo_mode_t vga_mode_1024x768_63;
extern const scanvideo_mode_t vga_mode_1280x1024_40;
extern const scanvideo_mode_t vga_mode_1024x768_60;
extern const scanvideo_mode_t vga_mode_1280x1024_60;
extern const scanvideo_mode_t vga_mode_720p_60;
extern const scanvideo_mode_t vga_mode_1080p_60;
extern const scanvideo_mode_t vga_mode_1440p_60;

extern const scanvideo_mode_t vga_mode_tft_800x480_50;
extern const scanvideo_mode_t vga_mode_tft_400x240_50;
*/

// Global variables for color values (not used in this minimal example)
int red_val;
int green_val;
int blue_val;
int frameCount;
bool core1_callback = false;

bgwxpicovga VGA; // Global instance of the VGA class
// Arduino entry point for Core 0
// Initializes synchronization primitives and scanvideo engine
int second_count;
int value;
//  uint16_t lineBuf()

// void linebuffer() {}

uint16_t buffer_black = PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 0, 0);
bool fill_new_buffer;
bool new_buffer_input;
bool new_buffer_input_ready;
int fill_new_buffer_count;
uint16_t tx_buffer[SCREEN_WIDTH];
// This function will be called for each scanline that needs to be rendered
// You can implement your scanline rendering logic here, using the provided
// buffer and scanline information.

// Implementation of generate_scanline for framebuffer_vga
// This is where you customize the framebuffer content per scanline
extern "C" void generate_scanline(uint16_t *line_buffer, uint length, uint y) {
  // Example: fill with a color gradient based on y
  for (uint x = 0; x < length; ++x) {
    // Simple color pattern: vertical gradient
    uint8_t r = (y * 255) / SCREEN_HEIGHT;
    uint8_t g = (x * 255) / SCREEN_WIDTH;
    uint8_t b = 128;
    line_buffer[x] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b);
  }
}

void buffer_poll() {
  if (fill_new_buffer_count >= SCREEN_WIDTH) {
    fill_new_buffer_count = 0;
  }
  if (!new_buffer_input) {
    fill_new_buffer_count++;
    tx_buffer[fill_new_buffer_count] = buffer_black;
  }
  while (new_buffer_input) {
    if (new_buffer_input_ready) {
      new_buffer_input = false;
      fill_new_buffer_count = 0;
    }
  }
}

void buffer_write(uint16_t r_val, uint16_t g_val, uint16_t b_val) {
  new_buffer_input = true;
  fill_new_buffer_count++;
  if (fill_new_buffer_count >= SCREEN_WIDTH) {
    fill_new_buffer_count = 0;
  }
  uint16_t color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(r_val, g_val, b_val);
  tx_buffer[fill_new_buffer_count] = color;
  new_buffer_input_ready = true;
}
void setPixelInScanline(int x, uint16_t rtx, uint16_t gtx, uint16_t btx) {
  // if (x >= 0 && x < SCREEN_WIDTH) {
  //   uint16_t color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(rtx, gtx, btx);
  //   user_scanline_buffer[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
  // }
}
void send_buffer() {
  // scanvideo_scanline_buffer_t *vga =
  // scanvideo_begin_scanline_generation(true); for (int x = 0; x <
  // SCREEN_WIDTH; x++) {
  //   uint32_t *vga = vga->data;
  //   uint16_t color = PICO_SCANVIDEO_PIXEL_FROM_RGB8(rtx, gtx, btx);
  //   vga[x] = COMPOSABLE_RAW_1P | ((uint32_t)color << 16);
  // }
  // vga[SCREEN_WIDTH] = COMPOSABLE_EOL_ALIGN << 16;
  // vga->data_used = SCREEN_WIDTH + 1;
  // vga->status = SCANLINE_OK;
  // scanvideo_end_scanline_generation(vga);
  VGA.sendUserBuffer(tx_buffer, SCREEN_WIDTH);
}

// --- Double buffering for scanlines ---
static uint16_t scanline_buffers[2][SCREEN_WIDTH];
static int next_scanline_idx = 0;

void myBufferCallback(uint16_t *colour_buf, int scanline) {
  // Fill this framebuffer row (320 pixels wide, for FRAMEBUFFER_DOUBLE mode)
  // with custom pixel data. Each pixel here maps to 2x2 on the 640x480 display.
  // Replace this with your own image data, palette lookup, or other logic.
  for (int x = 0; x < FRAMEBUFFER_DOUBLE_WIDTH; ++x) {
    uint8_t r = (uint8_t)(x % 256);
    uint8_t g = (uint8_t)(scanline % 256);
    uint8_t b = (uint8_t)((x + scanline) % 256);
    colour_buf[x] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b);
  }
}

// Placeholder for sending a VGA color (not used in this example)
void send_vga(int redValue, int greenValue, int blueValue) {
  // This function could be used to update color values or trigger output
}
void myFrameCallback(uint32_t frame) {
  core1_callback = true;
  // tempbuffer_test();

  // SPAM_GUARD_PRINT("myFrameCallback: ");
  SerialDbg.print("myFrameCallback: ");
  SerialDbg.println(frame);
  SerialDbg.println(frame);
}
// // Example scanline renderer: sets a single pixel to red on each scanline
// void myScanlineRenderer(scanvideo_scanline_buffer_t *dest) {
//   // Set pixel 100 to red on the current scanline
//   setPixel(dest, 100, VGA.getRGB(255, 0, 0));
//   // Mark scanline as ready for output
//   VGA.sendVGA(dest);
// }
void inc_vals() {
  red_val += 20;
  green_val += 22;
  blue_val += 23;
}
// void tempbuffer_test() {
//   uint16_t temp_buffer[SCREEN_WIDTH];
//   for (int x = 0; x < SCREEN_WIDTH; ++x) {
//     uint8_t r = random(0, 256);
//     uint8_t g = random(0, 256);
//     uint8_t b = random(0, 256);
//     // scanline_buffers[fill_idx][x] = VGA.getRGB(r, g, b);
//     temp_buffer[x] = VGA.getRGB(r, g, b);
//   }
//   VGA.storeBuffer(temp_buffer, SCREEN_WIDTH);
// }
void myLineCallback(uint16_t line_number) {

  SerialDbg.print("myLineCallback: ");
  SerialDbg.println(line_number);
  core1_callback = true;
}

uint16_t temp_buffer[SCREEN_WIDTH];
int send_x = 0;

void buffer_tester() {
  // core1_callback = true;
  if (send_x < SCREEN_WIDTH && core1_callback == true) {
    uint8_t r = random(0, 256);
    uint8_t g = random(0, 256);
    uint8_t b = random(0, 256);
    // scanline_buffers[fill_idsend_x][send_x] = VGA.getRGB(r, g, b);
    Serial.print("temp_buffer=[");
    Serial.print(send_x);
    Serial.println("]");
    temp_buffer[send_x] = VGA.getRGB(r, g, b);
    // temp_buffer[send_x] = COMPOSABLE_RAW_1P | (temp_buffer[send_x] << 16);
    send_x++;
  }
  // int width = VGA_MODE.width;
  //  int pixel_x = 33;  //placeholder value

  if (send_x >= SCREEN_WIDTH) {
    Serial.println("send_x >= SCREEN_WIDTH && core1_callback == true");
    Serial.print("temp_buffer[");
    Serial.print(send_x);
    Serial.println("]");
    // temp_buffer[SCREEN_WIDTH] = COMPOSABLE_EOL_ALIGN << 16;
    send_x = 0;
    VGA.m_framebuffer_store(temp_buffer, SCREEN_WIDTH);
    // int scanline_pos = VGA.getScanline();
    // VGA.storeBuffer2(temp_buffer, SCREEN_WIDTH, scanline_pos);
    core1_callback = false;
  }
}

// Arduino main loop for Core 0

long millisPrev = 0;
long millisDelay = 10000; // the delay period

void setup() {
  // set_sys_clock_khz(125000, true);
  SerialDbg.begin(115200);
  SerialDbg.println("system clock set to 125mhz");
  SerialDbg.println("");
  SerialDbg.println("");
  SerialDbg.println("");
  delay(2500);
  // SerialDbg.println("  set_sys_clock_khz(125000, true);");

  // Initialize VGA, mutexes, and semaphores

  // SPAM_GUARD_PRINT("setup: VGA.begin() called");

  // #ifdef VGA_CORE 0
  //   {
  SerialDbg.print("[CORE 0] Starting VGA on core: ");
  SerialDbg.println("#0");
  delay(2500);
  // VGA.setBufferCallback(myBufferCallback);
  // VGA.setFrameCallback(myFrameCallback);
  // VGA.setLineCallback(myLineCallback);
  framebuffer_vga_init();
  VGA.begin();
  // }
}
// #endif

// Arduino entry point for Core 1 (if used)
// Waits for Core 0 to finish setup before starting
void setup1() {
  SerialDbg.begin(115200);
  VGA.begin();
  // SPAM_GUARD_PRINT("setup1: VGA.begin() done");
  // sem_acquire_blocking(&core1_start_sem);
}

bool debug_switch_enable = false;
bool switcher = false;

void loop() {
  // SPAM_GUARD_PRINT("[CORE 0] loop: entered main loop");
  // VGA.loopCore0();
  // VGA.minimal_loop_backup();
  // VGA.debugLoop(7000);
}

void loop1() {
  // SPAM_GUARD_PRINT("[CORE 1] loop: entered main loop");
  // VGA.loopCore1();
  buffer_tester();
  // uint8_t r = random(0, 256);
  // uint8_t g = random(0, 256);
  // uint8_t b = random(0, 256);
  // scanline_buffers[fill_idx][x] = VGA.getRGB(r, g, b);
  // VGA.sendStream(r, g, b);

  // buffer_tester();
}
