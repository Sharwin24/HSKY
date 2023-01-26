#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
controller Controller1 = controller(primary);
motor RFM = motor(PORT20, ratio18_1, false);
motor LFM = motor(PORT10, ratio18_1, false);
motor RBM = motor(PORT11, ratio18_1, false);
motor LBM = motor(PORT1, ratio18_1, false);
inertial InertSens = inertial(PORT16);
motor DR4B_left = motor(PORT2, ratio18_1, true);
motor DR4B_right = motor(PORT3, ratio18_1, false);
motor Intake = motor(PORT9, ratio18_1, false);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}