/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.rerun;

import itzfx.data.FileUI;
import itzfx.data.Retrieval;
import itzfx.fxml.FXMLController;
import itzfx.utils.CssUtils;
import java.math.BigDecimal;
import java.util.ArrayDeque;
import java.util.Deque;
import java.util.List;
import java.util.Queue;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.stage.Window;

/**
 * This class contains several methods for the purpose of converting saved
 * reruns to RobotC code. The generated code should then be copy-pastable into a
 * RobotC file, though this feature has not been thoroughly tested.
 *
 * <p>
 * This feature relies on an API being present in the RobotC code, with the
 * following signatures:
 * </p>
 *
 * <p>
 * void drive(byte powerLeft, byte powerRight, long timeInMillis)
 *
 * This method should set the motor power for the left side of the drive to
 * "powerLeft", and that of the right side of the drive to "powerRight" for the
 * indicated time in milliseconds.
 * </p>
 *
 * <p>
 * void move(byte powerLeft, byte powerRight, float distanceInInches)
 *
 * This method should set the motor power for the left side of the drive to
 * "powerLeft", and that of the right side of the drive to "powerRight" for the
 * indicated distance in inches.
 * </p>
 *
 * <p>
 * task mogoTask
 *
 * This task should determine whether to intake or outtake a mobile goal, and it
 * should do so outside of the main user control thread. The generated code
 * <b>will</b> call "startTask" on this task.
 * </p>
 * <p>
 * task autostackTask
 *
 * This task should automatically intake a cone, if one is not already held, and
 * it should stack it on the held mobile goal, automatically incrementing. This
 * should not happen on the control loop's thread. The generated code
 * <b>will</b> call "startTask" on this task.
 * </p>
 *
 * <p>
 * task coneTask
 *
 * This task should automatically either intake or outtake a cone, depending on
 * whether or not one is currently held. It may happen on the control loop's
 * thread (though long-running implementations should not happen on the control
 * loop). The generated code <b>will</b> call "startTask" on this task.
 * </p>
 *
 * <p>
 * task statTask
 *
 * This task should stack a cone on a stationary goal, automatically
 * incrementing its height. This may not happen on the control loop's thread,
 * though the simulation pauses all other activity until this stack operation
 * has concluded. The generated code <b>will</b> call "startTask" on this task.
 * </p>
 *
 * @author Prem Chintalapudi 5776E
 */
public final class Translate {

    private static Deque<Segment> translate(Queue<List<Command>> commands) {
        Deque<Segment> graduated = new ArrayDeque<>();
        reduceToggles(commands);
        reduceDrives(commands);
        List<Command> singular = commands.stream().filter(l -> !l.isEmpty()).map(l -> l.get(0)).collect(Collectors.toList());
        for (int i = 0; i < singular.size(); i++) {
            Command command = singular.get(i);
            if (graduated.size() > 0 && graduated.peekLast().c == command) {
                graduated.peekLast().length++;
            } else {
                graduated.add(new Segment(command));
            }
        }
        return graduated;
    }

    /**
     * Gets a list of strings representing lines of code that might be generated
     * for an equivalent time-based program.
     *
     * @param commands the rerun-generated list of {@link Command commands} to
     * translate
     * @return the translated code, formatted to meet the method requirements as
     * shown in the documentation for {@link Translate this class}.
     *
     * @see Translate#translate(java.util.Queue)
     * @see Translate#to10Millis(itzfx.rerun.Translate.Segment)
     */
    public static Stream<String> translateTime(Queue<List<Command>> commands) {
        return translate(commands).stream().filter(Translate::filterZeroes).map(Translate::to10Millis);
    }
//
//    /**
//     * Gets a list of strings representing lines of code that might be generated
//     * for an equivalent distance-based program, based on the given robot's
//     * speed.
//     *
//     * @param commands the rerun-generated list of {@link Command commands} to
//     * translate
//     * @param r the robot from which to obtain a speed
//     * @return the translated code, formatted to meet the method requirements as
//     * shown in the documentation for {@link Translate this class}.
//     *
//     * @see Translate#translate(java.util.Queue)
//     * @see Translate#toDistance(itzfx.rerun.Translate.Segment, itzfx.Robot)
//     */
//    public static List<String> translateDistance(Queue<List<Command>> commands, Robot r) {
//        return translate(commands).stream().filter(Translate::filterZeroes).map(s -> toDistance(s, r)).collect(Collectors.toCollection(ArrayList::new));
//    }

    private static boolean filterZeroes(Segment s) {
        Command c = s.c;
        switch (c) {
            case FORWARD:
            case BACKWARD:
            case LEFT_TURN:
            case RIGHT_TURN:
            case FR:
            case BR:
            case FL:
            case BL:
                if (s.length == 0) {
                    return false;
                }
        }
        return true;
    }

    private static String to10Millis(Segment s) {
        switch (s.c) {
            case FORWARD:
                return "drive(127, 127, " + s.length * 100 + ");";
            case LEFT_TURN:
                return "drive(-127, 127, " + s.length * 100 + ");";
            case BACKWARD:
                return "drive(-127, -127, " + s.length * 100 + ");";
            case RIGHT_TURN:
                return "drive(127, -127, " + s.length * 100 + ");";
            case FR:
                return "drive(127, 0, " + s.length * 100 + ");";
            case BR:
                return "drive(0, -127, " + s.length * 100 + ");";
            case FL:
                return "drive(0, 127, " + s.length * 100 + ");";
            case BL:
                return "drive(-127, 0, " + s.length * 100 + ");";
            case MOGO:
                return "startTask(mogoTask);";
            case CONE:
                return "startTask(coneTask);";
            case AUTOSTACK:
                return "startTask(autostackTask);";
            case STATSTACK:
                return "startTask(statTask);";
            case NONE:
                return "wait10Msec(" + s.length * 10 + ");";
        }
        return null;
    }

//    private static String toDistance(Segment s, Robot r) {
//        switch (s.c) {
//            case FORWARD:
//                return "move(127, 127, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case LEFT_TURN:
//                return "move(-127, 127, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case BACKWARD:
//                return "move(-127, -127, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case RIGHT_TURN:
//                return "move(127, -127, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case FR:
//                return "move(127, 0, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case BR:
//                return "move(0, -127, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case FL:
//                return "move(0, 127, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case BL:
//                return "move(-127, 0, " + truncate(s.length / 10f * r.getSpeed()) + ");";
//            case MOGO:
//                return "startTask(mogoTask);";
//            case CONE:
//                return "startTask(coneTask);";
//            case AUTOSTACK:
//                return "startTask(autostackTask);";
//            case STATSTACK:
//                return "startTask(statTask);";
//            case NONE:
//                return "wait10Msec(" + s.length * 10 + ");";
//        }
//        return null;
//    }

    private static float truncate(float in) {
        return new BigDecimal(String.valueOf(in)).movePointRight(3).intValue() / 1000.0f;
    }

    private static void reduceToggles(Queue<List<Command>> commands) {
        MutableBoolean[] bools = new MutableBoolean[4];
        for (int i = 0; i < 4; i++) {
            bools[i] = new MutableBoolean();
        }
        commands.forEach(l -> l.removeIf(c -> c == Command.LOAD));
        commands.stream().peek(l -> validate(bools[0], Command.MOGO, l)).peek(l -> validate(bools[1], Command.CONE, l))
                .peek(l -> validate(bools[2], Command.AUTOSTACK, l)).forEach(l -> validate(bools[3], Command.STATSTACK, l));
    }

    private static void validate(MutableBoolean had, Command command, List<Command> verify) {
        if (had.b) {
            if (verify.contains(command)) {
                verify.remove(command);
            } else {
                had.b = false;
            }
        } else {
            if (verify.contains(command)) {
                had.b = true;
                verify.clear();
                verify.add(command);
            }
        }
    }

    private static class MutableBoolean {

        private boolean b;
    }

    private static void reduceDrives(Queue<List<Command>> commands) {
        commands.stream().filter(l -> l.size() > 1).forEach(Translate::reduceDrives);
    }

    private static void reduceDrives(List<Command> commands) {
        boolean f = commands.contains(Command.FORWARD);
        boolean b = commands.contains(Command.BACKWARD);
        boolean l = commands.contains(Command.LEFT_TURN);
        boolean r = commands.contains(Command.RIGHT_TURN);
        commands.clear();
        if (f) {
            if (b) {
                if (l) {
                    commands.add(r ? Command.NONE : Command.LEFT_TURN);
                } else {
                    commands.add(r ? Command.RIGHT_TURN : Command.NONE);
                }
            } else {
                if (l) {
                    commands.add(r ? Command.FORWARD : Command.FL);
                } else {
                    commands.add(Command.FR);
                }
            }
        } else {
            if (b) {
                if (l) {
                    commands.add(r ? Command.BACKWARD : Command.BL);
                } else {
                    commands.add(Command.BR);
                }
            } else {
                commands.add(Command.NONE);
            }
        }
    }

    private static class Segment {

        private final Command c;
        private long length;

        public Segment(Command c) {
            this.c = c;
            length = 1;
        }
    }

    /**
     * Provides a user interface to allow the user to select an autonomous file
     * and translate it into time-based RobotC code. This will block the passed
     * {@link Window} from receiving input.
     *
     * @param owner the window to block
     *
     * @see Translate#to10Millis(itzfx.rerun.Translate.Segment)
     */
    public static void userTranslateToTime(Window owner) {
        FileUI.load("Autonomous", "*.rrn", owner, f -> {
            String text = translateTime(Command.decode(Retrieval.read(f))).collect(Collectors.joining("\n"));
            Alert show = new Alert(Alert.AlertType.CONFIRMATION, "", new ButtonType("Copy", ButtonBar.ButtonData.OK_DONE), ButtonType.CANCEL);
            show.setTitle("Translated code");
            show.setHeaderText("Code equivalent");
            ScrollPane s = new ScrollPane(new Label(text));
            s.setPrefViewportHeight(300);
            show.getDialogPane().setContent(s);
            CssUtils.styleDialog(show);
            show.getButtonTypes().get(0);
            show.showAndWait().filter(bt -> bt.getButtonData() == ButtonBar.ButtonData.OK_DONE)
                    .ifPresent(bt -> FXMLController.copy(text));
        });
    }

//    /**
//     * Provides a user interface to allow the user to select an autonomous file
//     * and translate it into distance-based RobotC code. This will block the
//     * passed {@link Window} from receiving input.
//     *
//     * @param owner the window to block
//     * @param r the robot from which to take the speed from
//     *
//     * @see Translate#toDistance(itzfx.rerun.Translate.Segment, itzfx.Robot)
//     */
//    public static void userTranslateToDistance(Window owner, Robot r) {
//        FileUI.load("Autonomous", "*.rrn", owner, f -> {
//            String text = (translateDistance(Command.decode(Retrieval.read(f)), r).stream().collect(Collectors.joining("\n")));
//            Alert show = new Alert(Alert.AlertType.CONFIRMATION, "", new ButtonType("Copy", ButtonBar.ButtonData.OK_DONE), ButtonType.CANCEL);
//            ScrollPane s = new ScrollPane(new Label(text));
//            s.setPrefViewportHeight(300);
//            show.getDialogPane().setContent(s);
//            CssUtils.styleDialog(show);
//            show.showAndWait().filter(bt -> bt.getButtonData() == ButtonBar.ButtonData.OK_DONE)
//                    .ifPresent(bt -> FXMLController.copy(text));
//        });
//    }
}
