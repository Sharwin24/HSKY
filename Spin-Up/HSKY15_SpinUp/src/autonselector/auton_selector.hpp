#pragma once
#include "main.h"

namespace src::AutonSelector {

enum class Auton {
    NO_OPERATION = 0,
    SKILLS = 1,
    AUTON_1 = 2,
    AUTON_2 = 3,
};

extern void initialize();
extern Auton getSelectedAuton();

} // namespace src::AutonSelector