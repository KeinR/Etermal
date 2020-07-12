#include "Glyph.h"

etm::Glyph::Glyph():
    index(0), style(0) {
}
// With args
etm::Glyph::Glyph(int index, const Color &color, charStyle::type style):
    index(index), color(color), style(style) {
}
