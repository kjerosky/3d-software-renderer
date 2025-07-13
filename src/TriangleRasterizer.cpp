#include "TriangleRasterizer.h"

#include <algorithm>

#include "texture.h"

// --------------------------------------------------------------------------

TriangleRasterizer::TriangleRasterizer(int depth_buffer_width, int depth_buffer_height)
:
depth_buffer_width(0),
depth_buffer_height(0),
texture_filter(texture::TextureFilter::NEAREST),
texture_wrap(texture::TextureWrap::CLAMP) {

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

                float inverse_z0 = 1.0f / triangle.v0.view_z;
                float inverse_z1 = 1.0f / triangle.v1.view_z;
                float inverse_z2 = 1.0f / triangle.v2.view_z;
                float interpolated_inverse_z = inverse_z0 * w0 + inverse_z1 * w1 + inverse_z2 * w2;

                glm::vec3 color;
                if (texture == nullptr) {
                    glm::vec3 color_projected0 = triangle.v0.color * inverse_z0;
                    glm::vec3 color_projected1 = triangle.v1.color * inverse_z1;
                    glm::vec3 color_projected2 = triangle.v2.color * inverse_z2;

                    color = (color_projected0 * w0 + color_projected1 * w1 + color_projected2 * w2) / interpolated_inverse_z;
                    color = glm::clamp(color, 0.0f, 1.0f);
                } else {
                    glm::vec2 uv_projected0 = triangle.v0.tex_coord * inverse_z0;
                    glm::vec2 uv_projected1 = triangle.v1.tex_coord * inverse_z1;
                    glm::vec2 uv_projected2 = triangle.v2.tex_coord * inverse_z2;

                    glm::vec2 interpolated_perspective_corrected_uv =
                        (uv_projected0 * w0 + uv_projected1 * w1 + uv_projected2 * w2) / interpolated_inverse_z;

                    color = texture::sample_locked_surface(texture, pixel_format_details, interpolated_perspective_corrected_uv, texture_filter, texture_wrap);
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

// --------------------------------------------------------------------------

void TriangleRasterizer::set_texture_filter(const texture::TextureFilter texture_filter) {
    this->texture_filter = texture_filter;
}

// --------------------------------------------------------------------------

void TriangleRasterizer::set_texture_wrap(const texture::TextureWrap texture_wrap) {
    this->texture_wrap = texture_wrap;
}
