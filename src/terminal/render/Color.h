#ifndef ETERMAL_COLOR_H_INCLUDED
#define ETERMAL_COLOR_H_INCLUDED

// ../shader/Shader
namespace etm::shader { class Shader; }

namespace etm {

    /**
    * A multipurpose color object that stores RGB color
    * proportions in float array [@ref color].
    * @see Model
    * @see RModel
    */
    class Color {
    public:
        /// Type used for hexidecimal values
        /// @note hexes for Color are parsed as RGB values,
        /// so the first byte is ignored.
        typedef unsigned int hex_t;
        /// Type used for RGB @e values (0-255 inclusive).
        typedef unsigned char value_t;
        /// Type used for RGB @e proportions (0-1 incl).
        /// @see color
        typedef float prop_t;
    private:
        /// Internal color data proportions.
        /// Values are all 0-1.
        /// Stored in RGB format, where the first
        /// index stores red, second green, etc.
        prop_t color[3];

        /**
        * Sets the uniform at `location` to `*this`'s @ref color
        * via glUniform3fv
        * @param [in] location The location of the uniform
        * @see setBackground(const shader::Shader &shader)
        * @see setForeground(const shader::Shader &shader)
        * @see set(const shader::Shader &shader)
        */
        void setRGB(int location) const;
    public:

        /**
        * Default initalize color values to 0, 0, 0
        */
        Color();
        /**
        * Initialize with proporions (0-1)
        * @param [in] r The red
        * @param [in] g The green
        * @param [in] b The blue
        */
        Color(prop_t r, prop_t g, prop_t b);
        /**
        * Initialize with hex value
        * @note The hex is RGB, so only the lesser three
        * bytes are read.
        * @param [in] hex The RGB hex value 
        * @see setHex(hex_t hex)
        */
        Color(hex_t hex);

        /**
        * Convinience operator, calls @ref setHex(hex_t hex).
        * @note The hex is RGB, so only the lesser three
        * bytes are read.
        * @param [in] hex The RGB hex value
        * @return A reference to `*this`
        * @see setHex(hex_t hex)
        */
        Color &operator=(hex_t hex);

        /**
        * Compares two Color objects
        * @return `true` if both have the same color value,
        * with a margin if error of +/- 1 / 255 / 2
        */
        bool operator==(const Color &other) const;
        /**
        * Compares two Color objects
        * @return A NOT of @ref operator==(const Color &other)
        */
        bool operator!=(const Color &other) const;

        /**
        * Set a hex value as the color.
        * @note The hex is RGB, so only the lesser three
        * bytes are read.
        * @param [in] hex The RGB hex value 
        */
        void setHex(hex_t hex);

        /**
        * Set RGB color values.
        * @param [in] red The red
        * @param [in] green The green
        * @param [in] blue The blue
        */
        void setVal(value_t red, value_t green, value_t blue);

        /**
        * Get an integer representing the RGB values of this
        * color object.
        * Last byte is blue, preceeding is green, and the one before that is red.
        * @return Encoded RGB value
        * @see setHex(hex_t hex)
        */
        hex_t getHex() const;

        /**
        * Get pointer to data.
        * Especially useful when interfacing with other APIs (OpenGL).
        * Stored sequentually, in RGB format.
        * @note The lifetime of the data pointed to by the pointer
        * is tied to `*this`
        * @return Pointer to array of 3 floats, sorted RGB
        */
        prop_t *get();

        /**
        * Binds RGB data to the color uniform in the current shader
        * @note The given shader only supplies the uniform location - 
        * the data is set for the currently bound shader.
        * @param [in] shader The shader that contains the uniform location info
        * @see setBackground(const shader::Shader &shader)
        * @see setForeground(const shader::Shader &shader)
        */
        void set(const shader::Shader &shader) const;
        /**
        * Binds RGB data to the background color uniform in the current shader
        * @note The given shader only supplies the uniform location - 
        * the data is set for the currently bound shader.
        * @param [in] shader The shader that contains the uniform location info
        * @see set(const shader::Shader &shader)
        * @see setForeground(const shader::Shader &shader)
        */
        void setBackground(const shader::Shader &shader) const;
        /**
        * Binds RGB data to the background color uniform in the current shader
        * @note The given shader only supplies the uniform location - 
        * the data is set for the currently bound shader.
        * @param [in] shader The shader that contains the uniform location info
        * @see setBackground(const shader::Shader &shader)
        * @see set(const shader::Shader &shader)
        */
        void setForeground(const shader::Shader &shader) const;

        /**
        * Returns a copy of `*this` with the brightness modified
        * by `percent`.
        * @note The color values will never overflow, they are min/maxed
        * @param [in] percent The proportion to modify each color channel
        * by, where 0 is 0% and 1 is 100%
        * @return A newly derived color object with the modified colors
        */
        Color brighten(float percent) const;
    };
}

#endif
