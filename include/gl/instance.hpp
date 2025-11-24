#pragma once

#include "gl/glad.hpp"

#include <glm/glm.hpp>

#include "gl/buffer_stack.hpp"

namespace glviskit {

struct Instance {
    glm::mat4 transform;
};

using InstanceBuffer = BufferStack<Instance, GL_ARRAY_BUFFER>;

}  // namespace glviskit