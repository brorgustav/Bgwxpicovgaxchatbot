// Sample input_provider for mandlebuffer_stream_continuous using Serial.read:
//
// bool serial_input_provider(uint y, mandlebuffer_input_t *input) {
//     static uint indices[16];
//     static uint16_t colors[16];
//     input->count = 0;
//     input->has_data = false;
//     // Example protocol: each pixel is sent as 4 bytes: index, r, g, b
//     while (Serial.available() >= 4 && input->count < 16) {
//         uint idx = Serial.read();
//         uint8_t r = Serial.read();
//         uint8_t g = Serial.read();
//         uint8_t b = Serial.read();
//         indices[input->count] = idx;
//         colors[input->count] = PICO_SCANVIDEO_PIXEL_FROM_RGB8(r, g, b);
//         input->count++;
//     }
//     if (input->count > 0) {
//         input->indices = indices;
//         input->colors = colors;
//         input->has_data = true;
//         return true;
//     }
//     return false;
// }
//
// Usage:
// mandlebuffer_stream_continuous(serial_input_provider);
// Continuous stream: fills scanlines with black unless input data is provided
// input_provider: function pointer returning input for a given scanline
// Example signature: bool input_provider(uint y, mandlebuffer_input_t *input);
typedef struct {
	const uint *indices;
	const uint16_t *colors;
	uint count;
	bool has_data;
} mandlebuffer_input_t;

void mandlebuffer_stream_continuous(bool (*input_provider)(uint y, mandlebuffer_input_t *input));
// Example usage for mandlebuffer_stream_sparse_line:
//
//   uint indices[] = {10, 50, 100};
//   uint16_t colors[] = {
//       PICO_SCANVIDEO_PIXEL_FROM_RGB8(255,0,0),
//       PICO_SCANVIDEO_PIXEL_FROM_RGB8(0,255,0),
//       PICO_SCANVIDEO_PIXEL_FROM_RGB8(0,0,255)
//   };
//   mandlebuffer_stream_sparse_line(indices, colors, 3, 42); // Draws 3 colored pixels on line 42, rest black
// Stream a sparse scanline: set only specified indices/colors, fill rest with black
// data_indices: array of pixel indices to set (e.g., [5, 20, 100])
// data_colors: array of colors (same length as data_indices)
// count: number of data points
// y: scanline to write
void mandlebuffer_stream_sparse_line(const uint *data_indices, const uint16_t *data_colors, uint count, uint y);
// Fill the entire framebuffer with random pixel data
void mandlebuffer_fill_random(void);

// Fill a single scanline with random pixel data and stream it to the framebuffer
void mandlebuffer_stream_random_line(uint y);
// Stream a full image (2D array) into the framebuffer
// image: pointer to array of size (height x width) in row-major order
// width, height: dimensions of the image
// x, y: top-left position in framebuffer
// Clips if image exceeds framebuffer bounds
bool mandlebuffer_stream_image(const uint16_t *image, uint width, uint height, uint x, uint y);

// Stream a vertical column of pixels into the framebuffer
// data: pointer to array of pixel values
// len: number of pixels
// x, y: start position (writes downwards)
// Clips if column exceeds framebuffer bounds
bool mandlebuffer_stream_column(const uint16_t *data, uint len, uint x, uint y);
// mandlebuffer.h
// Header for framebuffer and timing structure

#ifndef MANDLEBUFFER_H
#define MANDLEBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include "pico/scanvideo.h"

#define FRAMEBUFFER_WIDTH 320
#define FRAMEBUFFER_HEIGHT 240

extern uint16_t framebuffer[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT];

void fill_scanline_buffer(struct scanvideo_scanline_buffer *buffer);
void render_loop();
void framebuffer_vga_init();

// User must implement this to fill a scanline with custom data
void generate_scanline(uint16_t *line_buffer, uint length, uint y);

// Stream a block of pixel data into the framebuffer at a given (x, y) position
// data: pointer to array of uint16_t pixel values (PICO_SCANVIDEO_PIXEL_FROM_RGB8 format)
// len: number of pixels to write
// x, y: start position in framebuffer (x is column, y is row)
// Returns true if successful, false if out of bounds
bool mandlebuffer_stream_pixels(const uint16_t *data, uint len, uint x, uint y);

#endif // MANDLEBUFFER_H
