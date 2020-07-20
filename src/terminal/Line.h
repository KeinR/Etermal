#ifndef ETERMAL_LINE_H_INCLUDED
#define ETERMAL_LINE_H_INCLUDED

#include <string>
#include <vector>
#include <limits>

namespace etm {
    class Line {
    public:
        typedef char value_type;
        typedef unsigned char u_value_type;
        typedef std::basic_string<value_type> string_t;
        typedef string_t::size_type size_type;

        class iterator {
            Line *parent;
            size_type index; // Actual index
        public:
            // Constructor should only ever be invoked
            // by the parent (!!!)
            // `index` is a (valid) starting index
            // (that is, it's been corrected).
            // `parent` is the controlling parent Line object.
            iterator(Line *parent, size_type index);

            // Returns true if the iterator is pointing
            // to a valid index
            bool valid();

            bool operator<(const iterator &other);
            bool operator<=(const iterator &other);
            bool operator>(const iterator &other);
            bool operator>=(const iterator &other);
            bool operator==(const iterator &other);

            iterator operator-(size_type distance);
            iterator operator+(size_type distance);
            void operator-=(size_type distance);
            void operator+=(size_type distance);
            void operator--();
            void operator++();
            value_type &operator*();

            // Gets the internal index that the
            // iterator points to.
            // Note that this is the dejure index,
            // and using it yourself could give unpredictable
            // results...
            size_type getIndex() const;
        };

    private:
        string_t string;
        size_type defactoSize;
        bool newline;
        bool startSpace;
        // Correct the defacto index to get the dejure index
        size_type correctIndex(size_type index);
        // Erase from defacto index
        void doErase(size_type index);
    public:

        static size_type findDefactoSize(const string_t &string);
        static size_type findDefactoSize(const string_t::const_iterator &start, const string_t::const_iterator &end);

        static constexpr size_type max = std::numeric_limits<size_type>::max() / 2;
        Line();
        value_type &operator[](size_type index);

        // Get iterator to the last element        
        iterator last();
        // Get iterator to the first element
        iterator begin();

        size_type size();
        void append(value_type c);
        // Inserts at `index`, pushing forward the last occupant.
        void insert(size_type index, value_type c);
        void prependStr(const string_t &str);
        // Erases to end, starting with `index`
        void erase(size_type index);
        void erase(const iterator &start);
        // Erases single char
        void eraseChar(size_type index);
        void appendStr(const string_t &str);
        void appendOther(const Line &other);
        void popBack();

        void appendControl(const string_t &val);
        void appendControlFromRange(const string_t &val, size_type index, size_type length);

        // Substring to end
        string_t substr(size_type startInc);

        void copyTo(string_t &str);

        void setNewline(bool value);
        void setStartSpace(bool value);

        bool hasNewline();
        bool hasStartSpace();

        // dejure accessors
        value_type &getDejure(size_type index);
        size_type dejureSize();
    };
}

#endif
