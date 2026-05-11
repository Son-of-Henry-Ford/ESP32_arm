#include <Arduino.h>
#include "StepDirDriver.h"
#include "Mfrc522Driver.h"
#include "ManipulatorController.h"

FastAccelStepperEngine engine = FastAccelStepperEngine();

namespace Config {
    const uint8_t X_STEP = 15, X_DIR = 16, X_EN = 17, X_END = 18;
    const uint8_t Y_STEP = 4, Y_DIR = 5, Y_EN = 6, Y_END = 7;
    const uint8_t RF_SS = 10, RF_RST = 9, MAGNET = 8;
}

// Создаем конкретные реализации
StepDirDriver driverX(Config::X_STEP, Config::X_DIR, Config::X_EN);
StepDirDriver driverY(Config::Y_STEP, Config::Y_DIR, Config::Y_EN);
Mfrc522Driver rfidDriver(Config::RF_SS, Config::RF_RST);

// Контроллер работает только с ИНТЕРФЕЙСАМИ (Исправлена опечатка Config::MAGNET)
ManipulatorController manipulator(driverX, driverY, rfidDriver, Config::X_END, Config::Y_END, Config::MAGNET);

// Задача обработки команд от Node.js сервера по UART
void TaskUART(void *pv) {
    String buffer = "";
    buffer.reserve(256);
    bool pendingMove = false;
    
    for (;;) {
        while (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '\n') {
                buffer.trim();
                
                if (buffer.startsWith("SCAN")) {
                    // Запрос: Сканировать метку под текущей позицией манипулятора
                    Serial.println("RES UID " + manipulator.scanAtCurrentPosition());
                    
                } else if (buffer == "HOME") {
                    // Запрос: Калибровка (возврат к Orange Pi ответа HOME_OK)
                    manipulator.home();
                    Serial.println("HOME_OK");
                    
                } else if (buffer.startsWith("MAG ")) {
                    // Запрос: Электромагнит ("MAG 1" или "MAG 0")
                    int state = buffer.substring(4).toInt();
                    manipulator.setMagnet(state > 0);
                    Serial.println("OK");
                    
                } else if (buffer.startsWith("M ")) {
                    // Запрос: Движение к координатам (например: "M 1200 4500")
                    int firstSpace = buffer.indexOf(' ', 2);
                    if (firstSpace > 0) {
                        long targetX = buffer.substring(2, firstSpace).toInt();
                        long targetY = buffer.substring(firstSpace + 1).toInt();
                        manipulator.moveTo(targetX, targetY);
                        pendingMove = true;
                    }
                }
                buffer = "";
            } else if (c != '\r') {
                if (buffer.length() < 256) {
                    buffer += c;
                }
            }
        }
        
        // Если робот двигался и успешно достиг цели, сообщаем серверу
        if (pendingMove && !manipulator.isMoving()) {
            Serial.println("OK");
            pendingMove = false;
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup() {
    Serial.begin(115200);
    while(!Serial && millis() < 3000);
    
    SPI.begin(); // Общая шина для RFID
    engine.init();
    manipulator.init();
    
    // Запуск обработчика команд
    xTaskCreatePinnedToCore(TaskUART, "UART", 4096, NULL, 1, NULL, 0);
    
    // Автоматическая калибровка при включении робота
    manipulator.home();
    Serial.println("SYS:OK: Arm controller ready");
}

void loop() { 
    // Поскольку мы используем FreeRTOS таски (TaskUART) и FastAccelStepper (работает на прерываниях), 
    // стандартный цикл loop не нужен.
    vTaskDelete(NULL); 
}