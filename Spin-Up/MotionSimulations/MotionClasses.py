# File with classes for MotionProfile.py
import pygame
import os
from graphics import Image
from dataclasses import dataclass, field
import numpy as np


@dataclass
class Pose():
    x: float
    y: float
    theta: float

    def __repr__(self) -> str:
        return "[{}, {}, {}]".format(self.x, self.y, self.theta)

    def __eq__(self, __o: object) -> bool:
        if isinstance(__o, Pose):
            return self.x == __o.x and self.y == __o.y and self.theta == __o.theta
        else:
            return False


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

    def clearPaths(self) -> None:
        """Removes all paths from the list of paths"""
        self.pathsToRun.clear()


@dataclass
class Robot():
    id: int
    pose: Pose
    image: Image
    startPose: Pose = None
    previousPose: Pose = None

    def __post_init__(self):
        self.startPose = self.pose

    def moveRobot(self, newPose: Pose) -> None:
        """Moves the robot to a new Pose"""
        self.previousPose = self.pose
        self.pose = newPose

    def driveRobot(self, distance: float) -> None:
        """Moves the robot forward by the given distance"""
        # We need to convert the distance to a change in x and y
        # based on the robot's current orientation
        newX = self.pose.x + distance * np.cos(np.radians(self.pose.theta))
        newY = self.pose.y + distance * np.sin(np.radians(self.pose.theta))
        self.moveRobot(Pose(newX, newY, self.pose.theta))

    def turnRobot(self, newTheta: float) -> None:
        """Turns the robot to the given angle [deg]"""
        self.moveRobot(Pose(self.pose.x, self.pose.y, newTheta))

    def draw(self, screen) -> None:
        """Draws the robot on the screen"""
        if not self.previousPose.__eq__(self.pose) and self.previousPose is not None:
            print("Redrawing Robot angle")
            self.image = pygame.transform.rotate(self.image, self.pose.theta)
        screen.blit(self.image, (self.pose.x, self.pose.y))

    def reset(self) -> None:
        """Resets the robot to its starting position"""
        self.pose = self.startPose
        self.previousPose = None
