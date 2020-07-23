#include "encode.h"

namespace etm::ctrl {
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
}

bool etm::ctrl::testStart(char_t c) {
    return c == CONTROL_CHAR_START;
}
bool etm::ctrl::testEnd(char_t c) {
    return c == CONTROL_CHAR_END;
}
unsigned int etm::ctrl::getJump() {
    return CONTROL_BLOCK_SIZE;
}
etm::ctrl::type etm::ctrl::decode(const str_iterator_t &start, const str_iterator_t &end) {
    type value = 0;
    int shift = 8 * static_cast<int>(CONTROL_BLOCK_DATA_BYTES);
    for (str_iterator_t it = start + 1, seqEnd = it + CONTROL_BLOCK_DATA_BYTES; it < seqEnd; ++it) {
        if (it >= end) {
            // Hit the string stop - corrupted?
            return 0;
        }
        shift -= 8;
        // Note:
        // static_cast<int>(char) will return the char WITH THE SIGN.
        // so -128 (char) will be -128 (int).
        // As you may know, this is annoying when doing bit manipulation.
        value |= static_cast<type>(static_cast<unsigned char>(*it)) << shift;
    }
    return value;
}
std::string etm::ctrl::encode(type id) {
    std::string str;
    // Mod/control block flag
    str.push_back(CONTROL_CHAR_START);
    // Encode 32 bit unsigned integer index, `id`, as 4 bytes in the string
    int shift = 8 * CONTROL_BLOCK_DATA_BYTES;
    for (unsigned int n = 0; n < CONTROL_BLOCK_DATA_BYTES; n++) {
        shift -= 8;
        str.push_back(static_cast<char_t>((id >> shift) & 0xff));
    }
    str.push_back(CONTROL_CHAR_END);
    return str;
}



namespace etm::utf8 {
    /// @see https://en.wikipedia.org/wiki/UTF-8#Description
    static constexpr int countBytes = 4;
}

int etm::utf8::test(char_t chr) {

    // Information used in tests
    struct test_data {
        // Shift distance to the right,
        // aka number of bytes to chop off
        // to isolate the header of the header byte
        int shift;
        // Actual value of the header after its been shifted
        int test;
    };

    // If all tests fail, rollback to extended ASCII.
    // Of course, we could omit the ASCII test, but it's
    // more efficient, given that most chars will be ASCII,
    // to test ASCII first.
    static constexpr test_data TESTS[countBytes] = {
        {7, 0x00}, // 1 byte
        {5, 0x06}, // 2 bytes
        {4, 0x0E}, // 3 bytes
        {3, 0x1E} // 4 bytes
    };

    uchar_t c = static_cast<uchar_t>(chr);
    for (int i = 0; i < countBytes; i++) {
        // Remove data bytes and isolate header
        uchar_t shf = c >> TESTS[i].shift;
        // Test if the shifted value equals the header
        if (shf == TESTS[i].test) {
            return i + 1; // +1 because zero-index'd
        }
    }
    // No tests passed, must be extended ASCII
    return 1;
}

unsigned int etm::utf8::read(const std::string &source, std::string::size_type index, int bytes) {

    // AND values to remove headers.
    // First byte omitted because the header
    // for when the codepoint length is 1 byte (ASCII),
    // The header is 0x00, so there's no point in ANDing.
    static constexpr int GET_DATA[countBytes-1] = {
        0x3F,
        0x1F,
        0x0f
    };
    // AND Remove header from a data byte (not header byte)
    // GET_DATA_B = 0x7F  =  01111111
    // header     = 0x80  =  10000000
    // Logical AND ------------------
    // Result             =  00000000
    // Of course, this example doesn't deal with data in the body
    // portion of the byte. Hoewver, it's very clear that the last 7
    // bits will have their values preserved.
    static constexpr int GET_DATA_B = 0x7F;

    // A macro for my sanity.
    // Unpredictable things can happen when shifting and casting
    // signed values. This garuntees that the right value will be given.
    #define GET(offset) (static_cast<uchar_t>(source[index+(offset)]))

    // Special provision for ASCII (optimization)
    if (bytes == 1) {
        return GET(0);
    } else {
        // ( // Take header byte and and it to zero the header bits
        //    source[index] & GET_DATA[ // We don't keep data for ASCII, so
        //                             bytes - 2 // decriment to account for that,
        //                             ]        // then one because zero index
        // )
        // << // Shift to make room for lesser bits
        // (
        //    (bytes - 1) // number of bytes after this one
        // * 6 // Number of bits per byte
        // )
        unsigned int result = (GET(0) & GET_DATA[bytes - 2]) << ((bytes - 1) * 6);
        // b=1 because we already read one
        for (int b = 1; b < bytes; b++) {
            if (index + b >= source.size()) {
                // Char corrupt
                return 0;
            }
            // Zero the byte header
            // (source[index + b] & GET_DATA_B)
            // << // Shift it to make room for remaining bits
            // // Shift of the header byte's bits, reduced by 6 for
            // // every byte after
            // ((bytes - b - 1)
            // * 6)
            result |= (GET(b) & GET_DATA_B) << ((bytes - b - 1) * 6);
        }
        return result;
    }

    #undef GET

}

int etm::utf8::lookbehind(const std::string &source, std::string::size_type index) {
    // Ammount to shift to remove the data portion
    // of a non-header byte and isolate the header
    constexpr int DATA_SHIFT = 6;
    // Value of the header byte
	constexpr int DATA_VAL = 0x02;

    // A macro for my sanity.
    // Unpredictable things can happen when shifting and casting
    // signed values. This garuntees that the right value will be given.
    #define GET(offset) (static_cast<uchar_t>(source[index+(offset)]))

    // Check if actually a body byte (not a header)
    if ((GET(0) >> DATA_SHIFT) == DATA_VAL) {
        // Look behind and look for a header 
        for (int i = 1; i < countBytes && index - i < source.size(); i++) {
            if ((GET(-i) >> DATA_SHIFT) != DATA_VAL) {
                // Not a data byte - must be a header.

                // Determine the size of the codepoint
                // described by the byte.
                // If it's 1, then that means that
                // it's an ASCII (or extended ASCII) char,
                // and that all the bytes that were just read are
                // extended ASCII, futher meaning that
                // the current character is a codepoint
                // header, and no distance needs to be moved.
                const int testRes = test(GET(-i));
                if (testRes != 1) {
                    return i;
                } else {
                    return 0;
                }
            }
        }
    }

    #undef GET

    // Actually a header, no distance must be moved
    return 0;
}

std::string etm::utf8::encode(codepoint_t codepoint) {
    std::string result;

    // 0x3F = 00111111 = 6 bits = the size of a data chunk.
    // Used to AND and truncate values to 6 bits.
    constexpr int DATA_TRUNC = 0x3F;
    // Number of bits in DATA_TRUNC
    constexpr int DATA_BITS = 6;
    // Value of data section header
    constexpr int DATA_HEAD = 0x80;

    if (codepoint <= 0x00007F) { // 1 byte
        // Special provision for ASCII
        result.push_back(codepoint);
    } else {
        // Number of bytes in the data section of the byte header
        int dataBytes;
        // bit header for byte header
        int header;
        if (codepoint <= 0x0007FF) { // 2 bytes
            dataBytes = 1;
            header = 0xC0;
        } else if (codepoint <= 0x00FFFF) { // 3 bytes
            dataBytes = 2;
            header = 0xE0;
        } else if (codepoint <= 0x10FFFF) { // 4 bytes
            dataBytes = 3;
            header = 0xF0;
        } else {
            // Invalid utf-8 codepoint
            dataBytes = 0;
            header = 0;
            result.push_back('\0');
        }
        // Add the header
        result.push_back((codepoint >> (DATA_BITS * dataBytes)) | header);
        // Add the trailing data bytse
        for (int i = 0; i < dataBytes; i++) {
            result.push_back(((codepoint >> (dataBytes - i)) & DATA_TRUNC) | DATA_HEAD);
        }
    }

    return result;
}
