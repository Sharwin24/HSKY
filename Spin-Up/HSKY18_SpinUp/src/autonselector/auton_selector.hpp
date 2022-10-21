#pragma once
#include "main.h"
#include "pros/apix.h"

#define SELECTOR_HUE 360
#define AUTONS "No Operation", "Example Auton 1", "Example2"

namespace src::AutonSelector {

const char *autonsList[] = {AUTONS, ""};
extern void initialize(const char **autons = autonsList);

} // namespace src::AutonSelector