// rgb_scanline_example.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define WIDTH 640
#define HEIGHT 480

// Function to render a single scanline
void render_scanline(uint8_t *buffer, int y) {
    for (int x = 0; x < WIDTH; x++) {
        buffer[y * WIDTH + x] = (x + y) % 256; // Example pixel value
    }
}

// Main function demonstrating frame management
int main() {
    uint8_t *frame_buffer = (uint8_t *)malloc(WIDTH * HEIGHT);
    if (!frame_buffer) {
        fprintf(stderr, "Failed to allocate memory for frame buffer\n");
        return -1;
    }

    while (1) {
        clock_t start_time = clock();
        for (int y = 0; y < HEIGHT; y++) {
            render_scanline(frame_buffer, y);
        }
        // Synchronize frame timing
        clock_t end_time = clock();
        double elapsed = (double)(end_time - start_time) / CLOCKS_PER_SEC;
        printf("Frame rendered in %.3f seconds\n", elapsed);
        
        // Implement synchronization logic here if needed
    }

    free(frame_buffer);
    return 0;
}