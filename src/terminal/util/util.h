#ifndef ETERMAL_UTIL_H_INCLUDED
#define ETERMAL_UTIL_H_INCLUDED

namespace etm {
    // Sets the glScissor box, with the x/y being the top-left
    // of the box, increasing down
    void setScissor(int x, int y, int width, int height);
    // Set the scissor coords to match the window
    // TEMPORTARY MEASURE
    void resetScissor();
}

#endif
