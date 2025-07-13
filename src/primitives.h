#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <glm/glm.hpp>

#include "Object.h"

namespace primitives {
    Object quad(float length, float height, glm::vec3 color, float max_texture_coord);
    Object cuboid(float length, float height, float depth, glm::vec3 color, float max_texture_coord);
};

#endif
