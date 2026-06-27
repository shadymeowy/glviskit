#pragma once

#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#include "../gl/gl.hpp"

namespace glviskit {

#if defined(GLVISKIT_GL33)
#define GLVISKIT_VERT_HEADER "#version 330 core\n"
#define GLVISKIT_FRAG_HEADER "#version 330 core\n"
#elif defined(GLVISKIT_GLES3)
#define GLVISKIT_VERT_HEADER "#version 300 es\nprecision highp float;\n"
#define GLVISKIT_FRAG_HEADER "#version 300 es\nprecision mediump float;\n"
#else
#error "No GL version defined"
#endif

template <const char *shader_vertex, const char *shader_fragment>
class Program {
   public:
    Program() {
        GLuint s_vertex = glCreateShader(GL_VERTEX_SHADER);
        const char *src_vertex = shader_vertex;
        glShaderSource(s_vertex, 1, &src_vertex, nullptr);
        glCompileShader(s_vertex);

        // check compile errors
        GLint success;
        glGetShaderiv(s_vertex, GL_COMPILE_STATUS, &success);
        if (success == 0) {
            std::array<GLchar, 512> info_log{};
            glGetShaderInfoLog(s_vertex, 512, nullptr, info_log.data());
            throw std::runtime_error(
                std::string{"Error compiling vertex shader: "} +
                info_log.data());
        }

        GLuint s_frag = glCreateShader(GL_FRAGMENT_SHADER);
        const char *src_frag = shader_fragment;
        glShaderSource(s_frag, 1, &src_frag, nullptr);
        glCompileShader(s_frag);

        // check compile errors
        glGetShaderiv(s_frag, GL_COMPILE_STATUS, &success);
        if (success == 0) {
            std::array<GLchar, 512> info_log{};
            glGetShaderInfoLog(s_frag, 512, nullptr, info_log.data());
            throw std::runtime_error(
                std::string{"Error compiling fragment shader: "} +
                info_log.data());
        }

        program = glCreateProgram();
        if (program == 0) {
            throw std::runtime_error("Error creating shader program");
        }

        glAttachShader(program, s_vertex);
        glAttachShader(program, s_frag);
        glLinkProgram(program);
        glDeleteShader(s_vertex);
        glDeleteShader(s_frag);

        loc_mvp = glGetUniformLocation(program, "mvp");
        if (loc_mvp == -1) {
        }
        loc_screen_size = glGetUniformLocation(program, "screen_size");
        if (loc_screen_size == -1) {
        }
        loc_alpha_test = glGetUniformLocation(program, "alpha_test");
        if (loc_alpha_test == -1) {
        }
        // symbol-only uniforms; -1 (and no-op) for shaders without them
        loc_atlas = glGetUniformLocation(program, "u_atlas");
        loc_px_range = glGetUniformLocation(program, "u_px_range");
    }

    // destructor
    ~Program() {
        if (program != 0) {
            glDeleteProgram(program);
            program = 0;
            loc_mvp = 0;
            loc_screen_size = 0;
            loc_alpha_test = 0;
            loc_atlas = 0;
            loc_px_range = 0;
        }
    }

    // this class is non-copyable
    Program(const Program &) = delete;
    auto operator=(const Program &) -> Program & = delete;

    // but movable
    Program(Program &&other) noexcept
        : program(other.program),
          loc_mvp(other.loc_mvp),
          loc_screen_size(other.loc_screen_size),
          loc_alpha_test(other.loc_alpha_test),
          loc_atlas(other.loc_atlas),
          loc_px_range(other.loc_px_range) {
        other.program = 0;
        other.loc_mvp = 0;
        other.loc_screen_size = 0;
        other.loc_alpha_test = 0;
        other.loc_atlas = 0;
        other.loc_px_range = 0;
    }

    auto operator=(Program &&other) noexcept -> Program & {
        if (this != &other) {
            if (program != 0) {
                glDeleteProgram(program);
            }
            program = other.program;
            loc_mvp = other.loc_mvp;
            loc_screen_size = other.loc_screen_size;
            loc_alpha_test = other.loc_alpha_test;
            loc_atlas = other.loc_atlas;
            loc_px_range = other.loc_px_range;
            other.program = 0;
            other.loc_mvp = 0;
            other.loc_screen_size = 0;
            other.loc_alpha_test = 0;
            other.loc_atlas = 0;
            other.loc_px_range = 0;
        }
        return *this;
    }

    void Use() { glUseProgram(program); }

    void SetMVP(const glm::mat4 &mvp) {
        if (loc_mvp == -1) {
            return;
        }
        glUniformMatrix4fv(loc_mvp, 1, GL_FALSE, &mvp[0][0]);
    }

    void SetScreenSize(const glm::vec2 &screen_size) {
        if (loc_screen_size == -1) {
            return;
        }
        glUniform2fv(loc_screen_size, 1, &screen_size[0]);
    }

    void SetAlphaTest(int alpha_test) {
        if (loc_alpha_test == -1) {
            return;
        }
        glUniform1i(loc_alpha_test, alpha_test);
    }

    // symbol-only; no-op on shaders that lack these uniforms
    void SetAtlas(int unit) {
        if (loc_atlas == -1) {
            return;
        }
        glUniform1i(loc_atlas, unit);
    }

    void SetPxRange(float px_range) {
        if (loc_px_range == -1) {
            return;
        }
        glUniform1f(loc_px_range, px_range);
    }

   private:
    GLuint program{};
    GLuint loc_mvp{}, loc_screen_size{}, loc_alpha_test{};
    GLint loc_atlas{}, loc_px_range{};
};

}  // namespace glviskit