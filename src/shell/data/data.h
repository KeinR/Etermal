#ifndef ETERMAL_DATA_H_INCLUDED
#define ETERMAL_DATA_H_INCLUDED

#include <string>

#include "ArgData.h"

namespace etm::data {

    /**
    * Represents string data.
    */
    class String: public ArgData {
        /// The string
        std::string str;
    public:
        /**
        * Construct a String from a string.
        * @param [in] str The string
        */
        String(const std::string &str) noexcept;
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };

    /**
    * Represents boolean data.
    */
    class Boolean: public ArgData {
        /// The bool flag
        bool flag;
    public:
        /**
        * Construct a Boolean from a boolean flag.
        * @param [in] flag The flag
        */
        Boolean(bool flag) noexcept;
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };

    /**
    * Represents integer data.
    */
    class Integer: public ArgData {
        /// The int value
        int value;
    public:
        /**
        * Constructs a new object
        * @param [in] value The data
        */
        Integer(int value) noexcept;
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };

    /**
    * Represents floating point data.
    */
    class Float: public ArgData {
        /// The float value
        float value;
    public:
        /**
        * Constructs a new object
        * @param [in] value The data
        */
        Float(float value) noexcept;
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };
}

#endif
