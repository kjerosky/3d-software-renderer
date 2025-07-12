#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <string>

#include "TriangleRasterizer.h"
#include "Primitives.h"

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

    const std::string WINDOW_TITLE = "3d Software Renderer";

    // --- sdl3 setup ---

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
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

    texture_surface = IMG_Load("resources/test-texture.png");
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

    SDL_SetRenderVSync(renderer, 1);

    TriangleRasterizer triangle_rasterizer;

    Object main_object = primitives::cuboid(2.0f, 2.0f, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    Object background_object = primitives::cuboid(1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 5.0f);

    bool is_rasterizing_textures = true;

    const bool* keyboard_state = SDL_GetKeyboardState(nullptr);
    bool previous_render_mode_key_state = false;

    const float ROTATION_DEGREES_Y_PER_SECOND = 360.0f / 8.0f;
    const float ROTATION_DEGREES_X_PER_SECOND = 360.0f / 16.0f;
    float rotation_degrees_y = 0.0f;
    float rotation_degrees_x = 0.0f;

    Uint64 previous_timestamp = SDL_GetTicks();

    float seconds_left_until_fps_report = 1.0f;
    int frame_count = 0;

    // --- main loop ---

    bool running = true;
    SDL_Event event;
    while (running) {
        Uint64 current_timestamp = SDL_GetTicks();
        float delta_time = (current_timestamp - previous_timestamp) / 1000.0f;
        previous_timestamp = current_timestamp;

        frame_count++;
        seconds_left_until_fps_report -= delta_time;
        if (seconds_left_until_fps_report <= 0.0f) {
            SDL_SetWindowTitle(window, (WINDOW_TITLE + std::string(" | FPS: ") + std::to_string(frame_count)).c_str());

            seconds_left_until_fps_report = 1.0f;
            frame_count = 0;
        }

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

        rotation_degrees_y += ROTATION_DEGREES_Y_PER_SECOND * delta_time;
        rotation_degrees_x += ROTATION_DEGREES_X_PER_SECOND * delta_time;

        int render_width, render_height;
        SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(render_width) / render_height,
            0.1f,
            100.0f
        );

        glm::mat4 view = glm::lookAt(camera_position, camera_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 rotation_x = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degrees_x), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotation_y = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degrees_y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 model = rotation_x * rotation_y;

        glm::mat4 background_object_model_translation = glm::translate(glm::mat4(1.0), glm::vec3(1.5f, 0.0f, 0.0f));
        glm::mat4 background_object_model = background_object_model_translation * model;

        SDL_Surface* render_texture_surface = nullptr;
        if (is_rasterizing_textures) {
            render_texture_surface = texture_surface;
        }

        main_object.rasterize(triangle_rasterizer, renderer, render_texture_surface, projection, view, model);
        background_object.rasterize(triangle_rasterizer, renderer, render_texture_surface, projection, view, background_object_model);

        SDL_RenderPresent(renderer);
    }

    // --- cleanup and quit ---

    cleanup();
    return 0;
}
