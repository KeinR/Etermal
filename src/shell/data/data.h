#ifndef ETERMAL_DATA_H_INCLUDED
#define ETERMAL_DATA_H_INCLUDED

#include <string>

#include "ArgData.h"

namespace etm::data {
    class String: public ArgData {
        std::string str;
    public:
        String(const std::string &str) noexcept;
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };

    class Boolean: public ArgData {
        bool flag;
    public:
        Boolean(bool flag) noexcept;
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };

    class Integer: public ArgData {
        int value;
        bool parseFailed;
    public:
        // If parse failed, `failed()` = true
        // and `value` is set to 0
        Integer(const std::string &raw) noexcept;
        bool failed() noexcept; // Did the parse fail?
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };

    class Float: public ArgData {
        float value;
        bool parseFailed;
    public:
        // If parse failed, `failed()` = true
        // and `value` is set to 0
        Float(const std::string &raw) noexcept;
        bool failed() noexcept; // Did the parse fail?
        std::string getString() noexcept override;
        bool getBool() noexcept override;
        int getInt() noexcept override;
        float getFloat() noexcept override;
    };
}

#endif
