/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.data;

import itzfx.KeyControl;
import itzfx.Robot;
import itzfx.utils.CssUtils;
import java.io.File;
import java.util.function.Consumer;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;
import javafx.stage.Window;

/**
 * This class provides useful UI methods for saving and loading files. The
 * standard procedure is to prompt the user to save, then open a
 * {@link FileChooser} to allow the user to choose a file, then use a
 * corresponding method in {@link Retrieval} to save/open the selected file.
 *
 * @see Retrieval
 * @author Prem Chintalapudi 5776E
 */
public class FileUI {

    /**
     * Provides a dialog for saving a {@link Robot} to a file. This method
     * prompts the user to enter a name for the new robot file before calling
     * {@link Retrieval#writeRobot(itzfx.Robot, java.io.File)}.
     *
     * @param r the robot to save
     * @param owner the window to block
     *
     * @see FileUI#save(java.lang.String, java.lang.String, javafx.stage.Window,
     * java.util.function.Consumer)
     */
    public static void saveRobot(Robot r, Window owner) {
        save("Robot", "*.rbt", owner, f -> Retrieval.writeRobot(r, f));
    }

    /**
     * Provides a dialog for loading a {@link Robot}'s data from a file. This
     * method prompts the user to select a saved .rbt file before calling
     * {@link Retrieval#readRobot(itzfx.Robot, java.io.File)}.
     *
     * @param r the robot to fill with data
     * @param owner the window to block
     *
     * @see FileUI#load(java.lang.String, java.lang.String, javafx.stage.Window,
     * java.util.function.Consumer)
     */
    public static void fillRobot(Robot r, Window owner) {
        load("Robot", "*.rbt", owner, f -> Retrieval.readRobot(r, f));
    }

    /**
     * Provides a dialog for saving a {@link KeyControl} to a file. This method
     * prompts the user to enter a name for the new KeyControl file before
     * calling
     * {@link Retrieval#writeKeyControl(itzfx.KeyControl, java.io.File)}.
     *
     * @param kc the KeyControl to save
     * @param owner the window to block
     *
     * @see FileUI#save(java.lang.String, java.lang.String, javafx.stage.Window,
     * java.util.function.Consumer)
     */
    public static void saveKeyControl(KeyControl kc, Window owner) {
        save("Controller", "*.kcl", owner, f -> Retrieval.writeKeyControl(kc, f));
    }

    /**
     * Provides a dialog for loading a {@link KeyControl}'s data from a file.
     * This method prompts the user to select a saved .kcl file before calling
     * {@link Retrieval#readKeyControl(java.io.File)}.
     *
     * @param r the robot to load the KeyControl into
     * @param owner the window to block
     *
     * @see FileUI#load(java.lang.String, java.lang.String, javafx.stage.Window,
     * java.util.function.Consumer)
     */
    public static void getKeyControl(Robot r, Window owner) {
        load("Controller", "*.kcl", owner, f -> r.setController(Retrieval.readKeyControl(f)));
    }

    /**
     * Provides a dialog for saving a rerun to a file. This method prompts the
     * user to enter a name for the new rerun file before calling
     * {@link Retrieval#writeRerun(java.util.List, java.io.File)}.
     *
     * @param r the robot from which to pull a recording
     * @param owner the window to block
     *
     * @see FileUI#save(java.lang.String, java.lang.String, javafx.stage.Window,
     * java.util.function.Consumer)
     */
    public static void saveRerun(Robot r, Window owner) {
        save("Rerun", "*.rrn", owner, f -> Retrieval.writeRerun(r.saveRecording(), f));
    }

    /**
     * Provides a dialog for loading a rerun's data from a file. This method
     * prompts the user to select a saved .rrn file before calling
     * {@link Retrieval#readRerun(itzfx.Robot, java.io.File)}.
     *
     * @param r the robot to load the rerun into
     * @param owner the window to block
     * @return the file that was loaded
     *
     * @see FileUI#load(java.lang.String, java.lang.String, javafx.stage.Window,
     * java.util.function.Consumer)
     */
    public static File getRerun(Robot r, Window owner) {
        return load("Autonomous", "*.rrn", owner, f -> Retrieval.readRerun(r, f));
    }

    /**
     * The basic method for prompting the user to select a save file. This
     * method uses a {@link FileChooser} to prompt the user to either create or
     * pick a file to save into. The file is then passed to the provided
     * consumer such that actions may be performed on it.
     *
     * @param descriptor the description of what the file type is
     * @param extension the extension for the file type
     * @param owner the window to block
     * @param action the code that performs actions on the selected file
     */
    public static void save(String descriptor, String extension, Window owner, Consumer<File> action) {
        Alert alert = new Alert(Alert.AlertType.CONFIRMATION, "", ButtonType.YES, ButtonType.NO);
        CssUtils.styleDialog(alert);
        alert.setHeaderText("Save " + descriptor);
        alert.setContentText("Would you like to save this " + descriptor.toLowerCase() + "?");
        alert.showAndWait().filter(bt -> bt == ButtonType.YES).ifPresent(bt -> {
            FileChooser fc = new FileChooser();
            fc.setInitialDirectory(Retrieval.getDataDirectory());
            fc.getExtensionFilters().add(new ExtensionFilter(descriptor, extension));
            File f = fc.showSaveDialog(owner);
            if (f != null) {
                action.accept(f);
            }
        });
    }

    /**
     * The basic method for prompting the user to select a saved file. This
     * method uses a {@link FileChooser} to prompt the user to either create or
     * pick a file to load from. The file is then passed to the provided
     * consumer such that actions may be performed on it.
     *
     * @param descriptor the description of what the file type is
     * @param extension the extension for the file type
     * @param owner the window to block
     * @param action the code that performs actions on the selected file
     * @return the file that was loaded
     */
    public static File load(String descriptor, String extension, Window owner, Consumer<File> action) {
        FileChooser fc = new FileChooser();
        fc.setInitialDirectory(Retrieval.getDataDirectory());
        fc.getExtensionFilters().add(new ExtensionFilter(descriptor, extension));
        File f = fc.showOpenDialog(owner);
        if (f != null) {
            action.accept(f);
        }
        return f;
    }

    public static File load(Window owner, Consumer<File> action, String descriptor, String... extensions) {
        FileChooser fc = new FileChooser();
        fc.setInitialDirectory(Retrieval.getDataDirectory());
        fc.getExtensionFilters().add(new ExtensionFilter(descriptor, extensions));
        File f;
        if ((f = fc.showOpenDialog(owner)) != null) {
            action.accept(f);
        }
        return f;
    }
}
