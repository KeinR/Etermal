#ifndef ETERMAL_LINE_H_INCLUDED
#define ETERMAL_LINE_H_INCLUDED

#include <vector>
#include <limits>

#include "Character.h"

namespace etm {
    class Line {
    public:
        typedef std::vector<Character> string_t;
        typedef string_t::size_type size_type;
    private:
        string_t string;
        bool newline;
        bool startSpace;
    public:
        static constexpr size_type max = std::numeric_limits<size_type>::max() / 2;
        Line();
        Character &operator[](size_type index);
        size_type size();
        void append(const Character &c);
        // Inserts at `index`, pushing forward the last occupant.
        void insert(size_type index, const Character &c);
        void insertStr(size_type index, const string_t &str);
        void erase(size_type index, size_type length = max);
        void appendStr(const string_t &str);
        void appendOther(const Line &other);
        void popBack();

        string_t substr(size_type startInc, size_type length = max);

        void copyTo(string_t &str);

        void reserve(size_type size);

        void setNewline(bool value);
        void setStartSpace(bool value);

        bool hasNewline();
        bool hasStartSpace();
    };
}

#endif
