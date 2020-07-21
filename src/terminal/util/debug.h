#ifndef ETERMAL_UTIL_H_INCLUDED
#define ETERMAL_UTIL_H_INCLUDED

namespace etm {
    /**
    * Assert that an OpenGL error is not set.
    * If an error is set, terminates the program
    * and displays the location of the assertion.
    * @note This is for debugging purposes only when tracing errors.
    * @param [in] location The location to display when an error is detected
    */
    void assertGLErr(const char *location);
    /**
    * Convert a given OpenGL error enum to a string version.
    * @param [in] error An OpenGL error enum
    * @return A string representation of the enum, or "-Unknown error-"
    * if the enum is invalid.
    */
    const char *getGLErrorStr(int error);
}

#endif
