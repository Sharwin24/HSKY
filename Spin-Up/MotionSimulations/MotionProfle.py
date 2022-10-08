# A visual tool for motion profiling

# 1. Generate a path. A path is constructed from a series of waypoints.
# Each waypoint holds the robot's pose {x, y, theta} at that point
# The path that is returned is a list of ProfilePoints, which is comprised
# of a ControlVector, wheel velocities, curvature, and time

"""
ControlVector(Pose ipose, double ivel = std::nan(""), double iaccel = 0.0, double ijerk = 0.0)

ProfilePoint(ControlVector ivector, vector<double> iwheel_velocities, double icurvature, double itime)
	@param ivector -> The pose and associated dynamics at this state in the path.
  @param iwheel_velocities ->  The component of the robot's velocity provided by each wheel in meters per second.
  @param icurvature  -> The degree to which the curve deviates from a straight line at this point in 1 / meters.
  @param itime  -> The timestamp for this state relative to the start of the path in seconds.
"""

import pygame
from pygame.locals import *
from dataclasses import dataclass
from graphics import Image
import os
from MotionClasses import *

# Pygame setup
WINDOW_WIDTH = 1080
WINDOW_HEIGHT = 1080
SIZE = (WINDOW_WIDTH, WINDOW_HEIGHT)

pygame.init()
screen = pygame.display.set_mode(SIZE)
clock = pygame.time.Clock()
backgroundImage = pygame.image.load(
    os.path.join(
        'MotionSimulations', 'background.png')).convert()
robotImage = pygame.image.load(os.path.join(
    'MotionSimulations', 'robot.png')).convert()

screen.blit(backgroundImage, (0, 0))
# screen.blit(robotImage, (1080/2, 1080/2))

# Object Creation
startPosition = Pose(70, 210, -90)
robot = Robot(1, startPosition, robotImage)
ROBOT_STEP = (1080 / 6) / 16
ROBOT_TURN_STEP = 1  # [deg]


def HandleKeystrokes(keys) -> bool:
    if keys[pygame.K_q]:
        print("Quitting Simulation")
        return False

    if keys[pygame.K_r]:
        print("Resetting Simulation")
        robot.reset()
    elif keys[pygame.K_UP]:
        robot.driveRobot(ROBOT_STEP)
    elif keys[pygame.K_DOWN]:
        robot.driveRobot(-ROBOT_STEP)
    elif keys[pygame.K_LEFT]:
        robot.turnRobot(-ROBOT_TURN_STEP)
    elif keys[pygame.K_RIGHT]:
        robot.turnRobot(ROBOT_TURN_STEP)

    return True


def updateFrame():
    # Draw background
    # Draw all Paths from MotionProfiler
    # Draw robot at current position
    screen.blit(backgroundImage, (0, 0))
    robot.draw(screen)
    print("Robot -> " + str(robot.pose))


continueSim = True
while continueSim:
    clock.tick(24)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            continueSim = False

    if continueSim:
        keys = pygame.key.get_pressed()
        continueSim = HandleKeystrokes(keys)
        updateFrame()
    pygame.display.update()


pygame.quit()
