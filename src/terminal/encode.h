#ifndef ETERMAL_ENV_H_INCLUDED
#define ETERMAL_ENV_H_INCLUDED

#include <string>

namespace etm {
    /**
    * Describes encodings for control sequences.
    */
    namespace ctrl {
        /// The start of a control sequence
        constexpr char CONTROL_CHAR_START = '\xE';
        /// The end of a control sequence
        constexpr char CONTROL_CHAR_END = '\xF';
        /// Not including the first CONTROL_CHAR_START control char - 
        /// number of bytes/characters included in a control block.
        /// 4 for sizeof(@ref type), 1 for CONTROL_CHAR_END
        constexpr unsigned int CONTROL_BLOCK_SIZE = 5;
        /// Number of bytes used to record offsets for control blocks.
        /// Basically sizeof(@ref type)
        constexpr unsigned int CONTROL_BLOCK_DATA_BYTES = 4;
        /// A value large enough to hold the control block index
        typedef int type;
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
