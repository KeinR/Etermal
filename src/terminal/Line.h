#ifndef ETERMAL_LINE_H_INCLUDED
#define ETERMAL_LINE_H_INCLUDED

#include <string>
#include <vector>
#include <limits>

namespace etm {

    /**
    * A single lines of text as displayed on the terminal screen.
    * @note The deFacto index is the @e character position (so, a 4 byte
    * UTF-8 encoded char is treated as having a length of 1), whereas
    * the deJure index is the @e actual position in the source string
    * (so that UTF-8 char would actually have a length of 4).
    * This is important because when accessing a line index, the caller
    * only cares about contiguous characters, not their byte sizes.
    * @see TextBuffer
    */
    class Line {
    public:
        /// The type of each char
        typedef char value_type;
        /// Unsigned value of each char
        typedef unsigned char u_value_type;
        /// String of chars
        typedef std::basic_string<value_type> string_t;
        /// Index type of char strings
        typedef string_t::size_type size_type;

        /**
        * Dual iterators that describe a range containing
        * an encoded codepoint in a string.
        * @note This goes without saying, but the iterators
        * @ref start and @ref end must be valid when passing
        * this object to a function.
        */
        struct codepoint {
            /// The start of the codepoint, the "header" byte
            string_t::const_iterator start;
            /// The end of the codepoint, the byte after the last
            /// value in the codepoint.
            /// aka, the end point exclusive.
            string_t::const_iterator end;
            /**
            * Default construct a codepoint.
            */
            codepoint();
            /**
            * Construct a codepoint with arguments.
            * @param [in] start The starting "header" byte, inclusive
            * @param [in] start The ending byte, exclusive
            */
            codepoint(
                const string_t::const_iterator &start,
                const string_t::const_iterator &end
            );
            /**
            * Test if the starting "header" byte is equal to the
            * one provided.
            * @return `true` if the two are equal
            */
            bool operator==(value_type c) const;
            /**
            * Test if the starting "header" byte isn't equal to the
            * one provided.
            * @return `true` if the two are @e not equal
            */
            bool operator!=(value_type c) const;
        };

        /**
        * An iterator for a @ref Line.
        * The preferred way to iterate over a Line,
        * for every time an index is accessed the
        * line as to correct it by iterating over the
        * entire string, accounting for zero length
        * sequences (control sequences) and UTF-8
        * codepoints that take up more than 1 byte.
        */
        class iterator {
            /// The parent Line
            Line *parent;
            /// The @e deJure index
            size_type index;
        public:
            /**
            * Construct an iterator.
            * This function should only ever be invoked
            * by `parent`.
            * @param [in] parent The parent line
            * @param [in] index The deJure index of the iterator
            */
            iterator(Line *parent, size_type index);

            /**
            * Check if the iterator is pointing to a valid index.
            * @warning Once an iterator has been invalidated, it is broken -
            * you may not increment, deincrement, dereference, or do anything
            * with it. This is due to how the iterator must read chars to
            * skip them, but also be able to be but into a invalid state when
            * running out of bounds.
            * @return `true` if the iterator is pointing at a valid
            * index, and can be safely dereferenced or 
            */
            bool valid();

            /**
            * Compares less than.
            * @param [in] other the object to compare against
            * @return If `*this`'s index is less than `other`'s
            */
            bool operator<(const iterator &other);
            /**
            * Compares less than or equal to.
            * @param [in] other the object to compare against
            * @return If `*this`'s index is less than or equal to `other`'s
            */
            bool operator<=(const iterator &other);
            /**
            * Compares greater than.
            * @param [in] other the object to compare against
            * @return If `*this`'s index is greater than `other`'s
            */
            bool operator>(const iterator &other);
            /**
            * Compares greater than or equal to.
            * @param [in] other the object to compare against
            * @return If `*this`'s index is greater than or equal to `other`'s
            */
            bool operator>=(const iterator &other);
            /**
            * Compares equal to.
            * @param [in] other the object to compare against
            * @return If `*this`'s index is equal to `other`'s
            */
            bool operator==(const iterator &other);

            /**
            * Create a new iterator that has moved backwards a distance.
            * @note The returned iterator can be invalidated. Always check @ref valid().
            * @param [in] distance The @e deFacto distance to move backwards
            * @return The derived iterator
            */
            iterator operator-(size_type distance);
            /**
            * Create a new iterator that has moved forwards a distance.
            * @note The returned iterator can be invalidated. Always check @ref valid().
            * @param [in] distance The @e deFacto distance to move forwards
            * @return The derived iterator
            */
            iterator operator+(size_type distance);
            /**
            * Moves this iterator a distance backwards.
            * @param [in] distance The @e deFacto distance to move backwards
            * @see operator--()
            */
            void operator-=(size_type distance);
            /**
            * Moves this iterator a distance forwards.
            * @param [in] distance The @e deFacto distance to move forwards
            * @see operator++()
            */
            void operator+=(size_type distance);
            /**
            * Moves this iterator one @e deFacto character backwards.
            * @see operator-=(size_type distance)
            */
            void operator--();
            /**
            * Moves this iterator one @e deFacto character forwards.
            * @see operator+=(size_type distance)
            */
            void operator++();
            /**
            * Extracts a UTF-8 encoded codepoint from the pointed index.
            */
            codepoint operator*();

            /**
            * Gets the @e deJure @ref index.
            * @return The index
            */
            size_type getIndex() const;
        };

    private:
        /// The internal, source string
        string_t string;
        /// The @e deFacto size, number of actual chars regardless of length
        size_type defactoSize;
        /// Is this line terminated by a newline?
        bool newline;
        /// Does this line have a supressed space at the start?
        bool startSpace;
        /**
        * Convert a @e deFacto index to a @e deJure index
        * @param [in] index The @e deFacto index
        * @return The corresponding @e deJure index
        */
        size_type correctIndex(size_type index);
        /**
        * Erase all characters from `index` to the end of the line.
        * @param [in] index The @e deJure index
        * @see erase(size_type index)
        * @see erase(const iterator &start)
        */
        void doErase(size_type index);
    public:

        /**
        * Finds the @e deFacto size of the string, that is,
        * how many actual codepoints are contained within it.
        * @param [in] string The string to measure
        * @return The @e deFacto size
        * @see findDefactoSize(const string_t::const_iterator &start, const string_t::const_iterator &end)
        */
        static size_type findDefactoSize(const string_t &string);
        /**
        * Finds the @e deFacto size of a @e range, that is,
        * how many actual codepoints are contained within it.
        * @param [in] string The string to measure
        * @return The @e deFacto size
        * @see findDefactoSize(const string_t &string)
        */
        static size_type findDefactoSize(const string_t::const_iterator &start, const string_t::const_iterator &end);

        static constexpr size_type max = std::numeric_limits<size_type>::max() / 2;
        Line();
        /**
        * Access the codepoint header of the codepoint starting
        * at the @e deFacto index.
        * @param [in] index The @e deFacto index of the codepoint
        * @return The codepoint's header byte
        * @see getDejure(size_type index)
        */
        value_type &operator[](size_type index);

        /**
        * Get iterator to the last element.
        * @return Iterator to the last element
        */
        iterator last();
        /**
        * Get iterator to the first element.
        * @return Iterator to the first element
        */
        iterator begin();

        /**
        * Get the @e deFacto size of the line.
        * @return The @e deFacto size
        */
        size_type size();
        /**
        * Append a codepoint to the end of the line.
        * @param [in] c The codepoint
        */
        void appendChar(const codepoint &c);
        /**
        * Append a ASCII char to the end of the line.
        * @param [in] chr The character
        */
        void appendChar(value_type chr);
        /**
        * Inserts a codepoint at `index`.
        * @param [in] index Insertion index
        * @param [in] c Codepoint to insert
        */
        void insertChar(size_type index, const codepoint &c);
        /**
        * Inserts an ASCII char at `index`.
        * @param [in] index Insertion index
        * @param [in] c Character to insert
        */
        void insertChar(size_type index, value_type c);
        /**
        * Prepend (insert at index 0) a given UTF-8 encoded string.
        * @param [in] str The string to prepend
        */
        void prependStr(const string_t &str);
        /**
        * Erase all characters from the @e deFacto `index`
        * to the end of the line.
        * @param [in] index The @e deFacto index
        * @see erase(const iterator &start)
        * @see doErase(size_type index)
        */
        void erase(size_type index);
        /**
        * Erase all characters from where the iterator is pointing
        * to the end of the line - including the iterator's position.
        * @warning The iterator will be invalidated
        * @param [in] start The iterator 
        * @see erase(size_type index)
        * @see doErase(size_type index)
        */
        void erase(const iterator &start);
        /**
        * Erases a single character at the provided @e deFacto index.
        * @param [in] index The @e deFacto index
        */
        void eraseChar(size_type index);
        /**
        * Appends a UTF-8 encoded string to the end of the line.
        * @param [in] str The string to append
        */
        void appendStr(const string_t &str);
        /**
        * Appends another line to the end of this line.
        * @param [in] other The line to append
        */
        void appendOther(const Line &other);
        /**
        * Remove the last codepoint from the string.
        * @warning Undefined behavior is invoked if @ref size() returns 0
        */
        void popBack();

        /**
        * Append a zero-length control sequence.
        * @param [in] val The string containing the control
        * @see appendControlFromRange(const string_t &val, size_type index, size_type length)
        * @see env
        */
        void appendControl(const string_t &val);
        /**
        * Append a zero-length control sequence from a range.
        * @param [in] val The source string
        * @param [in] index The start index
        * @param [in] length The number of characters to append
        * @see appendControl(const string_t &val)
        * @see env
        */
        void appendControlFromRange(const string_t &val, size_type index, size_type length);

        // Substring to end
        /**
        * Gets a substring from the @e deFacto index `startInc`
        * to the end of the line.
        * @param [in] startInc The starting index to copy
        * @return The substring
        */
        string_t substr(size_type startInc);

        /**
        * Append this line's @ref string contents to `str`.
        * @param [out] str String to append to
        */
        void copyTo(string_t &str);

        /**
        * Set whether this line is terminated with a newline (`\n`).
        * @param [in] value `true` if yes
        * @see hasNewline()
        */
        void setNewline(bool value);
        /**
        * Set whether this line starts with a zero-length space (` `).
        * @param [in] value `true` if yes
        * @see hasStartSpace()
        */
        void setStartSpace(bool value);

        /**
        * Query whether the line is terminated by a newline.
        * @return `true` if yes
        * @see setNewline(bool value)
        */
        bool hasNewline();
        /**
        * Query whether the line starts with a zero length space.
        * @return `true` if yes
        * @see setStartSpace(bool value)
        */
        bool hasStartSpace();

        /**
        * Gets the byte at the @e deJure index.
        * @param [in] index The @e deJure index
        * @return The byte at the index
        * @see operator[](size_type index)
        */
        value_type &getDejure(size_type index);
        /**
        * Gets the byte size of the line, the @e deJure size.
        * @return The byte size
        * @see size()
        */
        size_type dejureSize();

        /**
        * Get the internal UTF-8 encoded, control seq-mangled string.
        * @return The internal string
        */
        string_t &getString();
    };
}

#endif
