#pragma once
#include "main.h"

// All constants relevent for the 18" robot

// Motor Ports
static constexpr uint8_t CHASSIS_PORT_L1 = 1;
static constexpr uint8_t CHASSIS_PORT_L2 = 2;
static constexpr uint8_t CHASSIS_PORT_L3 = 3;
static constexpr uint8_t CHASSIS_PORT_L4 = 4;

static constexpr uint8_t CHASSIS_PORT_R1 = 5;
static constexpr uint8_t CHASSIS_PORT_R2 = 6;
static constexpr uint8_t CHASSIS_PORT_R3 = 7;
static constexpr uint8_t CHASSIS_PORT_R4 = 8;

static constexpr uint8_t FLYWHEEL_TOP_PORT = 9;
static constexpr uint8_t FLYWHEEL_BOTTOM_PORT = 10;
static constexpr uint8_t INDEXER_PORT = 11;
static constexpr uint8_t INTAKE_PORT = 12;

// Sensor Ports
static constexpr uint8_t IMU_PORT = 13;
static constexpr uint8_t OPTICAL_SENSOR_PORT = 14;
static constexpr uint8_t LEFT_ENCODER_PORT = 15;
static constexpr uint8_t RIGHT_ENCODER_PORT = 16; // Encoders are V5 Rotation Sensors
static constexpr uint8_t HORIZONTAL_ENCODER_PORT = 17;
static constexpr uint8_t ULTRASONIC_PING_PORT = 'B';
static constexpr uint8_t ULTRASONIC_ECHO_PORT = 'C';

// Physical robot constants
static constexpr float_t DRIVE_GEAR_RATIO = 36.0f / 84.0f;
static constexpr float_t WHEEL_DIAMETER = 4.125f;                 // [in]
static constexpr float_t WHEEL_TRACK = 11.8f;                     // [in]
static constexpr float_t ENCODER_WHEEL_DIAMETER = 2.75f;          // [in]
static constexpr float_t CENTER_TO_LEFT_ENCODER = 6.298f / 2.0f;  // [in]
static constexpr float_t CENTER_TO_RIGHT_ENCODER = 6.298f / 2.0f; // [in]
static constexpr float_t CENTER_TO_HORIZONTAL_ENCODER = 4.676f;   // [in]

// Programming constants
static constexpr float TURN_FACTOR = 0.90f; // Scalar factor for turning in driver control
static constexpr float P_GAIN_DRIVE = 0.00185f;
static constexpr float I_GAIN_DRIVE = 0.0f;
static constexpr float D_GAIN_DRIVE = 0.0001f;
static constexpr float P_GAIN_DRIVE_ULTRASONIC = 0.1f;
static constexpr float I_GAIN_DRIVE_ULTRASONIC = 0.00008f; // None of these are tuned
static constexpr float D_GAIN_DRIVE_ULTRASONIC = 0.05;
static constexpr float P_GAIN_TURN = 0.1875f;
static constexpr float I_GAIN_TURN = 0.000008f;
static constexpr float D_GAIN_TURN = 0.15f;
static constexpr pros::c::optical_rgb_s_t RED_COLOR = {255, 0, 0};  // TODO: Find actual value for red on roller using Optical sensor
static constexpr pros::c::optical_rgb_s_t BLUE_COLOR = {0, 0, 255}; // TODO: Find actual value for blue on roller using Optical sensor