#pragma once
#include "IRfidReader.h"
#include <MFRC522.h>
#include <SPI.h>
#include <array>

class Mfrc522Driver final : public IRfidReader {
private:
    MFRC522 mfrc522;
    static const std::array<MFRC522::PCD_RxGain, 8> gainLevels;

public:
    explicit Mfrc522Driver(uint8_t ss_pin, uint8_t rst_pin);
    bool init() override;
    void setPower(uint8_t level) override;
    bool isCardPresent() override;
    String readUID() override;
    void halt() override;
    void softReset() override;
};