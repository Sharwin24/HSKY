/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.rerun;

import java.util.ArrayDeque;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.Queue;
import java.util.stream.Collectors;

/**
 * Provides a standard command interface for storing and transferring reruns.
 * This command list comprises everything the robot can do, and can easily be
 * saved. Each command represents an action the robot took in a period of 10 ms.
 *
 * @author Prem Chintalapudi 5776E
 */
public enum Command {

    /**
     * Indicates that the robot was commanded to move forward.
     */
    FORWARD,
    /**
     * Indicates that the robot was commanded to move backward.
     */
    BACKWARD,
    /**
     * Indicates that the robot was commanded to turn left.
     */
    LEFT_TURN,
    /**
     * Indicates that the robot was commanded to turn right.
     */
    RIGHT_TURN,
    /**
     * Indicates that the robot was commanded to perform a mobile goal action.
     * The command in a robot is a toggle, so it may be either intake or
     * outtake.
     */
    MOGO,
    /**
     * Indicates that the robot was commanded to perform an autostack.
     */
    AUTOSTACK,
    /**
     * Indicates that the robot was commanded to perform a cone action. The
     * command in a robot is a toggle, so it may be either intake or outtake.
     */
    CONE,
    /**
     * Indicates that the robot was commanded to stack on a stationary goal.
     */
    STATSTACK,
    /**
     * Indicates that the alliance was instructed to load a cone on the aliiance
     * station's loader.
     */
    LOAD,
    /**
     * Indicates that the robot was waiting for instructions.
     */
    NONE,
    //Everything after here is just for itzfx.rerun.translate.Translate purposes only.

    /**
     * Indicates that both the forward and left enums were compressed.
     */
    FL,
    /**
     * Indicates that both the forward and right enums were compressed.
     */
    FR,
    /**
     * Indicates that both the backward and left enums were compressed.
     */
    BL,
    /**
     * Indicates that both the backward and right enums were compressed.
     */
    BR;

    /**
     * Converts a FIFO-Group of commands into a single list of strings. Each
     * string will encode a single list from the source FIFO collection.
     *
     * @param commands the FIFO collection of commands. Each collection within
     * the main one should represent a single 10ms pulse.
     * @return a list of strings that may be saved in a file.
     */
    public static List<String> encode(Queue<List<Command>> commands) {
        return commands.stream().map(pulse -> pulse.stream().map(Objects::toString).collect(Collectors.joining(" "))).collect(Collectors.toList());
    }

    /**
     * Converts a list of encoded Strings to a FIFO-Group of commands. Each
     * string may encode multiple commands. Each list will represent a 10ms
     * pulse.
     *
     * @param commands the list of encoded Strings
     * @return the newly decoded FIFO collection of commands
     */
    public static Queue<List<Command>> decode(List<String> commands) {
        return commands.stream().map(s -> s.split(" ")).map(Arrays::asList).map(l -> l.stream().map(Command::valueOf).collect(Collectors.toList())).collect(Collectors.toCollection(ArrayDeque::new));
    }
}
