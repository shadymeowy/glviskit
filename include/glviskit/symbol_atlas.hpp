#pragma once

#include <cstddef>

namespace glviskit {

// a prebuilt msdf atlas laid out as a uniform grid of equal cells
struct SymbolAtlasData {
    int width{0};
    int height{0};
    int channels{3};
    float px_range{0.0F};
    // grid cells in x, y
    int cols{0};
    int rows{0};
    // codepoint of cell index 0
    int first_codepoint{0};
    // number of contiguous ascii cells
    int char_count{0};
    // monospace advance in cell units
    float advance{1.0F};
    // line height in cell units (text spacing)
    float line_height{1.0F};
    const unsigned char *pixels{nullptr};
    std::size_t pixel_count{0};

    [[nodiscard]] auto Empty() const -> bool {
        return width == 0 || pixels == nullptr;
    }
};

// the default atlas baked into the binary
// defined in src/symbol_atlas_default.cpp
auto DefaultSymbolAtlas() -> const SymbolAtlasData &;

}  // namespace glviskit
