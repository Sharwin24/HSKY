# HSKY Software TODO List
A list of all the things that need to happen to be competition ready. This list includes everything just for the sake of completion but obviously some programming tasks are dependent on the hardware

## Path Generator & Follower
The math still needs some figuring out. Once the math is figured out for generating a spline to fit waypoints (RobotPoses), we need to figure out how to represent it
in C++ as a "follow-able" path using our "Path-Follower". There are a lot of sub-tasks here that I'll fill in later

## Testing
The entire code-base is basically untested. I've done some testing on high school robots to iron out build and runtime bugs but the more complicated aspects of our code need proper testing. Sensors are completely untested and will need to be tuned depending on the hardware. All PID control needs to be tuned for every mechanism.

## Autonomous Selector
I've created a simple selector using the LittleV Graphics Library based off a template. This is untested and also doesn't provide what I think we eventually want. Because we are using Odometry and motion-profiling, I'd like our selector/graphical interface to display real-time odometry readings, sensor values, drawn out paths from our generator when selecting autons, etc. The ceiling is very high with the graphical interface.

There are a few options to doing so:
<ul>
<li>The LittleV Graphics Library</li>
<li>The <code>pros::lcd</code> (simplest)</li>
<li><code>pros::screen</code></li>
</ul>

## Unity Simulation
Ryan is currently working on this and can update this section

## I'm probably missing a lot but it's fine
