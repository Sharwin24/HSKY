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
from dataclasses import dataclass
from graphics import Image
import os


@dataclass
class Pose():
    x: float
    y: float
    theta: float


@dataclass
class ControlVector():
    pose: Pose
    velocity: float = 0
    accel: float = 0
    jerk: float = 0


@dataclass
class WheelVelocities():
    leftVelocity: float
    rightVelocity: float


@dataclass
class ProfilePoint():
    vector: ControlVector
    wheel_velocities: WheelVelocities
    curvature: float
    time: float


@dataclass
class Path():
    id: int
    motionPoints: list[ProfilePoint]


@dataclass
class PathGenerator():
    maxVelocity: float
    timeStep: float

    def generatePath(self, waypoints: list[Pose]) -> Path:
        """Generates a smooth path that intersects the given waypoints
           Also calculates the curvature and wheel velocities for each point"""
        pass


@dataclass
class MotionProfiler():
    pathGenerator: PathGenerator
    pathsToRun: list[Path]

    def generatePath(self, waypoints: list[Pose]) -> Path:
        """Generates a path from the given waypoints and adds it to the list of paths
           Delegates to the path generator"""
        self.pathsToRun.append(self.pathGenerator.generatePath(waypoints))

    def removePath(self, pathId: int) -> None:
        """Removes the path with the given id from the list of paths"""
        [self.pathsToRun.remove(path)
         for path in self.pathsToRun if path.id == pathId]


@dataclass
class Robot():
    id: int
    pose: Pose
    image: Image

    def moveRobot(self, newPose: Pose) -> None:
        """Moves the robot to a new Pose"""
        pass


@dataclass
class Simulator():
    motionProfiler: MotionProfiler
    robot: Robot

    def run(self) -> None:
        """Runs the simulation"""
        pass


# Graphical Constants
WINDOW_WIDTH = 1080
WINDOW_HEIGHT = 1080
SIZE = (WINDOW_WIDTH, WINDOW_HEIGHT)
# Pygame Setup

pygame.init()
screen = pygame.display.set_mode(SIZE)

backgroundImage = pygame.image.load(
    os.path.join(
        'MotionSimulations', 'background.png')).convert()
robotImage = pygame.image.load(os.path.join(
    'MotionSimulations', 'robot.png')).convert()

screen.blit(backgroundImage, (0, 0))
screen.blit(robotImage, (56, 22))


while True:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()

    pygame.display.update()
