using namespace vex;

extern brain Brain;

// VEXcode devices
extern controller Controller1;
extern motor RFM;
extern motor LFM;
extern motor RBM;
extern motor LBM;
extern inertial InertSens;
extern motor DR4B_left;
extern motor DR4B_right;
extern motor Intake;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );