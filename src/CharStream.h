#ifndef char_stream_h
#define char_stream_h
#include <Arduino.h>

class CharStream : public Stream {
   public:
    CharStream(const char* input) : p(input), atEnd(false), pEnd(0) {
    }
    virtual int peek() {
        if (atEnd) return -1;
        char next = *p;
        if (next == 0) {
            atEnd = true;
            pEnd = p;
            return -1;
        } else {
            return next;
        }
    }
    virtual int read() {
        int result = peek();
        if (!atEnd) p++;
        return result;
    }
    virtual int available() {
        if (atEnd) return 0;
        if (pEnd == 0) {
            for (pEnd = p; *pEnd != 0; pEnd++) {
            }
        }
        return pEnd - p;
    }
    virtual size_t write(uint8_t c) {
        return 0;
    }

   private:
    /// @brief Pointer to current/next character in input string
    const char* p;
    /// @brief true if we have already reached the end of the input string
    bool atEnd;
    /// @brief pointer to end of input string, or 0 if not yet found
    const char* pEnd;
};

#endif