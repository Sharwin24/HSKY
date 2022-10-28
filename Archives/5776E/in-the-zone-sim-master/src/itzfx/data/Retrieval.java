/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.data;

import itzfx.KeyControl;
import itzfx.Robot;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.util.Arrays;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Runs all file input and output for the program.
 *
 * @author Prem Chintalapudi 5776E
 */
public class Retrieval {

    private static final File DATADIR;

    static {
        DATADIR = new File("Data");
        if (!DATADIR.isDirectory()) {
            DATADIR.mkdir();
        }
    }

    private Retrieval() {
    }

    /**
     * Gets the {@link File} representing a folder that holds all the robot,
     * rerun, and control format files. This file can be converted to a path
     * using the {@link File#getPath()} method.
     *
     * @return a file representing the home folder of this application
     */
    public static File getDataDirectory() {
        return DATADIR;
    }

    /**
     * Sets values for a {@link Robot} read from the given .rbt file. The data
     * must have been encoded from the {@link Robot#fileData()} method.
     *
     * @param r the robot to set the values of
     * @param f the file to read values from
     */
    public static void readRobot(Robot r, File f) {
        try {
            Robot.fillRobot(r, Files.readAllLines(f.toPath()).get(0));
        } catch (IOException ex) {
            Logger.getLogger(Retrieval.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    /**
     * Saves values for a {@link Robot} in the given .rbt file. The data will be
     * taken from the {@link Robot#fileData()} method.
     *
     * @param r the robot to pull values from
     * @param f the file to write values to
     */
    public static void writeRobot(Robot r, File f) {
        writeToFile(Arrays.asList(r.fileData()), f);
    }

    /**
     * Gets a saved control format, represented as a {@link KeyControl}. The
     * data must have been encoded from the {@link KeyControl#fileData()}
     * method.
     *
     * @param f the file to read the KeyControl from
     * @return the newly generated KeyControl
     */
    public static KeyControl readKeyControl(File f) {
        try {
            return KeyControl.getKeyControl(Files.readAllLines(f.toPath()).get(0));
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    /**
     * Saves a {@link KeyControl} into a file. The data is collected with the
     * {@link KeyControl#fileData()} method.
     *
     * @param kc the KeyControl to save
     * @param f the file to save to
     */
    public static void writeKeyControl(KeyControl kc, File f) {
        writeToFile(Arrays.asList(kc.fileData()), f);
    }

    /**
     * Loads a saved rerun's data into a {@link Robot}. The saved data must have
     * been pulled from a robot with the {@link Robot#saveRecording()} method.
     *
     * @param r the robot to load the data into
     * @param f the file to pull data from
     */
    public static void readRerun(Robot r, File f) {
        try {
            r.setAuton(Files.readAllLines(f.toPath()));
        } catch (IOException ex) {
            Logger.getLogger(Retrieval.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    /**
     * Convenience method for swallowing IOExceptions thrown during the file
     * reading process. This is only supposed to be called by
     * {@link itzfx.rerun.translate.Translate}, to permit it unrestricted access
     * to rerun data.
     *
     * @param f the character file in which data is stored
     * @return the lines of characters stored in this file, as a list of strings
     */
    public static List<String> read(File f) {
        try {
            return Files.readAllLines(f.toPath());
        } catch (IOException ex) {
            Logger.getLogger(Retrieval.class.getName()).log(Level.SEVERE, null, ex);
            throw new RuntimeException(ex);
        }
    }

    /**
     * Saves a rerun, represented as a list of strings, into a file. The list of
     * strings must have been generated from a {@link Robot}, with the
     * {@link Robot#saveRecording()} method.
     *
     * @param commands the command list to save
     * @param f the file to save to
     */
    public static void writeRerun(List<String> commands, File f) {
        writeToFile(commands, f);
    }

    private static void writeToFile(Iterable<? extends CharSequence> lines, File f) {
        try (PrintWriter p = new PrintWriter(f)) {
        } catch (FileNotFoundException fnfex) {
            throw new RuntimeException(fnfex);
        }
        try {
            Files.write(f.toPath(), lines);
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }
}
