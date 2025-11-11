#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>
#include "buffer_stack.hpp"

struct Instance {
    glm::mat4 transform;
};

using InstanceBuffer = BufferStack<Instance, GL_ARRAY_BUFFER>;