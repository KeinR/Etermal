#ifndef ETERMAL_ARGDATA_H_INCLUDED
#define ETERMAL_ARGDATA_H_INCLUDED

#include <string>

namespace etm {

    /**
    * An abstract class representing a parameter
    * of indeterminate data type.
    * @see data
    * @see ArgFilter
    */
    class ArgData {
    public:
        virtual ~ArgData() = 0;
        /**
        * The best representation of the parameter as a string.
        * @return The parameter's string value
        */
        virtual std::string getString() noexcept = 0;
        /**
        * The best representation of the parameter as a boolean.
        * @return The parameter's bool value
        */
        virtual bool getBool() noexcept = 0;
        /**
        * The best representation of the parameter as an integer.
        * @return The parameter's int value
        */
        virtual int getInt() noexcept = 0;
        /**
        * The best representation of the parameter as a float.
        * @return The parameter's float value
        */
        virtual float getFloat() noexcept = 0;
    };
}

#endif
