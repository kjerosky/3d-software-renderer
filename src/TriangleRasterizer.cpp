#include "TriangleRasterizer.h"

#include <algorithm>

// --------------------------------------------------------------------------

TriangleRasterizer::TriangleRasterizer(int depth_buffer_width, int depth_buffer_height)
:
depth_buffer_width(0),
depth_buffer_height(0) {

    resize_depth_buffer(depth_buffer_width, depth_buffer_height);
}

// --------------------------------------------------------------------------

TriangleRasterizer::~TriangleRasterizer() {
    // nothing to do for now
}

// --------------------------------------------------------------------------

void TriangleRasterizer::rasterize(SDL_Renderer* renderer, const Triangle& triangle, SDL_Surface* texture) {
    const SDL_PixelFormatDetails* pixel_format_details;
    if (texture != nullptr) {
        pixel_format_details = SDL_GetPixelFormatDetails(texture->format);
    }

    int bounding_box_min_x = std::min({ triangle.v0.screen_coord.x, triangle.v1.screen_coord.x, triangle.v2.screen_coord.x });
    int bounding_box_max_x = std::max({ triangle.v0.screen_coord.x, triangle.v1.screen_coord.x, triangle.v2.screen_coord.x });
    int bounding_box_min_y = std::min({ triangle.v0.screen_coord.y, triangle.v1.screen_coord.y, triangle.v2.screen_coord.y });
    int bounding_box_max_y = std::max({ triangle.v0.screen_coord.y, triangle.v1.screen_coord.y, triangle.v2.screen_coord.y });

    // We'll be more efficient here by limiting the bounding box to the viewable area.
    int render_width, render_height;
    SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);
    bounding_box_min_x = std::clamp(bounding_box_min_x, 0, render_width - 1);
    bounding_box_max_x = std::clamp(bounding_box_max_x, 0, render_width - 1);
    bounding_box_min_y = std::clamp(bounding_box_min_y, 0, render_height - 1);
    bounding_box_max_y = std::clamp(bounding_box_max_y, 0, render_height - 1);

    float area = edge(triangle.v0.screen_coord, triangle.v1.screen_coord, triangle.v2.screen_coord);
    if (area == 0) {
        return;
    }

    if (texture != nullptr) {
        SDL_LockSurface(texture);
    }

    for (int y = bounding_box_min_y; y <= bounding_box_max_y; y++) {
        for (int x = bounding_box_min_x; x <= bounding_box_max_x; x++) {
            glm::vec2 p = glm::vec2(x + 0.5f, y + 0.5f);

            float w0 = edge(triangle.v1.screen_coord, triangle.v2.screen_coord, p);
            float w1 = edge(triangle.v2.screen_coord, triangle.v0.screen_coord, p);
            float w2 = edge(triangle.v0.screen_coord, triangle.v1.screen_coord, p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                // Normalize to barycentric coordinates.
                w0 /= area;
                w1 /= area;
                w2 /= area;

                float depth = triangle.v0.ndc_z * w0 + triangle.v1.ndc_z * w1 + triangle.v2.ndc_z * w2;
                int depth_buffer_index = y * depth_buffer_width + x;
                if (depth > depth_buffer[depth_buffer_index]) {
                    continue;
                } else {
                    depth_buffer[depth_buffer_index] = depth;
                }

                glm::vec3 color;
                if (texture == nullptr) {
                    color = triangle.v0.color * w0 + triangle.v1.color * w1 + triangle.v2.color * w2;
                    color = glm::clamp(color, 0.0f, 1.0f);
                } else {
                    float inverse_z0 = 1.0f / triangle.v0.view_z;
                    float inverse_z1 = 1.0f / triangle.v1.view_z;
                    float inverse_z2 = 1.0f / triangle.v2.view_z;
                    float interpolated_inverse_z = inverse_z0 * w0 + inverse_z1 * w1 + inverse_z2 * w2;

                    glm::vec2 uv_projected0 = triangle.v0.tex_coord * inverse_z0;
                    glm::vec2 uv_projected1 = triangle.v1.tex_coord * inverse_z1;
                    glm::vec2 uv_projected2 = triangle.v2.tex_coord * inverse_z2;

                    glm::vec2 interpolated_perspective_corrected_uv =
                        (uv_projected0 * w0 + uv_projected1 * w1 + uv_projected2 * w2) / interpolated_inverse_z;

                    color = sample_locked_surface(texture, pixel_format_details, interpolated_perspective_corrected_uv);
                }

                SDL_SetRenderDrawColor(renderer, color.r * 255, color.g * 255, color.b * 255, 255);
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }

    if (texture != nullptr) {
        SDL_UnlockSurface(texture);
    }
}

// --------------------------------------------------------------------------

glm::vec3 TriangleRasterizer::sample_locked_surface(SDL_Surface* surface, const SDL_PixelFormatDetails* surface_pixel_format_details, const glm::vec2& texture_coordinates) {
    // Remember that the y-coordinate is upside down because of how images are loaded!
    // Also, for now we'll just clamp texture coordinates.
    float u = glm::clamp(texture_coordinates.x, 0.0f, 1.0f);
    float v = glm::clamp(1.0f - texture_coordinates.y, 0.0f, 1.0f);

    // We're assuming that the surface has four bytes per pixel, so the client
    // needs to ensure that the surface meets this invariant.
    int pixel_x = static_cast<int>(u * surface->w);
    int pixel_y = static_cast<int>(v * surface->h);
    int pixel_index = pixel_y * surface->w + pixel_x;
    Uint32 pixel_value = reinterpret_cast<Uint32*>(surface->pixels)[pixel_index];

    Uint8 r, g, b;
    SDL_GetRGB(pixel_value, surface_pixel_format_details, nullptr, &r, &g, &b);

    glm::vec3 color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
    color = glm::clamp(color, 0.0f, 1.0f);

    return color;
}

// --------------------------------------------------------------------------

float TriangleRasterizer::edge(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p) {
    // This is the 2d cross product, which gives the signed area of the
    // parallelogram formed by the vectors ab and ap.
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}

// --------------------------------------------------------------------------

void TriangleRasterizer::clear_depth_buffer() {
    std::fill(depth_buffer.begin(), depth_buffer.end(), 1.0f);
}

// --------------------------------------------------------------------------

void TriangleRasterizer::resize_depth_buffer(int new_width, int new_height) {
    if (depth_buffer_width == new_width && depth_buffer_height == new_height) {
        return;
    }

    depth_buffer_width = new_width;
    depth_buffer_height = new_height;
    depth_buffer.resize(depth_buffer_width * depth_buffer_height);
}
