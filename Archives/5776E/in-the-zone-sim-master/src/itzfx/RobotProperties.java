/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

/**
 *
 * @author prem
 */
public class RobotProperties {

    public static RobotProperties create(float robotSpeed, float robotMogoIntakeTime, float robotAutostackTime, float robotStatTime, int robotMogoMaxStack, int robotStatMaxStack, int robotMogoFront) {
        return new RobotProperties(robotSpeed, robotMogoIntakeTime, robotAutostackTime, robotStatTime, robotMogoMaxStack, robotStatMaxStack, robotMogoFront);
    }

    public static RobotProperties getDefault() {
        return create(27.0f, 0.5f, 1.5f, 1.5f, 13, 4, 1);
    }

    public static RobotProperties getFilledVersion(RobotProperties unsafe, RobotProperties original) {
        return create(
                compare(unsafe.robotSpeed, original.robotSpeed),
                compare(unsafe.robotMogoIntakeTime, original.robotMogoIntakeTime),
                compare(unsafe.robotAutostackTime, original.robotAutostackTime),
                compare(unsafe.robotStatTime, original.robotStatTime),
                compare(unsafe.robotMogoMaxStack, original.robotMogoMaxStack),
                compare(unsafe.robotStatMaxStack, original.robotStatMaxStack),
                compare(unsafe.robotMogoFront, original.robotMogoFront)
        );
    }

    private static float compare(float i, float safe) {
        return i == Float.MIN_VALUE ? safe : i;
    }

    private static int compare(int i, int safe) {
        return i == Integer.MIN_VALUE ? safe : i;
    }

    private final float robotSpeed;
    private final float robotMogoIntakeTime;
    private final float robotAutostackTime;
    private final float robotStatTime;
    private final int robotMogoMaxStack;
    private final int robotStatMaxStack;
    private final int robotMogoFront;

    private RobotProperties(float robotSpeed, float robotMogoIntakeTime,
            float robotAutostackTime, float robotStatTime, int robotMogoMaxStack,
            int robotStatMaxStack, int robotMogoFront) {
        this.robotSpeed = robotSpeed;
        this.robotMogoIntakeTime = robotMogoIntakeTime;
        this.robotAutostackTime = robotAutostackTime;
        this.robotStatTime = robotStatTime;
        this.robotMogoMaxStack = robotMogoMaxStack;
        this.robotStatMaxStack = robotStatMaxStack;
        this.robotMogoFront = robotMogoFront;
    }

    /**
     * @return the robotSpeed
     */
    public float getRobotSpeed() {
        return robotSpeed;
    }

    /**
     * @return the robotMogoIntakeTime
     */
    public float getRobotMogoIntakeTime() {
        return robotMogoIntakeTime;
    }

    /**
     * @return the robotAutostackTime
     */
    public float getRobotAutostackTime() {
        return robotAutostackTime;
    }

    /**
     * @return the robotStatTime
     */
    public float getRobotStatTime() {
        return robotStatTime;
    }

    /**
     * @return the robotMogoMaxStack
     */
    public int getRobotMogoMaxStack() {
        return robotMogoMaxStack;
    }

    /**
     * @return the robotStatMaxStack
     */
    public int getRobotStatMaxStack() {
        return robotStatMaxStack;
    }

    /**
     * @return the robotMogoFront
     */
    public int isRobotMogoFront() {
        return robotMogoFront;
    }

    public String fileData() {
        return "" + robotSpeed + " " + robotMogoIntakeTime + " "
                + robotAutostackTime + " " + robotStatTime + " "
                + robotMogoMaxStack + " " + robotStatMaxStack + " "
                + (robotMogoFront > 0);
    }
}
