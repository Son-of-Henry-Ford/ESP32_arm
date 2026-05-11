#pragma once
#include <string>
#include <cstdint>
#include <iostream>

typedef uint8_t byte;

#define HEX 16
#define HIGH 1
#define LOW 0
#define INPUT_PULLUP 2
#define OUTPUT 3

class String : public std::string {
public:
    String(const char* s = "") : std::string(s) {}
    String(std::string s) : std::string(s) {}
    String(int val, int base = 10) {
        if (base == 16) {
            char buf[10];
            sprintf(buf, "%x", val);
            *this = buf;
        } else {
            *this = std::to_string(val);
        }
    }
    void toUpperCase() {
        for (auto & c: *this) c = toupper(c);
    }
    String& operator+=(const String& other) {
        std::string::operator+=(other);
        return *this;
    }
};

inline void pinMode(uint8_t, uint8_t) {}
inline void digitalWrite(uint8_t, uint8_t) {}
inline void vTaskDelay(uint32_t) {}
