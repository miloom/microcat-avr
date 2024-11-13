#pragma once

#include "minimoto.h"
#include "encoder.h"



class MotorController {
    public:
        void init(uint8_t motor_num);
        int32_t update();
    private:
        Encoder encoder;
        MiniMoto motor;
        float frequency{};
        int32_t zero_direction{};
        uint32_t calibrated_zero{};
        int8_t reversed{}; 
        float amplitude{};
        float phase_offset{};

        void move(int16_t movement);

};