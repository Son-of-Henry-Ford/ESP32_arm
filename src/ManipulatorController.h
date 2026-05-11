#pragma once
#include "IMotorAxis.h"
#include "IRfidReader.h"
#include <math.h>

class ManipulatorController {
private:
    IMotorAxis &axisX, &axisY;
    IRfidReader &rfid;
    uint8_t endX, endY, magnet;
    bool isHomed = false;

    // Базовые настройки кинематики для векторного движения (шагов/сек)
    const float MAX_VECTOR_SPEED = 3000.0f;  
    const float MAX_VECTOR_ACCEL = 1500.0f;  

public:
    ManipulatorController(IMotorAxis& x, IMotorAxis& y, IRfidReader& r, uint8_t ex, uint8_t ey, uint8_t m)
        : axisX(x), axisY(y), rfid(r), endX(ex), endY(ey), magnet(m) {}

    void init() {
        axisX.init(); 
        axisY.init(); 
        rfid.init();
        
        pinMode(endX, INPUT_PULLUP); 
        pinMode(endY, INPUT_PULLUP);
        pinMode(magnet, OUTPUT); 
        digitalWrite(magnet, LOW);
    }

    void setMagnet(bool state) { 
        digitalWrite(magnet, state ? HIGH : LOW); 
    }

    // Полноценный цикл калибровки (Homing) с защитой от наводок и ударов
    void home() {
        auto homeOne = [this](IMotorAxis& axis, uint8_t pin) {
            axis.enable(); 
            axis.setCurrentPosition(0);
            
            // 1. Отскок: если концевик уже зажат (LOW), отъезжаем вперед
            if (digitalRead(pin) == LOW) { 
                axis.setAcceleration(8000);
                axis.setMaxSpeed(1000); 
                axis.moveTo(5000); 
                
                while(digitalRead(pin) == LOW) {
                    vTaskDelay(2); // Ждем физического отпускания
                }
                
                // Экстренное торможение
                axis.setAcceleration(200000); 
                axis.stop(); 
                while(axis.isMoving()) vTaskDelay(1); 
                axis.setCurrentPosition(0);
            }

            // 2. Быстрый поиск нуля (движемся в минус)
            axis.setAcceleration(8000);
            axis.setMaxSpeed(3000); 
            axis.moveTo(-1000000); 
            
            while(true) {
                // Программный антидребезг (защита от электромагнитных наводок двигателей)
                if (digitalRead(pin) == LOW) {
                    vTaskDelay(2);
                    if (digitalRead(pin) == LOW) break; // Подтверждаем нажатие
                }
                vTaskDelay(1);
            }
            
            // Экстренное торможение (исключает раздавливание концевика по инерции)
            axis.setAcceleration(200000);
            axis.stop(); 
            while(axis.isMoving()) vTaskDelay(1);

            // 3. Отскок для снятия механического натяжения
            axis.setCurrentPosition(0);
            axis.setAcceleration(8000);
            axis.setMaxSpeed(1000);
            axis.moveTo(500); 
            while(axis.isMoving()) vTaskDelay(1); // Здесь штатное торможение (цель известна)

            // 4. Медленное, максимально точное касание концевика
            axis.setCurrentPosition(0);
            axis.setAcceleration(8000);
            axis.setMaxSpeed(500);
            axis.moveTo(-1000000); 
            
            while(true) {
                if (digitalRead(pin) == LOW) {
                    vTaskDelay(2);
                    if (digitalRead(pin) == LOW) break;
                }
                vTaskDelay(1);
            }
            
            axis.setAcceleration(200000); // Экстренное торможение
            axis.stop();
            while(axis.isMoving()) vTaskDelay(1);

            // 5. Калибровка оси завершена
            axis.setCurrentPosition(0);
        };

        // Сначала паркуем Y (каретку), затем X
        homeOne(axisY, endY); 
        homeOne(axisX, endX);
        
        isHomed = true;
    }

    String scanAtCurrentPosition() {
        if (rfid.isCardPresent()) return rfid.readUID();
        return "NONE";
    }

    // ========================================================
    // Линейная интерполяция через пропорциональную кинематику
    // ========================================================
    void moveTo(long targetX, long targetY) { 
        if (!isHomed) return;

        long currentX = axisX.getCurrentPosition();
        long currentY = axisY.getCurrentPosition();
        
        long dx = targetX - currentX;
        long dy = targetY - currentY;

        if (dx == 0 && dy == 0) return;

        // Явное приведение к float для полной безопасности от переполнений
        float fdx = (float)dx;
        float fdy = (float)dy;

        // Вычисление гипотенузы через специальную функцию (аппаратно ускорена на ESP32)
        float distance = hypotf(fdx, fdy);

        // Вычисляем доли для каждой оси через безопасный float-модуль
        float ratioX = fabsf(fdx) / distance;
        float ratioY = fabsf(fdy) / distance;

        float speedX = MAX_VECTOR_SPEED * ratioX;
        float speedY = MAX_VECTOR_SPEED * ratioY;
        
        float accelX = MAX_VECTOR_ACCEL * ratioX;
        float accelY = MAX_VECTOR_ACCEL * ratioY;

        // Запускаем двигатели с новыми синхронными параметрами
        if (dx != 0) {
            if (speedX < 1.0f) speedX = 1.0f; 
            if (accelX < 1.0f) accelX = 1.0f;
            axisX.setMaxSpeed(speedX);
            axisX.setAcceleration(accelX);
            axisX.moveTo(targetX);
        }
        
        if (dy != 0) {
            if (speedY < 1.0f) speedY = 1.0f; 
            if (accelY < 1.0f) accelY = 1.0f;
            axisY.setMaxSpeed(speedY);
            axisY.setAcceleration(accelY);
            axisY.moveTo(targetY);
        }
    }
    
    bool isMoving() { 
        return axisX.isMoving() || axisY.isMoving(); 
    }
};