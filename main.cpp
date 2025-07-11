#include <SDL3/SDL.h>
#include <iostream>
#include <algorithm>

#include "TriangleRasterizer.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// --------------------------------------------------------------------------

void cleanup() {
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }

    SDL_Quit();
}

// --------------------------------------------------------------------------

int main() {
    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 360;

    // --- sdl3 setup ---

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "[ERROR] SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "[ERROR] SDL_CreateRenderer error:" << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    TriangleRasterizer triangle_rasterizer;

    // --- main loop ---

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int render_width, render_height;
        SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);

        float side_length = std::min(render_width * 0.75f, render_height * 0.75f);
        float height = side_length / 2.0f * tanf(M_PI / 3.0f);
        Triangle equilateral_triangle = {
            {{ render_width / 2.0f - side_length / 2.0f, render_height / 2.0f + height / 2.0f}, { 1.0f, 0.0f, 0.0f }},
            {{ render_width / 2.0f + side_length / 2.0f, render_height / 2.0f + height / 2.0f}, { 0.0f, 1.0f, 0.0f }},
            {{ render_width / 2.0f, render_height / 2.0f - height / 2.0f}, { 0.0f, 0.0f, 1.0f }},
        };
        triangle_rasterizer.rasterize(renderer, equilateral_triangle);

        SDL_RenderPresent(renderer);
    }

    // --- cleanup and quit ---

    cleanup();
    return 0;
}
