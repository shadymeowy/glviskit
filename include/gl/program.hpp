#pragma once

#include <array>
#include <glm/glm.hpp>
#include <iostream>

#include "glad.hpp"

namespace glviskit {

template <const char *shader_vertex, const char *shader_fragment>
class Program {
   public:
    explicit Program(GladGLContext &gl) : gl{gl} {
        GLuint s_vertex = gl.CreateShader(GL_VERTEX_SHADER);
        const char *src_vertex = shader_vertex;
        gl.ShaderSource(s_vertex, 1, &src_vertex, nullptr);
        gl.CompileShader(s_vertex);

        // check compile errors
        GLint success;
        gl.GetShaderiv(s_vertex, GL_COMPILE_STATUS, &success);
        if (success == 0) {
            std::array<GLchar, 512> info_log{};
            gl.GetShaderInfoLog(s_vertex, 512, nullptr, info_log.data());
            std::cerr << "Error compiling vertex shader: " << info_log.data()
                      << '\n';
            exit(EXIT_FAILURE);
        }

        GLuint s_frag = gl.CreateShader(GL_FRAGMENT_SHADER);
        const char *src_frag = shader_fragment;
        gl.ShaderSource(s_frag, 1, &src_frag, nullptr);
        gl.CompileShader(s_frag);

        // check compile errors
        gl.GetShaderiv(s_frag, GL_COMPILE_STATUS, &success);
        if (success == 0) {
            std::array<GLchar, 512> info_log{};
            gl.GetShaderInfoLog(s_frag, 512, nullptr, info_log.data());
            std::cerr << "Error compiling fragment shader: " << info_log.data()
                      << '\n';
            exit(EXIT_FAILURE);
        }

        program = gl.CreateProgram();
        if (program == 0) {
            std::cerr << "Error creating shader program" << '\n';
            exit(EXIT_FAILURE);
        }

        gl.AttachShader(program, s_vertex);
        gl.AttachShader(program, s_frag);
        gl.LinkProgram(program);
        gl.DeleteShader(s_vertex);
        gl.DeleteShader(s_frag);

        loc_mvp = gl.GetUniformLocation(program, "mvp");
        if (loc_mvp == -1) {
            std::cerr << "Warning: mvp uniform not found in shader program"
                      << '\n';
        }
        loc_screen_size = gl.GetUniformLocation(program, "screen_size");
        if (loc_screen_size == -1) {
            std::cerr
                << "Warning: screen_size uniform not found in shader program"
                << '\n';
        }
    }

    // destructor
    ~Program() {
        if (program != 0) {
            gl.DeleteProgram(program);
            program = 0;
            loc_mvp = 0;
            loc_screen_size = 0;
        }
    }

    // this class is non-copyable
    Program(const Program &) = delete;
    auto operator=(const Program &) -> Program & = delete;

    // but movable
    Program(Program &&other) noexcept
        : gl(other.gl),
          program(other.program),
          loc_mvp(other.loc_mvp),
          loc_screen_size(other.loc_screen_size) {
        other.program = 0;
        other.loc_mvp = 0;
        other.loc_screen_size = 0;
    }

    auto operator=(Program &&other) noexcept -> Program & {
        if (this != &other) {
            if (program != 0) {
                gl.DeleteProgram(program);
            }
            gl = other.gl;
            program = other.program;
            loc_mvp = other.loc_mvp;
            loc_screen_size = other.loc_screen_size;
            other.program = 0;
            other.loc_mvp = 0;
            other.loc_screen_size = 0;
        }
        return *this;
    }

    void Use() { gl.UseProgram(program); }

    void SetMVP(const glm::mat4 &mvp) {
        if (loc_mvp == -1) {
            return;
        }
        gl.UniformMatrix4fv(loc_mvp, 1, GL_FALSE, &mvp[0][0]);
    }

    void SetScreenSize(const glm::vec2 &screen_size) {
        if (loc_screen_size == -1) {
            return;
        }
        gl.Uniform2fv(loc_screen_size, 1, &screen_size[0]);
    }

   private:
    GladGLContext &gl;
    GLuint program{};
    GLuint loc_mvp{}, loc_screen_size{};
};

}  // namespace glviskit