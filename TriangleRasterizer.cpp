#include "TriangleRasterizer.h"

// --------------------------------------------------------------------------

TriangleRasterizer::TriangleRasterizer() {
    // nothing to do for now
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

    int bounding_box_min_x = std::min({ triangle.v1.coordinates.x, triangle.v2.coordinates.x, triangle.v3.coordinates.x });
    int bounding_box_max_x = std::max({ triangle.v1.coordinates.x, triangle.v2.coordinates.x, triangle.v3.coordinates.x });
    int bounding_box_min_y = std::min({ triangle.v1.coordinates.y, triangle.v2.coordinates.y, triangle.v3.coordinates.y });
    int bounding_box_max_y = std::max({ triangle.v1.coordinates.y, triangle.v2.coordinates.y, triangle.v3.coordinates.y });

    // We'll be more efficient here by limiting the bounding box to the viewable area.
    int render_width, render_height;
    SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);
    bounding_box_min_x = std::clamp(bounding_box_min_x, 0, render_width - 1);
    bounding_box_max_x = std::clamp(bounding_box_max_x, 0, render_width - 1);
    bounding_box_min_y = std::clamp(bounding_box_min_y, 0, render_height - 1);
    bounding_box_max_y = std::clamp(bounding_box_max_y, 0, render_height - 1);

    float area = edge(triangle.v1.coordinates, triangle.v2.coordinates, triangle.v3.coordinates);
    if (area == 0) {
        return;
    }

    if (texture != nullptr) {
        SDL_LockSurface(texture);
    }

    for (int y = bounding_box_min_y; y <= bounding_box_max_y; y++) {
        for (int x = bounding_box_min_x; x <= bounding_box_max_x; x++) {
            SDL_FPoint p = { x + 0.5f, y + 0.5f };

            float w1 = edge(triangle.v2.coordinates, triangle.v3.coordinates, p);
            float w2 = edge(triangle.v3.coordinates, triangle.v1.coordinates, p);
            float w3 = edge(triangle.v1.coordinates, triangle.v2.coordinates, p);

            if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
                // Normalize to barycentric coordinates.
                w1 /= area;
                w2 /= area;
                w3 /= area;

                Color color;
                if (texture == nullptr) {
                    color = triangle.v1.color * w1 + triangle.v2.color * w2 + triangle.v3.color * w3;
                    color.clamp();
                } else {
                    SDL_FPoint interpolated_texture_coordinates = {
                        triangle.v1.uv_coordinates.x * w1 + triangle.v2.uv_coordinates.x * w2 + triangle.v3.uv_coordinates.x * w3,
                        triangle.v1.uv_coordinates.y * w1 + triangle.v2.uv_coordinates.y * w2 + triangle.v3.uv_coordinates.y * w3,
                    };

                    color = sample_locked_surface(texture, pixel_format_details, interpolated_texture_coordinates);
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

Color TriangleRasterizer::sample_locked_surface(SDL_Surface* surface, const SDL_PixelFormatDetails* surface_pixel_format_details, const SDL_FPoint& texture_coordinates) {
    // For now, we'll clamp texture coordinates.
    SDL_FPoint uv = {
        std::clamp(texture_coordinates.x, 0.0f, 1.0f),
        std::clamp(1.0f - texture_coordinates.y, 0.0f, 1.0f),
    };

    // We're assuming that the surface has four bytes per pixel, so the client
    // needs to ensure that the surface meets this invariant.
    int pixel_x = static_cast<int>(uv.x * surface->w);
    int pixel_y = static_cast<int>(uv.y * surface->h);
    int pixel_index = pixel_y * surface->w + pixel_x;
    Uint32 pixel_value = reinterpret_cast<Uint32*>(surface->pixels)[pixel_index];

    Uint8 r, g, b;
    SDL_GetRGB(pixel_value, surface_pixel_format_details, nullptr, &r, &g, &b);

    Color color = { r / 255.0f, g / 255.0f, b / 255.0f };
    color.clamp();

    return color;
}

// --------------------------------------------------------------------------

float TriangleRasterizer::edge(const SDL_FPoint& a, const SDL_FPoint& b, const SDL_FPoint& p) {
    // This is the 2d cross product, which gives the signed area of the
    // parallelogram formed by the vectors ab and ap.
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}
