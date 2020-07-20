#ifndef ETERMAL_ENV_H_INCLUDED
#define ETERMAL_ENV_H_INCLUDED

#include <string>

/*

DOCS:

Syntax emulates ansi a little.
<ESC>[<command><arg>;

Where <ESC> is the escape char 0x1b,
<command> is a single digit command,
and <arg> is the argument.

Currently, there are two commands:

b - set background color
f - set foreground color

For both of the above, <arg> is a
hexidecimal number corresponding to a
hexidecimal color code with a maximum size of
3 bytes: first byte is the red, second blue, and
third green (RGB).
The semicolon is only necessary if you have truncated
color values (ex. writing f143 as opposed to 00f143)
and you have number values that come after that could be mistaken
as the missing digets of the command, like in <ESC>[ffa32;123456,
where "123456" are digets you want to be displayed.

Other commands:

B - resets background to the default
F - resets foreground to the default
r - resets foreground and background to the default

Examples:

<ESC>[ff0b105 - sets foreground color to a nice orange
<ESC>[fff2f00 - sets background color to a harsh red
<ESC>[fff; - sets background color to blue. same as <ESC>[f0000ff

*/

namespace etm {
    namespace env {
        constexpr char CONTROL_CHAR_START = '\xE';
        constexpr char CONTROL_CHAR_END = '\xF';
        // Not including the first CONTROL_CHAR_START control char - 
        // number of characters included in a control block.
        // 4 for sizeof(cblock_t), 1 for CONTROL_CHAR_END
        constexpr unsigned int CONTROL_BLOCK_SIZE = 5;
        // Number of bytes used to record offsets for control blocks
        constexpr unsigned int CONTROL_BLOCK_DATA_BYTES = 4;
        // A value large enough to hold the control block index
        typedef int type;
    }

    namespace utf8 {

        typedef std::string string_t;
        typedef string_t::iterator iterator_t;
        typedef string_t::size_type size_type_t;
        typedef char char_t;
        typedef unsigned char uchar_t;
        typedef unsigned int codepoint_t;

        // Returns the number of bytes
        // that the codepoint contains.
        // Assumes that the `index` is valid for the given `source`
        int test(char_t chr);

        // Reads the bytes of a utf8 char.
        // `bytes` is the return value from test(...).
        // Expects `bytes` to be in the range [1,4], and bad things will happen if that's not the case
        unsigned int read(const string_t &source, size_type_t index, int bytes);

        // Returns number of bytes that must be moved backwards
        // to return to the utf8 char header.
        // `index` must be one of the body chars, or the header.
        int lookbehind(const string_t &source, size_type_t index);

        // Encodes the given codepoint as a string
        std::string encode(codepoint_t codepoint);
    }
}

#endif
