#pragma once

#include <memory>

#include "main.h"
#include "okapi/api/util/mathUtil.hpp"
#include "okapi/impl/chassis/controller/chassisControllerBuilder.hpp"
#include "pros/misc.hpp"
#include "robot_constants.hpp"

using namespace okapi;

namespace src::Chassis {

// Struct for Position and Orientation (Pose)
// Orientation is absolute angle in radians
// [0,0,0] is the starting position of the robot instead of a constant field origin
typedef struct Pose {
    float x;     // [in]
    float y;     // [in]
    float theta; // [rad]
} Pose_t;

enum class StartingPosition { // Figure out all possible starting positions and name them
    RED_FRONT,
    RED_BACK,
    BLUE_FRONT,
    BLUE_BACK,
};

class FieldConstants {
  public:
    FieldConstants(){};
    ~FieldConstants(){};

    void setStartingPosition(StartingPosition position);
    float getTileSize() const { return 24.0f; }   // [in]
    float getFieldSize() const { return 144.0f; } // [in]
    Pose_t getRedGoalPosition() const { return this->redGoalPosition; }
    Pose_t getBlueGoalPosition() const { return this->blueGoalPosition; }

  private:
    StartingPosition startingPosition;
    void setRedGoalPosition(Pose_t pose);
    void setBlueGoalPosition(Pose_t pose);
    Pose_t redGoalPosition;
    Pose_t blueGoalPosition;
};

class OdometrySuite {
  public:
    // Constructor and Destructor
    OdometrySuite();
    ~OdometrySuite();

    // Control Functions
    void reset();
    void update();

    // Getters
    float getXPosition();
    float getYPosition();
    float getOrientation();

  private:
    // Pose Variables
    float xPosition;
    float yPosition;
    float orientation;
    // Last Reset Variables
    float leftEncoderAtLastReset = 0;
    float rightEncoderAtLastReset = 0;
    float horizontalEncoderAtLastReset = 0;
    float orientationAtLastReset = 0;
    // Previous Cycle Variables
    float previousLeftEncoderValue;
    float previousRightEncoderValue;
    float previousHorizontalEncoderValue;
    float previousOrientation;
    float previousGlobalX;
    float previousGlobalY;

    // Math Functions
    void cartesian2Polar(float x, float y, float &r, float &theta);
    void polar2Cartesian(float r, float theta, float &x, float &y);
};

// MotorGroup for the left side of the chassis
static MotorGroup leftChassisMotorGroup = {
    Motor(CHASSIS_PORT_L1, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_L2, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_L3, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_L4, true, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees)};

// MotorGroup for the right side of the chassis
static MotorGroup rightChassisMotorGroup = {
    Motor(CHASSIS_PORT_R1, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_R2, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_R3, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees),
    Motor(CHASSIS_PORT_R4, false, AbstractMotor::gearset::blue, AbstractMotor::encoderUnits::degrees)};

// Chassis pointer for access to chassis model: chassis->getModel()
extern std::shared_ptr<ChassisController> chassis;
static pros::Controller controller = pros::Controller(pros::E_CONTROLLER_MASTER);

// Control loop functions
extern void initialize();
extern void update();
extern void act();

// Chassis motion functions
extern void movePID(float leftTarget, float rightTarget, int ms, float maxV = 1.0f);
extern void gyroPID(float degree, bool CW, int ms = 1000);
extern void turnPID(float degree, bool CW, int ms = 1000);
extern void ultrasonicPID(int distance, int ms = 2000);

// Chassis state functions
extern void setRobotStartingPosition(StartingPosition position);
extern void setChassisBrakeMode(AbstractMotor::brakeMode mode);

// Chassis Odometry functions
extern void odometryTask(void *);
extern Pose_t getRobotPose();
extern void printRobotPoseTask(void *);

// Chassis sensors
static pros::Imu imuSensor = pros::Imu(IMU_PORT);
static pros::ADIUltrasonic ultrasonic(ULTRASONIC_PING_PORT, ULTRASONIC_ECHO_PORT);
static pros::Rotation leftEncoder = pros::Rotation(LEFT_ENCODER_PORT);
static pros::Rotation rightEncoder = pros::Rotation(RIGHT_ENCODER_PORT);
static pros::Rotation horizontalEncoder = pros::Rotation(HORIZONTAL_ENCODER_PORT);

} // namespace src::Chassis