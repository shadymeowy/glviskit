#pragma once

#include "gl/instance.hpp"
#include "primitive/circle.hpp"
#include "primitive/line.hpp"
#include "primitive/mesh.hpp"
#include "primitive/point.hpp"

namespace glviskit {

// holds the private rendering state
class RenderState {
   public:
    RenderState()
        : line_buffer_{vbo_inst},
          point_buffer_{vbo_inst},
          circle_buffer_{vbo_inst},
          mesh_buffer_{vbo_inst} {}

   private:
    // instance transform buffer
    InstanceBuffer vbo_inst;

    // buffers to render
    line::Buffer line_buffer_;
    point::Buffer point_buffer_;
    circle::Buffer circle_buffer_;
    mesh::Buffer mesh_buffer_;

    // allow skipping rendering
    bool enabled_ = true;

    void Save() {
        line_buffer_.Save();
        point_buffer_.Save();
        circle_buffer_.Save();
        mesh_buffer_.Save();
    }

    void Restore() {
        line_buffer_.Restore();
        point_buffer_.Restore();
        circle_buffer_.Restore();
        mesh_buffer_.Restore();
    }

    void Clear() {
        line_buffer_.Clear();
        point_buffer_.Clear();
        circle_buffer_.Clear();
        mesh_buffer_.Clear();
    }

    void SaveInstances() { vbo_inst.Save(); }
    void RestoreInstances() { vbo_inst.Restore(); }
    void ClearInstances() { vbo_inst.Clear(); }

    friend class WindowRenderer;
    friend class RenderList;
    friend class Path;
    friend class Mesh;
};

}  // namespace glviskit