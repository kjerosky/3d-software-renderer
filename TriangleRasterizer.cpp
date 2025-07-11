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

void TriangleRasterizer::rasterize(SDL_Renderer* renderer, const Triangle& triangle) {
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

                Color final_color = triangle.v1.color * w1 + triangle.v2.color * w2 + triangle.v3.color * w3;
                final_color.clamp();

                SDL_SetRenderDrawColor(renderer, final_color.r * 255, final_color.g * 255, final_color.b * 255, 255);
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
}

// --------------------------------------------------------------------------

float TriangleRasterizer::edge(const SDL_FPoint& a, const SDL_FPoint& b, const SDL_FPoint& p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}
