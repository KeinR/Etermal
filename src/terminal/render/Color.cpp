#include "Color.h"

#include <algorithm>
#include <cmath>

#include "../shader/Shader.h"
#include "../render/opengl.h"

static etm::Color::value_t valify(float p) {
    return std::round(p * 255);
}

etm::Color::Color(): Color(0.0f, 0.0f, 0.0f) {
}
etm::Color::Color(prop_t r, prop_t b, prop_t g):
    color{r, b, g} {
}
etm::Color::Color(hex_t hex) {
    setHex(hex);
}

etm::Color &etm::Color::operator=(hex_t hex) {
    setHex(hex);
    return *this;
}

bool etm::Color::operator==(const Color &other) const {
    constexpr float maxDev = 1.0f / 255.0f / 2.0f;
    for (int i = 0; i < 4; i++) {
        // Check that the difference is greater than a deviation,
        // so as to account for floating point inaccuracies
        if (std::abs(other.color[i] - color[i]) > maxDev) {
            return false;
        }
    }
    return true;
}

bool etm::Color::operator!=(const Color &other) const {
    return !(*this == other);
}

void etm::Color::setHex(hex_t hex) {
    // We ignore the first byte
    setVal(
        (hex >> 16) & 0xFF,
        (hex >> 8) & 0xFF,
        hex & 0xFF
    );
}

void etm::Color::setVal(value_t red, value_t green, value_t blue) {
    color[0] = red / 255.0f;
    color[1] = green / 255.0f;
    color[2] = blue / 255.0f;
}

etm::Color::hex_t etm::Color::getHex() const {
    return (valify(color[0]) << 16) | (valify(color[1]) << 8) | valify(color[2]);
}

etm::Color::prop_t *etm::Color::get() {
    // Important to be compatable with other thingies, ex OpenGL
    return color;
}

void etm::Color::setRGB(int location) const {
    // Notice how the digit for the function
    glUniform3fv(location, 1, color);
}

void etm::Color::set(const shader::Shader &shader) const {
    setRGB(shader.getColor());
}
void etm::Color::setBackground(const shader::Shader &shader) const {
    setRGB(shader.getBackGColor());
}
void etm::Color::setForeground(const shader::Shader &shader) const {
    setRGB(shader.getForeGColor());
}

etm::Color etm::Color::brighten(float percent) const {
    Color c;
    for (int i = 0; i < 3; i++) {
        c.color[i] = color[i] + color[i] * percent;
        // Truncate the value if the value is too high/low
        if (c.color[i] > 1) {
            c.color[i] = 1;
        } else if (c.color[i] < 0) {
            c.color[i] = 0;
        }
    }
    return c;
}
