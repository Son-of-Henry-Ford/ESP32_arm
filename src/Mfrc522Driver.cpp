#include "Mfrc522Driver.h"

const std::array<MFRC522::PCD_RxGain, 8> Mfrc522Driver::gainLevels = {
    MFRC522::RxGain_18dB, MFRC522::RxGain_23dB, MFRC522::RxGain_18dB_2,
    MFRC522::RxGain_23dB_2, MFRC522::RxGain_33dB, MFRC522::RxGain_38dB,
    MFRC522::RxGain_43dB, MFRC522::RxGain_48dB
};

Mfrc522Driver::Mfrc522Driver(uint8_t ss_pin, uint8_t rst_pin) : mfrc522(ss_pin, rst_pin) {}

bool Mfrc522Driver::init() {
    mfrc522.PCD_Init();
    bool isOk = mfrc522.PCD_PerformSelfTest();
    mfrc522.PCD_Init(); 
    return isOk;
}

void Mfrc522Driver::setPower(uint8_t level) {
    mfrc522.PCD_SetAntennaGain(gainLevels[level > 7 ? 7 : level]);
}

bool Mfrc522Driver::isCardPresent() { return mfrc522.PICC_IsNewCardPresent(); }

String Mfrc522Driver::readUID() {
    if (!mfrc522.PICC_ReadCardSerial()) return "NONE";
    String uid;
    uid.reserve(mfrc522.uid.size * 2);
    const char hexArray[] = "0123456789ABCDEF";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid += hexArray[mfrc522.uid.uidByte[i] >> 4];
        uid += hexArray[mfrc522.uid.uidByte[i] & 0x0F];
    }
    return uid;
}

void Mfrc522Driver::halt() { mfrc522.PICC_HaltA(); mfrc522.PCD_StopCrypto1(); }
void Mfrc522Driver::softReset() { mfrc522.PCD_Reset(); mfrc522.PCD_Init(); }