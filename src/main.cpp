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
SDL_Texture* target_texture = nullptr;
SDL_Surface* texture_surface = nullptr;

// --------------------------------------------------------------------------

void fit_inner_rect_within_outer_rect(const SDL_FRect& inner_rect, const SDL_FRect& outer_rect, SDL_FRect& result) {
    float inner_aspect_ratio = inner_rect.w / inner_rect.h;

    float vertical_size_after_horizontal_expansion = outer_rect.w / inner_aspect_ratio;
    bool expanding_inner_rect_horizontally_fits_vertically = vertical_size_after_horizontal_expansion <= outer_rect.h;

    if (expanding_inner_rect_horizontally_fits_vertically) {
        result.w = outer_rect.w;
        result.h = vertical_size_after_horizontal_expansion;
        result.x = 0.0f;
        result.y = (outer_rect.h - result.h) / 2.0f;
    } else {
        float horizontal_size_after_vertical_expansion = outer_rect.h * inner_aspect_ratio;

        result.w = horizontal_size_after_vertical_expansion;
        result.h = outer_rect.h;
        result.x = (outer_rect.w - result.w) / 2.0f;
        result.y = 0.0f;
    }
}

// --------------------------------------------------------------------------

void cleanup() {
    if (texture_surface != nullptr) {
        SDL_DestroySurface(texture_surface);
    }

    if (target_texture != nullptr) {
        SDL_DestroyTexture(target_texture);
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
    const int INITIAL_WINDOW_WIDTH = 640;
    const int INITIAL_WINDOW_HEIGHT = 360;

    const std::string WINDOW_TITLE = "3d Software Renderer";

    // --- sdl3 setup ---

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    window = SDL_CreateWindow(WINDOW_TITLE.c_str(), INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
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

    target_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    if (target_texture == nullptr) {
        std::cerr << "[ERROR] SDL_CreateTexture error: " << SDL_GetError() << std::endl;
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

    TriangleRasterizer triangle_rasterizer(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

    Object main_object = primitives::cuboid(2.0f, 2.0f, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    Object background_object = primitives::cuboid(1.5f, 1.5f, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 5.0f);

    const bool* keyboard_state = SDL_GetKeyboardState(nullptr);

    bool is_rasterizing_textures = true;
    bool previous_texture_rasterization_toggle_key_state = false;

    bool is_upscaling = true;
    bool previous_upscale_toggle_key_state = false;
    SDL_FRect target_texture_rect = { 0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT };

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

        const bool current_texture_rasterization_toggle_key_state = keyboard_state[SDL_SCANCODE_RETURN];
        if (!previous_texture_rasterization_toggle_key_state && current_texture_rasterization_toggle_key_state) {
            is_rasterizing_textures = !is_rasterizing_textures;
        }
        previous_texture_rasterization_toggle_key_state = current_texture_rasterization_toggle_key_state;

        const bool current_upscale_toggle_key_state = keyboard_state[SDL_SCANCODE_SPACE];
        if (!previous_upscale_toggle_key_state && current_upscale_toggle_key_state) {
            is_upscaling = !is_upscaling;
        }
        previous_upscale_toggle_key_state = current_upscale_toggle_key_state;

        if (is_upscaling) {
            // Since we're going to maintain the original aspect ratio, filling the window
            // with black before rendering the target texture will give us "black bars"
            // around any space not covered by the target texture due to aspect ratio
            // misalignment.
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderTarget(renderer, target_texture);

            SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
            SDL_RenderClear(renderer);
        } else {
            SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
            SDL_RenderClear(renderer);
        }

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

        triangle_rasterizer.resize_depth_buffer(render_width, render_height);
        triangle_rasterizer.clear_depth_buffer();

        main_object.rasterize(triangle_rasterizer, renderer, render_texture_surface, projection, view, model);
        background_object.rasterize(triangle_rasterizer, renderer, render_texture_surface, projection, view, background_object_model);

        if (is_upscaling) {
            SDL_SetRenderTarget(renderer, nullptr);

            int window_width, window_height;
            SDL_GetRenderOutputSize(renderer, &window_width, &window_height);

            SDL_FRect texture_fit_to_window_rect;
            SDL_FRect window_rect = { 0.0f, 0.0f, static_cast<float>(window_width), static_cast<float>(window_height) };
            fit_inner_rect_within_outer_rect(target_texture_rect, window_rect, texture_fit_to_window_rect);

            SDL_RenderTexture(renderer, target_texture, nullptr, &texture_fit_to_window_rect);
        }

        SDL_RenderPresent(renderer);
    }

    // --- cleanup and quit ---

    cleanup();
    return 0;
}
