#include "env.h"

namespace etm::utf8 {
    // See https://en.wikipedia.org/wiki/UTF-8#Description

    static constexpr int countBytes = 4;
}

int etm::utf8::test(char_t chr) {

    struct test_data {
        // Shift distance to the right,
        // aka number of bytes to chop off
        // to isolate the header
        int shift;
        // AND value for header
        int test;
    };

    // If all tests fail, rollback to extended ASCII
    static constexpr test_data TESTS[countBytes] = {
        {7, 0x00}, // 1 byte
        {5, 0x06}, // 2 bytes
        {4, 0x0E}, // 3 bytes
        {3, 0x1E} // 4 bytes
    };

    uchar_t c = static_cast<uchar_t>(chr);
    for (int i = 0; i < countBytes; i++) {
        uchar_t shf = c >> TESTS[i].shift;
        if (shf == TESTS[i].test) {
            return i + 1; // +1 because zero-index'd
        }
    }
    return 1;
}

unsigned int etm::utf8::read(const std::string &source, std::string::size_type index, int bytes) {

    // AND values to remove headers.
    // First byte omitted because the header is zero
    static constexpr int GET_DATA[countBytes-1] = {
        0x3F,
        0x1F,
        0x0f
    };
    // AND Remove header from a data byte (not header byte)
    static constexpr int GET_DATA_B = 0x7F;

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
    // Value header, not byte header
    constexpr int DATA_SHIFT = 6;
	constexpr int DATA_VAL = 0x02;

    #define GET(offset) (static_cast<uchar_t>(source[index+(offset)]))

    // Check if actually a body char
    if ((GET(0) >> DATA_SHIFT) == DATA_VAL) {
        for (int i = 1; i < countBytes && index - i < source.size(); i++) {
            if ((GET(-i) >> DATA_SHIFT) != DATA_VAL) {
                const int testRes = test(GET(-i));
                if (testRes != 1) {
                    return testRes - 1;
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

    constexpr int DATA_TRUNC = 0x3F;
    constexpr int DATA_BITS = 6;
    constexpr int DATA_HEAD = 0x80;

    if (codepoint <= 0x00007F) { // 1 byte
        result.push_back(codepoint);
    } else {
        int dataBytes;
        int header;
        if (codepoint <= 0x0007FF) { // 2 bytes
            dataBytes = 1;
            header = 0xC0;
        } else if (codepoint <= 0x00FFFF) {
            dataBytes = 2;
            header = 0xE0;
        } else if (codepoint <= 0x10FFFF) {
            dataBytes = 3;
            header = 0xF0;
        } else {
            // Invalid utf-8 codepoint
            dataBytes = 0;
            header = 0;
            result.push_back('\0');
        }
        result.push_back((codepoint >> (DATA_BITS * dataBytes)) | header);
        for (int i = 0; i < dataBytes; i++) {
            result.push_back(((codepoint >> (dataBytes - i)) & DATA_TRUNC) | DATA_HEAD);
        }
    }

    return result;
}
