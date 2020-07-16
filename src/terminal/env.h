#ifndef ETERMAL_ENV_H_INCLUDED
#define ETERMAL_ENV_H_INCLUDED

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

namespace etm::env {
    constexpr char CONTROL_CHAR = '\x5';
    // Not including the control char - 
    // number of characters included in a control block
    constexpr unsigned int CONTROL_BLOCK_SIZE = 4;
    typedef unsigned int cblock_t;
}

#endif
