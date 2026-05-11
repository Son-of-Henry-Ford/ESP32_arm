#pragma once
#include <cstdint>
#include <vector>

class MFRC522 {
public:
    enum PCD_RxGain : uint8_t {
        RxGain_18dB = 0x00,
        RxGain_23dB = 0x01,
        RxGain_18dB_2 = 0x02,
        RxGain_23dB_2 = 0x03,
        RxGain_33dB = 0x04,
        RxGain_38dB = 0x05,
        RxGain_43dB = 0x06,
        RxGain_48dB = 0x07
    };

    struct Uid {
        uint8_t size;
        uint8_t uidByte[10];
    } uid;

    MFRC522(uint8_t ss, uint8_t rst) {}

    void PCD_Init() {}
    bool PCD_PerformSelfTest() { return true; }

    // Mocking for test verification
    static PCD_RxGain lastGain;
    void PCD_SetAntennaGain(PCD_RxGain gain) {
        lastGain = gain;
    }

    static void resetMock() {
        lastGain = (PCD_RxGain)0xFF;
    }

    bool PICC_IsNewCardPresent() { return false; }
    bool PICC_ReadCardSerial() { return false; }
    void PICC_HaltA() {}
    void PCD_StopCrypto1() {}
    void PCD_Reset() {}
};
