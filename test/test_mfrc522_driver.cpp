#include <cassert>
#include <iostream>
#include "../Mfrc522Driver.h"
#include "mocks/MFRC522.h"

void test_setPower_within_bounds() {
    MFRC522::resetMock();
    Mfrc522Driver driver(10, 9);

    // Test level 0
    driver.setPower(0);
    assert(MFRC522::lastGain == MFRC522::RxGain_18dB);

    // Test level 4
    driver.setPower(4);
    assert(MFRC522::lastGain == MFRC522::RxGain_33dB);

    // Test level 7
    driver.setPower(7);
    assert(MFRC522::lastGain == MFRC522::RxGain_48dB);

    std::cout << "test_setPower_within_bounds passed" << std::endl;
}

void test_setPower_out_of_bounds() {
    MFRC522::resetMock();
    Mfrc522Driver driver(10, 9);

    // Test level 8 (edge case: should map to gainLevels[7])
    driver.setPower(8);
    assert(MFRC522::lastGain == MFRC522::RxGain_48dB);

    // Test level 255 (should map to gainLevels[7])
    driver.setPower(255);
    assert(MFRC522::lastGain == MFRC522::RxGain_48dB);

    std::cout << "test_setPower_out_of_bounds passed" << std::endl;
}

int main() {
    test_setPower_within_bounds();
    test_setPower_out_of_bounds();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
