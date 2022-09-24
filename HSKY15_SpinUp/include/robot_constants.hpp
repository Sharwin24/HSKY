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
static constexpr uint8_t OPTICAL_SENSOR_PORT = 12;
static constexpr uint8_t CATAPULT_BUTTON_PORT = 'A';
static constexpr uint8_t ULTRASONIC_PING_PORT = 'B';
static constexpr uint8_t ULTRASONIC_ECHO_PORT = 'C';

// Physical robot constants
static constexpr float_t WHEEL_DIAMETER = 3.25f; // [in]
static constexpr float_t WHEEL_TRACK = 11.8f;    // [in]
static constexpr float_t DRIVE_GEAR_RATIO = 60.0f / 36.0f;

// Programming constants
static constexpr float TURN_FACTOR = 0.90f;                 // Turn at 90% of max speed for driver control
static constexpr float DEGREES_TO_ENGAGE_SLIP_GEAR = 15.0f; // [degrees]
static constexpr float P_GAIN_DRIVE = 0.00185f;
static constexpr float I_GAIN_DRIVE = 0.0f;
static constexpr float D_GAIN_DRIVE = 0.0001f;
static constexpr float P_GAIN_DRIVE_ULTRASONIC = 0.1f;
static constexpr float I_GAIN_DRIVE_ULTRASONIC = 0.00008f;
static constexpr float D_GAIN_DRIVE_ULTRASONIC = 0.05;
static constexpr float P_GAIN_TURN = 0.1875f;
static constexpr float I_GAIN_TURN = 0.000008f;
static constexpr float D_GAIN_TURN = 0.15f;