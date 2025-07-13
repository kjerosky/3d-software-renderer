#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

namespace texture {
    enum class TextureFilter {
        NEAREST,
        BILINEAR,
    };

    enum class TextureWrap {
        CLAMP,
        REPEAT,
    };

    glm::vec3 sample_locked_surface(SDL_Surface* surface,
                                    const SDL_PixelFormatDetails* surface_pixel_format_details,
                                    const glm::vec2& texture_coordinate,
                                    const texture::TextureFilter& texture_filter,
                                    const texture::TextureWrap& texture_wrap);

    glm::vec2 apply_wrap_to_texture_coord(const glm::vec2& texture_coordinate, const texture::TextureWrap texture_wrap);

    glm::vec3 apply_filter_to_texture_coord(SDL_Surface* surface,
                                               const SDL_PixelFormatDetails* surface_pixel_format_details,
                                               const texture::TextureFilter& filter,
                                               const texture::TextureWrap& wrap,
                                               const glm::vec2& tex_coord);
};

#endif
