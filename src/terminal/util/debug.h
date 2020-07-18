#ifndef ETERMAL_UTIL_H_INCLUDED
#define ETERMAL_UTIL_H_INCLUDED

namespace etm {
    void assertGLErr(const char *location);
    const char *getGLErrorStr(int error);
}

#endif
