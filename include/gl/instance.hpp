#pragma once

#include <glm/glm.hpp>

#include "buffer_stack.hpp"
#include "../gl/gl.hpp"

namespace glviskit {

struct Instance {
    glm::mat4 transform;
};

using InstanceBuffer = BufferStack<Instance, GL_ARRAY_BUFFER>;

}  // namespace glviskit