#ifndef ETERMAL_LINE_H_INCLUDED
#define ETERMAL_LINE_H_INCLUDED

#include <string>
#include <limits>

namespace etm {
    class Line {
    public:
        typedef char value_type;
        typedef std::basic_string<value_type> string_t;
        typedef string_t::size_type size_type;
    private:
        string_t string;
        size_type defactoSize;
        bool newline;
        bool startSpace;
        // Correct the defacto index to get the dejure index
        size_type correctIndex(size_type index);
    public:

        static size_type findDefactoSize(const string_t &string);
        static size_type findDefactoSize(const string_t::const_iterator &start, const string_t::const_iterator &end);

        static constexpr size_type max = std::numeric_limits<size_type>::max() / 2;
        Line();
        value_type &operator[](size_type index);
        size_type size();
        void append(value_type c);
        // Inserts at `index`, pushing forward the last occupant.
        void insert(size_type index, value_type c);
        void prependStr(const string_t &str);
        // Erases to end, starting with `index`
        void erase(size_type index);
        // Erases single char
        void eraseChar(size_type index);
        void appendStr(const string_t &str);
        void appendOther(const Line &other);
        void popBack();

        void appendControl(const string_t &val);

        // Substring to end
        string_t substr(size_type startInc);

        void copyTo(string_t &str);

        void reserve(size_type size);

        void setNewline(bool value);
        void setStartSpace(bool value);

        bool hasNewline();
        bool hasStartSpace();

        // dejure accessors
        value_type getDejure(size_type index);
        size_type dejureSize();
    };
}

#endif
