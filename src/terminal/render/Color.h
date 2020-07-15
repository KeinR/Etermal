#ifndef ETERMAL_COLOR_H_INCLUDED
#define ETERMAL_COLOR_H_INCLUDED

// ../shader/Shader
namespace etm::shader { class Shader; }

namespace etm {
    class Color {
    public:
        // Type used for hexidecimal values
        // Note that RGB color values are 24 bit (3 byte),
        // so any superfluous values are ignored.
        typedef unsigned int hex_t;
        // Type used for full RGBA values (0-255) incl.
        typedef unsigned char value_t;
        // Type used for RGBA proportions (0-1) incl.
        typedef float prop_t;
    private:
        // RGBA, Red Green Blue Alpha, in that order
        prop_t color[4];
        // RGB
        void setRGB(int location);
        // RGBA
        void setRGBA(int location);
    public:
        // Default initialization
        // 0, 0, 0, 255 by default; ie, "opaque black"
        Color();
        // Init with hex and optional alpha
        Color(hex_t hex, value_t alpha = 0xFF);

        // Convinience operator, calls setHex(hex)
        Color &operator=(hex_t hex);

        // Returns true if both have the same color values.
        // Because of how the colors are stored as floating point
        // values, checks each value is +/- 1 / 255 / 2
        bool operator==(const Color &other);

        // Hex value for RGB, with first 8 bytes ignored
        void setHex(hex_t hex);
        // Same as above, but including an additional alpha value
        void setHex(hex_t hex, value_t alpha);

        void setVal(value_t red, value_t green, value_t blue);
        void setVal(value_t red, value_t green, value_t blue, value_t alpha);
        void setAlpha(value_t alpha);

        // Get RGB (no alpha) as a hexidecimal value (represented in base 10 obv.)
        hex_t getHex() const;

        // Get pointer to data, array of length 4 in RGBA format
        // Note that this data is tied to the color object, and will destruct when it does.
        prop_t *get();

        // Binds this color to the current color uniform
        void set(const shader::Shader &shader);
        void setBackground(const shader::Shader &shader);
        void setForeground(const shader::Shader &shader);

        // Returns a copy based of this one, with
        // the brightness modified by `percent`.
        // Darker values would be caused by a negative arg,
        // Brighter ones by a positive one
        Color brighten(float percent) const;
    };
}

#endif
