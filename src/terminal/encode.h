#ifndef ETERMAL_ENV_H_INCLUDED
#define ETERMAL_ENV_H_INCLUDED

#include <string>

namespace etm {

    /**
    * Describes encodings for control sequences.
    */
    namespace ctrl {
        /// A value large enough to hold the control block index
        typedef unsigned int type;

        typedef char char_t;
        typedef std::string::const_iterator str_iterator_t;

        /**
        * Test if the given char is the @e start of a control block.
        * @param [in] c The char to test
        * @return `true` if yes
        * @see testEnd(char_t c)
        * @see getJump()
        */
        bool testStart(char_t c);
        /**
        * Test if the given char is the @e end of a control block.
        * @param [in] c The char to test
        * @return `true` if yes
        * @see testStart(char_t c)
        * @see getJump()
        */
        bool testEnd(char_t c);
        /**
        * Gets the number of bytes that must be skipped
        * in order to get to the other end of the control block.
        * If you were at the start, it would be the distance to move postitive
        * to get to the end byte, and vice versa.
        * @return The number of bytes to move
        */
        unsigned int getJump();
        /**
        * Decode a control sequence from a range to an index ID.
        * @param [in] start The location of the starting byte [see @see testStart(char_t c)]
        * @param [in] end The end of the string, exclusive (to guard against ill-placed control chars)
        * @return The decoded index ID
        */
        type decode(const str_iterator_t &start, const str_iterator_t &end);
        /**
        * Encode a given index ID as a control string.
        * @param [in] id The ID to encode
        * @return The encoded string
        */
        std::string encode(type id);

    }

    /**
    * Handles processing of UTF-8 encoded strings.
    */
    namespace utf8 {

        /// Type of used string
        typedef std::string string_t;
        /// Iterator of used string
        typedef string_t::iterator iterator_t;
        /// Index type of used string
        typedef string_t::size_type size_type_t;
        /// Character type
        typedef char char_t;
        /// Unsigned character type
        typedef unsigned char uchar_t;
        /// Codepoint type
        typedef unsigned int codepoint_t;

        /**
        * "tests" the given start of a codepoint (char) by reading
        * its bit header and determining from that how many bytes
        * the entire codepoint takes up (including `chr`).
        * @param [in] chr The character to test
        * @return The number of bytes the entire codepoint takes up, including `chr`
        */
        int test(char_t chr);

        /**
        * Decodes a UTF-8 codepoint.
        * @note `bytes` MUST be the result of a call to @ref test(char_t chr),
        * or at the very least be in the range [1,4]
        * @param [in] source The source string
        * @param [in] index The index of the first byte/char
        * @param [in] bytes Number of bytes/chars that make up the codepoint (obtained from @ref test(char_t chr))
        * @return Decoded codepoint, or 0 if the codepoint is `null` or the codepoint is corrupt
        * @see encode(codepoint_t codepoint)
        */
        unsigned int read(const string_t &source, size_type_t index, int bytes);

        /**
        * Returns number of bytes that must be moved backwards
        * to return to the utf8 char header from a position somewhere
        * in the codepoint data.
        * @param [in] source The source string
        * @param [in] index The starting index, MUST be dereferencable!
        * @return The number of bytes that must be moved backwards to return
        * to the codepoint's header (starting byte), or 0 if already there.
        * @see test(char_t chr)
        */
        int lookbehind(const string_t &source, size_type_t index);

        /**
        * Encodes a given codepoint as a sequence of characters/bytes
        * @param [in] codepoint The codepoint to encode
        * @return The encoded codepoint as a string. Will be in the range of [1,4] bytes in size.
        * @see read(const string_t &source, size_type_t index, int bytes)
        */
        std::string encode(codepoint_t codepoint);
    }
}

#endif
