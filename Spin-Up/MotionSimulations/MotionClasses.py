# File with classes for MotionProfile.py
import pygame
import os
from graphics import Image
from dataclasses import dataclass, field
import numpy as np

"""
ControlVector(Pose ipose, double ivel = std::nan(""), double iaccel = 0.0, double ijerk = 0.0)

ProfilePoint(ControlVector ivector, vector<double> iwheel_velocities, double icurvature, double itime)
	@param ivector -> The pose and associated dynamics at this state in the path.
  @param iwheel_velocities ->  The component of the robot's velocity provided by each wheel in meters per second.
  @param icurvature  -> The degree to which the curve deviates from a straight line at this point in 1 / meters.
  @param itime  -> The timestamp for this state relative to the start of the path in seconds.
"""


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
        # Keep theta in range [0, 360)
        newPose.theta = newPose.theta % 360
        if newPose.theta < 0:
            newPose.theta += 360
        self.previousPose = self.pose
        self.pose = Pose(newPose.x, newPose.y, newPose.theta)

    def driveRobot(self, distance: float) -> None:
        """Moves the robot forward by the given distance"""
        # We need to convert the distance to a change in x and y
        # based on the robot's current orientation
        if self.pose.theta == 0:
            newX = self.pose.x
            newY = self.pose.y - distance
        elif self.pose.theta == 180:
            newX = self.pose.x
            newY = self.pose.y + distance
        else:
            newX = self.pose.x + distance * \
                np.cos(np.radians((90 + self.pose.theta) % 360))
            newY = self.pose.y + distance * \
                np.sin(np.radians((90 + self.pose.theta) % 360))
        self.moveRobot(Pose(newX, newY, self.pose.theta))

    def turnRobot(self, newTheta: float) -> None:
        """Turns the robot to the given angle [deg]"""
        self.moveRobot(Pose(self.pose.x, self.pose.y,
                       self.pose.theta + newTheta))

    def draw(self, screen) -> None:
        """Draws the robot on the screen"""
        # offset from pivot to center
        imageW, imageH = self.image.get_size()
        image_rect = self.image.get_rect(
            topleft=(self.pose.x - imageW/2, self.pose.y - imageH/2))
        offset_center_to_pivot = pygame.math.Vector2(
            (self.pose.x, self.pose.y)) - image_rect.center
        # rotated offset from pivot to center
        if not self.previousPose.__eq__(self.pose) and self.previousPose is not None:
            rotated_offset = offset_center_to_pivot.rotate(-self.pose.theta)
        else:  # No rotation
            rotated_offset = offset_center_to_pivot.rotate(0)
        rotated_image_center = (
            self.pose.x - rotated_offset.x, self.pose.y - rotated_offset.y)
        rotated_image = pygame.transform.rotate(self.image, self.pose.theta)
        rotated_image_rect = rotated_image.get_rect(
            center=rotated_image_center)
        screen.blit(rotated_image, rotated_image_rect)

    def reset(self) -> None:
        """Resets the robot to its starting position"""
        self.pose = self.startPose
        self.previousPose = None
