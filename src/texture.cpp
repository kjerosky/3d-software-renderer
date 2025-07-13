#include "texture.h"

// --------------------------------------------------------------------------

glm::vec3 texture::sample_locked_surface(SDL_Surface* surface,
                                         const SDL_PixelFormatDetails* surface_pixel_format_details,
                                         const glm::vec2& texture_coordinate,
                                         const texture::TextureFilter& texture_filter,
                                         const texture::TextureWrap& texture_wrap) {

    glm::vec2 wrapped_tex_coord = apply_wrap_to_texture_coord(texture_coordinate, texture_wrap);

    // Remember that the y-coordinate is upside down because of how images are loaded!
    glm::vec2 image_tex_coord = glm::vec2(wrapped_tex_coord.x, 1.0f - wrapped_tex_coord.y);

    return apply_filter_to_texture_coord(surface, surface_pixel_format_details, texture_filter, texture_wrap, image_tex_coord);
}

// --------------------------------------------------------------------------

glm::vec2 texture::apply_wrap_to_texture_coord(const glm::vec2& texture_coordinate, const texture::TextureWrap texture_wrap) {
    glm::vec2 final_tex_coord;

    if (texture_wrap == TextureWrap::CLAMP) {
        final_tex_coord = glm::clamp(texture_coordinate, 0.0f, 1.0f);
    } else if (texture_wrap == TextureWrap::REPEAT) {
        int x_int = static_cast<int>(texture_coordinate.x);
        float x_frac = texture_coordinate.x - x_int;
        if (x_frac < 0.0f) {
            x_frac += 1.0f;
        }

        int y_int = static_cast<int>(texture_coordinate.y);
        float y_frac = texture_coordinate.y - y_int;
        if (y_frac < 0.0f) {
            y_frac += 1.0f;
        }

        final_tex_coord = glm::vec2(x_frac, y_frac);
    }

    return final_tex_coord;
}

// --------------------------------------------------------------------------

glm::vec3 texture::apply_filter_to_texture_coord(SDL_Surface* surface,
                                                    const SDL_PixelFormatDetails* surface_pixel_format_details,
                                                    const texture::TextureFilter& filter,
                                                    const texture::TextureWrap& wrap,
                                                    const glm::vec2& tex_coord) {

    Uint8 r, g, b;

    // We're assuming that the surface has four bytes per pixel, so the client
    // needs to ensure that the surface meets this invariant.
    Uint32* pixels = reinterpret_cast<Uint32*>(surface->pixels);

    int p00_x = static_cast<int>(tex_coord.x * surface->w);
    int p00_y = static_cast<int>(tex_coord.y * surface->h);

    float p00_x_frac = (tex_coord.x * surface->w) - p00_x;
    float p00_y_frac = (tex_coord.y * surface->h) - p00_y;

    SDL_GetRGB(pixels[p00_y * surface->w + p00_x], surface_pixel_format_details, nullptr, &r, &g, &b);
    glm::vec3 p00_color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

    glm::vec3 color;
    if (filter == TextureFilter::NEAREST) {
        color = p00_color;
    } else if (filter == TextureFilter::BILINEAR) {
        int next_x = p00_x_frac < 0.5 ? p00_x - 1 : p00_x + 1;
        int next_y = p00_y_frac < 0.5 ? p00_y - 1 : p00_y + 1;

        if (wrap == TextureWrap::CLAMP) {
            next_x = glm::clamp(next_x, 0, surface->w - 1);
            next_y = glm::clamp(next_y, 0, surface->h - 1);
        } else if (wrap == TextureWrap::REPEAT) {
            if (next_x < 0) {
                next_x = surface->w - 1;
            } else if (next_x >= surface->w) {
                next_x = 0;
            }

            if (next_y < 0) {
                next_y = surface->h - 1;
            } else if (next_y >= surface->h) {
                next_y = 0;
            }
        }

        // This is a diagram depicting the naming convention of the points.
        // Flip it horizontally and/or vertically as necessary.
        //  ----- -----
        // | p00 | p10 |
        //  ----- -----
        // | p10 | p11 |
        //  ----- -----

        SDL_GetRGB(pixels[p00_y * surface->w + next_x], surface_pixel_format_details, nullptr, &r, &g, &b);
        glm::vec3 p10_color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

        SDL_GetRGB(pixels[next_y * surface->w + p00_x], surface_pixel_format_details, nullptr, &r, &g, &b);
        glm::vec3 p01_color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

        SDL_GetRGB(pixels[next_y * surface->w + next_x], surface_pixel_format_details, nullptr, &r, &g, &b);
        glm::vec3 p11_color = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

        float t_x = p00_x_frac < 0.5 ? 0.5f - p00_x_frac : p00_x_frac - 0.5f;
        float t_y = p00_y_frac < 0.5 ? 0.5f - p00_y_frac : p00_y_frac - 0.5f;

        glm::vec3 horizontal_color1 = glm::mix(p00_color, p10_color, t_x);
        glm::vec3 horizontal_color2 = glm::mix(p10_color, p11_color, t_x);
        color = glm::mix(horizontal_color1, horizontal_color2, t_y);
    }

    return glm::clamp(color, 0.0f, 1.0f);
}