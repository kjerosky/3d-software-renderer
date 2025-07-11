#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <algorithm>

#include "TriangleRasterizer.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Surface* texture_surface = nullptr;

// --------------------------------------------------------------------------

void cleanup() {
    if (texture_surface != nullptr) {
        SDL_DestroySurface(texture_surface);
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
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

    window = SDL_CreateWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "[ERROR] SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::cerr << "[ERROR] SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    texture_surface = IMG_Load("test-texture.png");
    if (texture_surface == nullptr) {
        std::cerr << "[ERROR] IMG_Load error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    const SDL_PixelFormatDetails* pixel_format_details = SDL_GetPixelFormatDetails(texture_surface->format);
    if (pixel_format_details->bytes_per_pixel != 4) {
        SDL_Surface* converted_surface = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA8888);
        if (converted_surface == nullptr) {
            std::cerr << "[ERROR] SDL_ConvertSurface could not convert loaded image surface to RGBA8888 pixel format: " << SDL_GetError() << std::endl;
            cleanup();
            return 1;
        } else {
            SDL_DestroySurface(texture_surface);
            texture_surface = converted_surface;
        }
    }

    TriangleRasterizer triangle_rasterizer;

    bool is_rasterizing_textures = false;

    const bool* keyboard_state = SDL_GetKeyboardState(nullptr);
    bool previous_render_mode_key_state = false;

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

        const bool current_render_mode_key_state = keyboard_state[SDL_SCANCODE_SPACE];
        if (!previous_render_mode_key_state && current_render_mode_key_state) {
            is_rasterizing_textures = !is_rasterizing_textures;
        }
        previous_render_mode_key_state = current_render_mode_key_state;

        SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
        SDL_RenderClear(renderer);

        int render_width, render_height;
        SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);

        const float half_side_length = std::min(render_width * 0.75f, render_height * 0.75f) / 2.0f;
        Triangle tri1 = {
            {{ render_width / 2.0f - half_side_length, render_height / 2.0f + half_side_length }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{ render_width / 2.0f + half_side_length, render_height / 2.0f + half_side_length }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
            {{ render_width / 2.0f + half_side_length, render_height / 2.0f - half_side_length }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
        };
        Triangle tri2 = {
            {{ render_width / 2.0f - half_side_length, render_height / 2.0f + half_side_length }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{ render_width / 2.0f + half_side_length, render_height / 2.0f - half_side_length }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
            {{ render_width / 2.0f - half_side_length, render_height / 2.0f - half_side_length }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        };

        SDL_Surface* render_texture_surface = nullptr;
        if (is_rasterizing_textures) {
            render_texture_surface = texture_surface;
        }

        triangle_rasterizer.rasterize(renderer, tri1, render_texture_surface);
        triangle_rasterizer.rasterize(renderer, tri2, render_texture_surface);

        SDL_RenderPresent(renderer);
    }

    // --- cleanup and quit ---

    cleanup();
    return 0;
}
