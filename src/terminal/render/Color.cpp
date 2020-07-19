#include "Color.h"

#include <cmath>

#include "../shader/Shader.h"
#include "../render/opengl.h"

static etm::Color::value_t valify(float p) {
    return std::round(p * 255);
}

etm::Color::Color(): Color(0.0f, 0.0f, 0.0f, 1.0f) {
}
etm::Color::Color(prop_t r, prop_t b, prop_t g, prop_t a):
    color{r, b, g, a} {
}
etm::Color::Color(hex_t hex, value_t alpha) {
    setHex(hex, alpha);
}

etm::Color &etm::Color::operator=(hex_t hex) {
    setHex(hex);
    return *this;
}

bool etm::Color::operator==(const Color &other) const {
    constexpr float maxDev = 1.0f / 255.0f / 2.0f;
    for (int i = 0; i < 4; i++) {
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
void etm::Color::setHex(hex_t hex, value_t alpha) {
    // We ignore the first byte
    setVal(
        (hex >> 16) & 0xFF,
        (hex >> 8) & 0xFF,
        hex & 0xFF,
        alpha
    );
}

void etm::Color::setVal(value_t red, value_t green, value_t blue) {
    color[0] = red / 255.0f;
    color[1] = green / 255.0f;
    color[2] = blue / 255.0f;
}
void etm::Color::setVal(value_t red, value_t green, value_t blue, value_t alpha) {
    color[0] = red / 255.0f;
    color[1] = green / 255.0f;
    color[2] = blue / 255.0f;
    color[3] = alpha / 255.0f;
}
void etm::Color::setAlpha(value_t alpha) {
    color[3] = alpha / 255.0f;
}

etm::Color::hex_t etm::Color::getHex() const {
    return (valify(color[0]) << 16) | (valify(color[1]) << 8) | valify(color[2]);
}

etm::Color::prop_t *etm::Color::get() {
    // Important to be compatable with other thingies, ie FONT
    return color;
}

void etm::Color::setRGB(int location) const {
    // Notice how the digit for the function is different
    glUniform3fv(location, 1, color);
}
void etm::Color::setRGBA(int location) const {
    glUniform4fv(location, 1, color);
}

void etm::Color::set(const shader::Shader &shader) const {
    setRGBA(shader.getColor());
}
void etm::Color::setBackground(const shader::Shader &shader) const {
    setRGB(shader.getBackGColor());
}
void etm::Color::setForeground(const shader::Shader &shader) const {
    setRGB(shader.getForeGColor());
}

etm::Color etm::Color::brighten(float percent) const {
    Color c;
    for (int i = 0; i < 4; i++) {
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
