// Continuous stream: fills scanlines with black unless input data is provided
// Call this in your main loop. If input data is available, it will be drawn; otherwise, the scanline is filled with black.
// Provide a callback or function to fetch input data for each scanline.
typedef struct {
    const uint *indices;
    const uint16_t *colors;
    uint count;
    bool has_data;
} mandlebuffer_input_t;

// input_provider: function pointer returning input for a given scanline
// Example signature: mandlebuffer_input_t input_provider(uint y);
void mandlebuffer_stream_continuous(bool (*input_provider)(uint y, mandlebuffer_input_t *input)) {
    for (uint y = 0; y < FRAMEBUFFER_HEIGHT; ++y) {
        mandlebuffer_input_t input;
        input.has_data = false;
        if (input_provider && input_provider(y, &input) && input.has_data && input.count > 0) {
            mandlebuffer_stream_sparse_line(input.indices, input.colors, input.count, y);
        } else {
            // Fill scanline with black
            uint16_t line[FRAMEBUFFER_WIDTH];
            for (uint x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
                line[x] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 0, 0);
            }
            mandlebuffer_stream_pixels(line, FRAMEBUFFER_WIDTH, 0, y);
        }
    }
}
// Stream a sparse scanline: set only specified indices/colors, fill rest with black
// data_indices: array of pixel indices to set (e.g., [5, 20, 100])
// data_colors: array of colors (same length as data_indices)
// count: number of data points
// y: scanline to write
void mandlebuffer_stream_sparse_line(const uint *data_indices, const uint16_t *data_colors, uint count, uint y) {
    if (y >= FRAMEBUFFER_HEIGHT) return;
    uint16_t line[FRAMEBUFFER_WIDTH];
    // Fill with black
    for (uint x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
        line[x] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(0, 0, 0);
    }
    // Set specified pixels
    for (uint i = 0; i < count; ++i) {
        uint x = data_indices[i];
        if (x < FRAMEBUFFER_WIDTH) {
            line[x] = data_colors[i];
        }
    }
    mandlebuffer_stream_pixels(line, FRAMEBUFFER_WIDTH, 0, y);
}
// Fill the entire framebuffer with random pixel data
void mandlebuffer_fill_random(void) {
    for (uint y = 0; y < FRAMEBUFFER_HEIGHT; ++y) {
        for (uint x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
            uint8_t r = rand() % 256;
            uint8_t g = rand() % 256;
            uint8_t b = rand() % 256;
            framebuffer[y * FRAMEBUFFER_WIDTH + x] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b);
        }
    }
}

// Fill a single scanline with random pixel data and stream it to the framebuffer
void mandlebuffer_stream_random_line(uint y) {
    if (y >= FRAMEBUFFER_HEIGHT) return;
    uint16_t line[FRAMEBUFFER_WIDTH];
    for (uint x = 0; x < FRAMEBUFFER_WIDTH; ++x) {
        uint8_t r = rand() % 256;
        uint8_t g = rand() % 256;
        uint8_t b = rand() % 256;
        line[x] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b);
    }
    mandlebuffer_stream_pixels(line, FRAMEBUFFER_WIDTH, 0, y);
}
// Stream a full image (2D array) into the framebuffer
// image: pointer to array of size (height x width) in row-major order
// width, height: dimensions of the image
// x, y: top-left position in framebuffer
// Clips if image exceeds framebuffer bounds
bool mandlebuffer_stream_image(const uint16_t *image, uint width, uint height, uint x, uint y) {
    if (x >= FRAMEBUFFER_WIDTH || y >= FRAMEBUFFER_HEIGHT) return false;
    uint max_h = FRAMEBUFFER_HEIGHT - y;
    if (height > max_h) height = max_h;
    for (uint row = 0; row < height; ++row) {
        const uint16_t *src = image + row * width;
        mandlebuffer_stream_pixels(src, width, x, y + row);
    }
    return true;
}

// Stream a vertical column of pixels into the framebuffer
// data: pointer to array of pixel values
// len: number of pixels
// x, y: start position (writes downwards)
// Clips if column exceeds framebuffer bounds
bool mandlebuffer_stream_column(const uint16_t *data, uint len, uint x, uint y) {
    if (x >= FRAMEBUFFER_WIDTH || y >= FRAMEBUFFER_HEIGHT) return false;
    uint max_len = FRAMEBUFFER_HEIGHT - y;
    if (len > max_len) len = max_len;
    for (uint i = 0; i < len; ++i) {
        framebuffer[(y + i) * FRAMEBUFFER_WIDTH + x] = data[i];
    }
    return true;
}
// mandlebuffer.c
// Extracted framebuffer and timing structure from Mandelbrot example for reuse
// Custom pixel data generation should be provided by the user

#include <stdint.h>
#include <stdbool.h>
#include "pico/scanvideo.h"
#include "pico/sync.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "mandlebuffer.h"

#define FRAMEBUFFER_WIDTH 320
#define FRAMEBUFFER_HEIGHT 240

struct mutex frame_logic_mutex;
static void frame_update_logic();

static uint y;
static bool params_ready;

uint16_t framebuffer[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];

// User must provide this function to fill a scanline in the framebuffer
// Example: void generate_scanline(uint16_t *line_buffer, uint length, uint y);
void generate_scanline(uint16_t *line_buffer, uint length, uint y);

// Stream a block of pixel data into the framebuffer at a given (x, y) position
// data: pointer to array of uint16_t pixel values (PICO_SCANVIDEO_PIXEL_FROM_RGB8 format)
// len: number of pixels to write
// x, y: start position in framebuffer (x is column, y is row)
// Returns true if successful, false if out of bounds
bool mandlebuffer_stream_pixels(const uint16_t *data, uint len, uint x, uint y) {
    if (y >= FRAMEBUFFER_HEIGHT || x >= FRAMEBUFFER_WIDTH) return false;
    uint max_len = FRAMEBUFFER_WIDTH - x;
    if (len > max_len) len = max_len;
    uint16_t *dest = framebuffer + y * FRAMEBUFFER_WIDTH + x;
    for (uint i = 0; i < len; ++i) {
        dest[i] = data[i];
    }
    return true;
}

void fill_scanline_buffer(struct scanvideo_scanline_buffer *buffer) {
    static uint32_t postamble[] = {
            0x0000u | (COMPOSABLE_EOL_ALIGN << 16)
    };

    buffer->data[0] = 4;
    buffer->data[1] = host_safe_hw_ptr(buffer->data + 8);
    buffer->data[2] = 158; // first four pixels are handled separately
    uint16_t *pixels = framebuffer + scanvideo_scanline_number(buffer->scanline_id) * FRAMEBUFFER_WIDTH;
    buffer->data[3] = host_safe_hw_ptr(pixels + 4);
    buffer->data[4] = 1;
    buffer->data[5] = host_safe_hw_ptr(postamble);
    buffer->data[6] = 0;
    buffer->data[7] = 0;
    buffer->data_used = 8;

    buffer->data[8] = (pixels[0] << 16u) | COMPOSABLE_RAW_RUN;
    buffer->data[9] = (pixels[1] << 16u) | 0;
    buffer->data[10] = (COMPOSABLE_RAW_RUN << 16u) | pixels[2];
    buffer->data[11] = ((FRAMEBUFFER_WIDTH - 3 + 1) << 16u) | pixels[3];
}

void render_loop() {
    while (true) {
        mutex_enter_blocking(&frame_logic_mutex);
        if (y == FRAMEBUFFER_HEIGHT) {
            params_ready = false;
            frame_update_logic();
            y = 0;
        }
        uint _y = y++;
        mutex_exit(&frame_logic_mutex);

        // Fill framebuffer scanline with user data
        generate_scanline(framebuffer + _y * FRAMEBUFFER_WIDTH, FRAMEBUFFER_WIDTH, _y);
    }
}

void framebuffer_vga_init() {
    mutex_init(&frame_logic_mutex);
    frame_update_logic();
    y = 0;
    params_ready = true;
}

// Dummy frame_update_logic, user can override if needed
void __attribute__((weak)) frame_update_logic() {}
