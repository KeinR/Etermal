#ifndef ETERMAL_ARGDATA_H_INCLUDED
#define ETERMAL_ARGDATA_H_INCLUDED

#include <string>

namespace etm {
    class ArgData {
    public:
        virtual ~ArgData() = 0;
        virtual std::string getString() noexcept = 0;
        virtual bool getBool() noexcept = 0;
        virtual int getInt() noexcept = 0;
        virtual float getFloat() noexcept = 0;
    };
}

#endif
