#pragma once
#include "main.h"

// Motor Ports
static constexpr uint8_t CHASSIS_PORT_L1 = 1;
static constexpr uint8_t CHASSIS_PORT_L2 = 2;
static constexpr uint8_t CHASSIS_PORT_L3 = 3;
static constexpr uint8_t CHASSIS_PORT_L4 = 4;

static constexpr uint8_t CHASSIS_PORT_R1 = 5;
static constexpr uint8_t CHASSIS_PORT_R2 = 6;
static constexpr uint8_t CHASSIS_PORT_R3 = 7;
static constexpr uint8_t CHASSIS_PORT_R4 = 8;

static constexpr uint8_t CATAPULT_PORT = 9;
static constexpr uint8_t INTAKE_PORT = 10;

// Sensor Ports
static constexpr uint8_t IMU_PORT = 11;

// Physical robot constants
static constexpr float_t WHEEL_DIAMETER = 3.25; // [in]
static constexpr float_t WHEEL_TRACK = 13.5f;   // [in]
static constexpr float_t DRIVE_GEAR_RATIO = 60.0f / 36.0f;

// Programming constants
static constexpr float TURN_FACTOR = 0.90f;
static constexpr float P_GAIN = 0.00185f;
static constexpr float I_GAIN = 0.0f;
static constexpr float D_GAIN = 0.0001f;
static constexpr float P_GAIN_TURN = 0.1875f;
static constexpr float I_GAIN_TURN = 0.000008f;
static constexpr float D_GAIN_TURN = 0.15f;