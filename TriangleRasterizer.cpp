#include "TriangleRasterizer.h"

#include <algorithm>

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
    int bounding_box_min_x = std::min({ triangle.p1.x, triangle.p2.x, triangle.p3.x });
    int bounding_box_max_x = std::max({ triangle.p1.x, triangle.p2.x, triangle.p3.x });
    int bounding_box_min_y = std::min({ triangle.p1.y, triangle.p2.y, triangle.p3.y });
    int bounding_box_max_y = std::max({ triangle.p1.y, triangle.p2.y, triangle.p3.y });

    // We'll be more efficient here by limiting the bounding box to the viewable area.
    int render_width, render_height;
    SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);
    bounding_box_min_x = std::max(bounding_box_min_x, 0);
    bounding_box_max_x = std::min(bounding_box_max_x, render_width - 1);
    bounding_box_min_y = std::max(bounding_box_min_y, 0);
    bounding_box_max_y = std::min(bounding_box_max_y, render_height - 1);

    for (int y = bounding_box_min_y; y <= bounding_box_max_y; y++) {
        for (int x = bounding_box_min_x; x <= bounding_box_max_x; x++) {
            SDL_FPoint p = { x + 0.5f, y + 0.5f };

            float w1 = edge(triangle.p1, triangle.p2, p);
            float w2 = edge(triangle.p2, triangle.p3, p);
            float w3 = edge(triangle.p3, triangle.p1, p);

            if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderPoint(renderer, x, y);
            }
        }
    }
}

// --------------------------------------------------------------------------

float TriangleRasterizer::edge(const SDL_FPoint& a, const SDL_FPoint& b, const SDL_FPoint& p) {
    return (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);
}
