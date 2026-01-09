#pragma once

#include "gl/instance.hpp"
#include "primitive/circle.hpp"
#include "primitive/line.hpp"
#include "primitive/point.hpp"

namespace glviskit {

// holds the private rendering state
class RenderState {
   public:
    RenderState()
        : line_buffer{vbo_inst},
          point_buffer{vbo_inst},
          circle_buffer{vbo_inst} {}

   private:
    // instance transform buffer
    InstanceBuffer vbo_inst;

    // buffers to render
    line::Buffer line_buffer;
    point::Buffer point_buffer;
    circle::Buffer circle_buffer;

    void Save() {
        line_buffer.Save();
        point_buffer.Save();
        circle_buffer.Save();
    }

    void Restore() {
        line_buffer.Restore();
        point_buffer.Restore();
        circle_buffer.Restore();
    }

    void Clear() {
        line_buffer.Clear();
        point_buffer.Clear();
        circle_buffer.Clear();
    }

    void SaveInstances() { vbo_inst.Save(); }
    void RestoreInstances() { vbo_inst.Restore(); }
    void ClearInstances() { vbo_inst.Clear(); }

    friend class WindowRenderer;
    friend class RenderList;
    friend class Path;
};

}  // namespace glviskit