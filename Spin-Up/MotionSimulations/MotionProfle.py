# A visual tool for motion profiling

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

# colors
white = (255, 255, 255)
black = (0, 0, 0)
green = (0, 255, 0)
blue = (0, 0, 128)
red = (255, 0, 0)


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
startPosition = Pose(125, 275, 270)
robot = Robot(1, startPosition, robotImage)
ROBOT_STEP = (1080 / 6) / 16
ROBOT_TURN_STEP = 10  # [deg]


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
        robot.turnRobot(ROBOT_TURN_STEP)
    elif keys[pygame.K_RIGHT]:
        robot.turnRobot(-ROBOT_TURN_STEP)

    return True


def updateFrame():
    # Draw background
    # Draw all Paths from MotionProfiler
    # Draw robot at current position
    screen.blit(backgroundImage, (0, 0))
    robot.draw(screen)
    text = pygame.font.Font(None, 36).render(
        "Robot -> " + str(robot.pose), True, red, black)
    textRect = text.get_rect()
    textRect.center = (154, 950)
    screen.blit(text, textRect)
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
