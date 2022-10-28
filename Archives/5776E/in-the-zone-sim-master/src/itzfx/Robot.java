/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

import itzfx.data.FileUI;
import itzfx.fxml.game.objects.MobileGoal;
import itzfx.fxml.game.objects.StationaryGoal;
import itzfx.fxml.game.objects.RedMobileGoal;
import itzfx.fxml.game.objects.BlueMobileGoal;
import itzfx.fxml.game.objects.Cone;
import itzfx.fxml.Field;
import itzfx.rerun.Command;
import itzfx.rerun.Rerun;
import itzfx.scoring.ScoreReport;
import itzfx.scoring.ScoreType;
import itzfx.scoring.Scoreable;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Deque;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;
import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.binding.FloatBinding;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.FloatProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleFloatProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.css.PseudoClass;
import javafx.event.ActionEvent;
import javafx.geometry.Point2D;
import javafx.scene.Node;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.effect.DropShadow;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.util.Duration;

/**
 * This class is the monitor and controller of the on-field robot. It handles
 * movement, right-click, and relates those actions to the properties of the
 * robot. It also is capable of recording autonomous routines and re-running
 * them later.
 *
 * @author Prem Chintalapudi 5776E
 */
public final class Robot extends Mobile implements Scoreable {

    private static final PseudoClass RED = PseudoClass.getPseudoClass("red");
    private static final PseudoClass BLUE = PseudoClass.getPseudoClass("blue");

    private final StackPane node;
    private final StackPane realRobot;

    private RobotProperties properties;

    private final Hitbox hb;

    private final BooleanProperty active, driveBaseMovable;
    private boolean red;

    {
        active = new SimpleBooleanProperty(true);
        driveBaseMovable = new SimpleBooleanProperty(true);
    }

    private final ScoreReport sr;

    private final ImageView iv = new ImageView();

    {
        iv.setFitWidth(90);
        iv.setFitHeight(90);
        iv.setImage(new Image(Robot.class.getResourceAsStream("/itzfx/images/topviewicon.png")));
        iv.setEffect(new DropShadow(15, Color.BLACK));
        iv.setCache(true);
    }

    /*
    ============================================================================
    
    Construction
    
    ============================================================================
     */
    /**
     * Creates a robot at the specified coordinates with the specified initial
     * rotation. No layout properties (layoutX, layoutY) are adjusted, just the
     * translate properties of the node.
     *
     * @param layoutX the x coordinate to place the robot at
     * @param layoutY the y coordinate to place the robot at
     * @param initRotate the initial rotation of the robot
     */
    public Robot(float layoutX, float layoutY, float initRotate) {
        super(layoutX, layoutY, initRotate);
        Rectangle cover = new Rectangle(90, 90);
        cover.getStyleClass().add("filter");
        realRobot = new StackPane(iv, cover);
        realRobot.getStyleClass().add("robot");
        red = true;
        realRobot.pseudoClassStateChanged(RED, true);
        node = new StackPane(realRobot);
        node.setOnMouseDragged((MouseEvent m) -> super.setCenter((float) m.getSceneX() - 120, (float) m.getSceneY() - 120 - 45));
        iv.setRotate(90);
        hb = new Hitbox(45, Hitbox.CollisionType.STRONG, this, 18);
        hitboxing();
        sr = new ScoreReport(this);
        sr.setScoreType(ScoreType.ZONE_NONE);
        redMogo = new RedMobileGoal(25, 45);
        blueMogo = new BlueMobileGoal(25, 45);
        privateCone = new Cone(90, 45);
        node.getChildren().addAll(redMogo.getNode(), blueMogo.getNode(), privateCone.getNode());
        privateCone.permaDisableCollisions();
        privateCone.vanish();
        properties();
        mogoUndo();
        linkActions();
        setController(KeyControl.Defaults.SINGLE.getKC());
        properties = RobotProperties.getDefault();
        orps.update(properties);
    }

    private void register() {
        Field.getOwner(this).register(sr);
    }

    /**
     * Registers the mobile goals. This method should only be called once, after
     * the robot has been added to the field.
     */
    public void registerMogos() {
        Field.getOwner(this).register(redMogo.getReporter());
        Field.getOwner(this).register(blueMogo.getReporter());
        register();
    }

    /**
     * Determines whether this robot has the specified mobile goal. This is
     * generally invoked by the
     * {@link Field#getOwner(itzfx.fxml.game.objects.MobileGoal)} method to
     * check whether the mobile goal seeking an owner is in fact held by this
     * robot on a certain field.
     *
     * @param mogo the {@link MobileGoal} to test
     * @return true if this robot is holding the specified Mobile Goal
     */
    public boolean owner(MobileGoal mogo) {
        return mogo == redMogo || mogo == blueMogo;
    }

    private void mogoUndo() {
        redMogo.getNode().translateXProperty().unbind();
        blueMogo.getNode().translateXProperty().unbind();
        redMogo.permaDisableCollisions();
        blueMogo.permaDisableCollisions();
        redMogo.vanish();
        blueMogo.vanish();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected FloatBinding translateXBind() {
        return Bindings.createFloatBinding(() -> {
            Node n = realRobot;
            return super.centerXProperty().get() - ((float) n.getBoundsInLocal().getWidth() / 2 + (float) n.getBoundsInLocal().getMinX());
        }, super.centerXProperty());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected FloatBinding translateYBind() {
        return Bindings.createFloatBinding(() -> {
            Node n = realRobot;
            return super.centerYProperty().get() - ((float) n.getBoundsInLocal().getHeight() / 2 + (float) n.getBoundsInLocal().getMinY());
        }, super.centerYProperty());
    }

    /**
     *
     *
     * @see {@link Mobile#cleanUp}
     */
    @Override
    protected void cleanUp() {
        mogoAnimation.stop();
        stackAnimation.stop();
        if (heldMogo.get() != null) {
            heldMogo.get().setCenter(super.getCenterX() + (float) privateMogo.get().getNode().getTranslateX() * (float) Math.cos(Math.toRadians(node.getRotate())),
                    super.getCenterY() + (float) privateMogo.get().getNode().getTranslateX() * (float) Math.sin(Math.toRadians(node.getRotate())));
            heldMogo.get().reappear();
            heldMogo.set(null);
        }
        if (heldCone.get() != null) {
            heldCone.get().setCenter(super.getCenterX() + privateCone.getCenterX() * (float) Math.cos(Math.toRadians(node.getRotate())),
                    super.getCenterY() + privateCone.getCenterX() * (float) Math.sin(Math.toRadians(node.getRotate())));
            heldCone.get().reappear();
            heldCone.set(null);
        }
    }

    private void hitboxing() {
        hb.setXSupplier(super.centerXProperty()::get);
        hb.setYSupplier(super.centerYProperty()::get);
        node.getChildren().add(hb.getVisual());
        Hitbox.register(hb);
    }

    private void properties() {
        super.centerXProperty().addListener(Mobile.limitToField(45, centerXProperty()));
        super.centerYProperty().addListener(Mobile.limitToField(45, centerYProperty()));
        super.centerXProperty().addListener(super.exclude20(65));
        super.centerYProperty().addListener(super.exclude20(65));
        super.registerProperties();
    }

    private void switchImage() {
        FileUI.load(iv.getScene().getWindow(), f -> iv.setImage(new Image(f.toURI().toString(), 90, 90, true, true)), "Image", "*.jpg", "*.png");
    }

    private void linkActions() {
        actions.add(k -> forward());
        actions.add(k -> leftTurn());
        actions.add(k -> backward());
        actions.add(k -> rightTurn());
        actions.add(k -> mogo());
        actions.add(k -> autostack());
        actions.add(k -> cone());
        actions.add(k -> statStack());
        actions.add(k -> load());
    }

    /**
     * Adds "Enable Driver Control" and "Set Robot Image" options.
     *
     * @param rightClick {@inheritDoc}
     */
    @Override
    protected void rightClickOptions(ContextMenu rightClick) {
        MenuItem dc = new MenuItem("Enable Driver Control");
        dc.setOnAction(e -> {
            e.consume();
            driverControl();
            resume();
            KeyBuffer.unlock();
        });
        rightClick.getItems().add(dc);
        MenuItem si = new MenuItem("Set Robot Image");
        si.setOnAction(e -> {
            e.consume();
            switchImage();
        });
        rightClick.getItems().add(si);
    }

    /*
    ============================================================================
    
    Controller things
    
    ============================================================================
     */
    private final List<Consumer<KeyCode>> actions = new ArrayList<>(9);

    private KeyControl controller;

    /**
     * Gets the control format used by this Robot. A control format consists of
     * a set of {@link KeyCode KeyCodes} linked to specific actions. This robot
     * accepts a {@link KeyControl} as a valid control format.
     *
     * @return a KeyControl that represents this robot's control format
     */
    public KeyControl getController() {
        return controller;
    }

    /**
     * Sets the control format used by this Robot. A control format consists of
     * a set of {@link KeyCode KeyCodes} linked to specific actions. This robot
     * accepts a {@link KeyControl} as a valid control format.
     *
     * @param controller the KeyControl to set as a control format
     */
    public void setController(KeyControl controller) {
        if (controller != null) {
            Iterator<KeyCode> iteratorNew = Arrays.asList(controller.keys()).iterator();
            if (this.controller != null) {
                Iterator<KeyCode> iteratorOld = Arrays.asList(this.controller.keys()).iterator();
                actions.stream().peek(a -> KeyBuffer.remove(iteratorOld.next(), a)).forEach(a -> KeyBuffer.register(iteratorNew.next(), a));
            } else {
                actions.stream().forEach(a -> KeyBuffer.register(iteratorNew.next(), a));
            }
            this.controller = controller;
            okcs.setKeys(controller);
        }
    }

    /*
    ============================================================================
    
    Collisions
    
    ============================================================================
     */
    /**
     * {@inheritDoc}
     */
    @Override
    public void disableCollision() {
        hb.disable();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void enableCollision() {
        hb.enable();
    }

    /**
     * {@inheritDoc}
     *
     * @return true if the robot can collide
     */
    @Override
    public boolean canCollide() {
        return hb.canCollide();
    }

    /**
     *
     * {@inheritDoc}
     */
    @Override
    public void permaDisableCollisions() {
        Hitbox.unregister(hb);
    }

    /*
    ============================================================================
    
    Alliancing
    
    ============================================================================
     */
    /**
     * Sets this robot to the specified alliance.
     *
     * @param red is true if this robot is meant to be red, false if it is meant
     * to be blue
     */
    public void setRed(boolean red) {
        this.red = red;
        realRobot.pseudoClassStateChanged(RED, red);
        realRobot.pseudoClassStateChanged(BLUE, !red);
    }

    /**
     *
     *
     * @see {@link Scoreable#isRed}
     */
    @Override
    public boolean isRed() {
        return red;
    }

    /*
    ============================================================================
    
    Rerun/Replay recording
    
    ============================================================================
     */
    private boolean recording;
    private ScheduledFuture<?> recordingTask;

    private final Deque<List<Command>> saved = new ArrayDeque<>(1000);

    private List<Command> pulse = new ArrayList<>(2);

    /**
     * Every call, if this robot is recording a rerun, a new set of commands is
     * buffered to the rerun.
     */
    private void pulse() {
        if (pulse.isEmpty()) {
            pulse.add(Command.NONE);
        }
        saved.add(pulse);
        pulse = new ArrayList<>(2);
    }

    /**
     * Begins recording a rerun.
     */
    public void record() {
        if (!recording) {
            pulse.clear();
            saved.clear();
        }
        driverControl();
        recording = true;
        recordingTask = Start.PULSER.scheduleAtFixedRate(this::pulse, 0, 17, TimeUnit.MILLISECONDS);
    }

    /**
     * Determines if the robot is currently recording a rerun or not.
     *
     * @return true if the robot is currently recording
     */
    public boolean isRecording() {
        return recording;
    }

    /**
     * Stops recording the current rerun.
     */
    public void stopRecording() {
        recording = false;
        if (recordingTask != null) {
            recordingTask.cancel(false);
        }
    }

    /**
     * Gets a list of strings that can be saved in a file that encode the last
     * recorded rerun.
     *
     * @return a file-worthy list of strings that can be decoded later into a
     * robot-friendly rerun code
     */
    public List<String> saveRecording() {
        if (!saved.isEmpty()) {
            while (saved.peek().get(0) == Command.NONE) {
                saved.poll();
            }
        }
        if (!saved.isEmpty()) {
            while (saved.peekLast().get(0) == Command.NONE) {
                saved.pollLast();
            }
        }
        List<String> recorded = Command.encode(saved);
        saved.clear();
        return recorded;
    }

    /*
    ============================================================================
    
    Rerun/Replay read back
    
    ============================================================================
     */
    /**
     * Sets the autonomous program to run from a list of strings representing a
     * past rerun. This is generally pulled from a rerun file (*.rrn).
     *
     * @param commands the past rerun, represented as a list of encoded strings
     */
    public void setAuton(List<String> commands) {
        rerun = new Rerun(this, commands);
    }

    private Rerun rerun;

    public Rerun getRerun() {
        return rerun;
    }

    /**
     * Runs the saved autonomous routine.
     */
    public void runProgram() {
        if (rerun != null) {
            rerun.readBack();
        }
    }

    /*
    ============================================================================
    
    Robot movement methods
    
    ============================================================================
     */
    /**
     * Moves the robot forwards. This does take into account robot speed.
     */
    public void forward() {
        if (active.get() && driveBaseMovable.get()) {
            Platform.runLater(() -> {
                super.shiftCenter(properties.getRobotSpeed() * 5 / 36 * (float) Math.cos(Math.toRadians(node.getRotate())),
                        properties.getRobotSpeed() * 5 / 36 * (float) Math.sin(Math.toRadians(node.getRotate())));
            });
            handle10PointBar();
            if (isPrimed()) {
                Field.getOwner(this).play();
                deprime();
            }
            pulse.add(Command.FORWARD);
        }
    }

    /**
     * Moves the robot backwards. This does take into account robot speed.
     */
    public void backward() {
        if (active.get() && driveBaseMovable.get()) {
            Platform.runLater(() -> {
                super.shiftCenter(-properties.getRobotSpeed() * 5 / 36 * (float) Math.cos(Math.toRadians(node.getRotate())),
                        -properties.getRobotSpeed() * 5 / 36 * (float) Math.sin(Math.toRadians(node.getRotate())));
            });
            handle10PointBar();
            if (isPrimed()) {
                Field.getOwner(this).play();
                deprime();
            }
            pulse.add(Command.BACKWARD);
        }
    }

    private void handle10PointBar() {
        float dist = 1.5f;
        switch (barZone()) {
            case 1:
                Platform.runLater(() -> {
                    super.shiftCenter(-dist, dist);
                });
                break;
            case -1:
                Platform.runLater(() -> {
                    super.shiftCenter(dist, -dist);
                });
        }
    }

    private int barZone() {
        double yMx = super.getCenterY() - super.getCenterX();
        final int range = 30;
        if ((yMx > 480 - range && yMx < 480) || (-yMx > 480 && -yMx < 480 + range)) {
            return -1;
        } else if ((-yMx > 480 - range && -yMx < 480) || (yMx > 480 && yMx < 480 + range)) {
            return 1;
        }
        return 0;
    }

    /**
     * Turns the robot to the left. This does take into account robot speed.
     */
    public void leftTurn() {
        if (active.get() && driveBaseMovable.get()) {
            Platform.runLater(() -> {
                node.setRotate(node.getRotate() - properties.getRobotSpeed() / (Math.PI * 4.2));
            });
            if (isPrimed()) {
                Field.getOwner(this).play();
                deprime();
            }
            pulse.add(Command.LEFT_TURN);
        }
    }

    /**
     * Moves the robot to the right. This does take into account robot speed.
     */
    public void rightTurn() {
        if (active.get() && driveBaseMovable.get()) {
            Platform.runLater(() -> {
                node.setRotate(node.getRotate() + properties.getRobotSpeed() / (Math.PI * 4.2));
            });
            if (isPrimed()) {
                Field.getOwner(this).play();
                deprime();
            }
            pulse.add(Command.RIGHT_TURN);
        }
    }

    /*
    ============================================================================
    
    Mobile goal methods/animation
    
    ============================================================================
     */
    private final BooleanProperty movingMogo = new SimpleBooleanProperty();

    private final Timeline mogoAnimation = new Timeline();
    private final MobileGoal redMogo;
    private final MobileGoal blueMogo;

    private final ObjectProperty<MobileGoal> privateMogo = new SimpleObjectProperty<>();
    private final ObjectProperty<MobileGoal> heldMogo = new SimpleObjectProperty<>();

    /**
     * Toggles intake/outtake of mobile goal, and attempts to do so.
     */
    public void mogo() {
        if (active.get()) {
            if (!movingMogo.get()) {
                if (heldMogo.get() == null) {
                    Platform.runLater(() -> {
                        mogoIntake();
                    });
                } else {
                    Platform.runLater(() -> {
                        mogoOuttake();
                    });
                }
                if (isPrimed()) {
                    Field.getOwner(this).play();
                    deprime();
                }
                pulse.add(Command.MOGO);
            }
        }
    }

    private void mogoIntake() {
        MobileGoal mogo = Field.getOwner(this).huntMogo(new Point2D(super.getCenterX(), super.getCenterY()),
                new Point2D(70 * Math.cos(Math.toRadians(node.getRotate())) * (properties.isRobotMogoFront() > 0 ? 1 : -1),
                        70 * Math.sin(Math.toRadians(node.getRotate())) * (properties.isRobotMogoFront() > 0 ? 1 : -1)), isRed());
        if (mogo != null) {
            privateMogo.set(mogo instanceof RedMobileGoal ? redMogo : blueMogo);
            heldMogo.set(mogo);
            mogo.vanish();
            heldMogo.get().shiftStack(privateMogo.get());
            privateMogo.get().getNode().setTranslateX(properties.isRobotMogoFront() > 0 ? 70 : -70);
            privateMogo.get().reappear();
            movingMogo.set(true);
            mogoAnimation.stop();
            mogoAnimation.getKeyFrames().clear();
            mogoAnimation.getKeyFrames().add(new KeyFrame(Duration.seconds(this.properties.getRobotMogoIntakeTime()), this::finishMogoIntake,
                    new KeyValue(privateMogo.get().getNode().translateXProperty(), properties.isRobotMogoFront() > 0 ? 25 : -25)));
            mogoAnimation.play();
        }
    }

    private void finishMogoIntake(ActionEvent e) {
        e.consume();
        mogoAnimation.stop();
        movingMogo.set(false);
    }

    private void mogoOuttake() {
        movingMogo.set(true);
        mogoAnimation.stop();
        mogoAnimation.getKeyFrames().clear();
        mogoAnimation.getKeyFrames().add(new KeyFrame(Duration.seconds(this.properties.getRobotMogoIntakeTime()), this::finishMogoOuttake,
                new KeyValue(privateMogo.get().getNode().translateXProperty(), properties.isRobotMogoFront() > 0 ? 70 : -70)));
        mogoAnimation.play();
    }

    private void finishMogoOuttake(ActionEvent e) {
        e.consume();
        mogoAnimation.stop();
        privateMogo.get().vanish();
        privateMogo.get().shiftStack(heldMogo.get());
        heldMogo.get().setCenter(super.getCenterX() + 70 * (float) Math.cos(Math.toRadians(node.getRotate())) * (properties.isRobotMogoFront() > 0 ? 1 : -1),
                super.getCenterY() + 70 * (float) Math.sin(Math.toRadians(node.getRotate())) * (properties.isRobotMogoFront() > 0 ? 1 : -1));
        heldMogo.get().reappear();
        heldMogo.set(null);
        movingMogo.set(false);
    }

    /*
    ============================================================================
    
    Cone methods/animation
    
    ============================================================================
     */
    private final BooleanProperty movingCone = new SimpleBooleanProperty();

    private final Cone privateCone;
    private final ObjectProperty<Cone> heldCone = new SimpleObjectProperty<>();

    private long lastConeMove;

    /**
     * Toggles intake/outtake of a cone, and attempts to do so.
     */
    public void cone() {
        if (active.get()) {
            if (!movingCone.get() && System.currentTimeMillis() > 100 + lastConeMove) {
                lastConeMove = System.currentTimeMillis();
                if (heldCone.get() == null) {
                    Platform.runLater(() -> {
                        coneIntake();
                    });
                } else {
                    Platform.runLater(() -> {
                        coneOuttake();
                    });
                }
                if (isPrimed()) {
                    Field.getOwner(this).play();
                    deprime();
                }
                pulse.add(Command.CONE);
            }
        }
    }

    /**
     * @param cone the cone to intake
     * @deprecated only public for field reset
     */
    @Deprecated
    public void forceIntake(Cone cone) {
        heldCone.set(cone);
        cone.vanish();
        privateCone.setX(90);
        privateCone.reappear();
    }

    private void coneIntake() {
        Cone cone = Field.getOwner(this).huntCone(new Point2D(super.getCenterX(), super.getCenterY()),
                new Point2D(60 * Math.cos(Math.toRadians(node.getRotate())),
                        60 * Math.sin(Math.toRadians(node.getRotate()))));
        if (cone != null) {
            forceIntake(cone);
        }
    }

    private void coneOuttake() {
        privateCone.vanish();
        heldCone.get().setCenter(super.getCenterX() + 60 * (float) Math.cos(Math.toRadians(node.getRotate())),
                super.getCenterY() + 60 * (float) Math.sin(Math.toRadians(node.getRotate())));
        heldCone.get().reappear();
        heldCone.set(null);
    }

    private final Timeline stackAnimation = new Timeline();

    /**
     * Tries to intake a cone if none are held, and autostacks it if one is held
     * following the check.
     */
    public void autostack() {
        if (active.get() && heldMogo.get() != null && !movingCone.get() && privateMogo.get().score() / 2 < this.properties.getRobotMogoMaxStack()) {
            if (heldCone.get() == null) {
                Platform.runLater(() -> {
                    coneIntake();
                });
            }
            if (heldCone.get() != null) {
                Platform.runLater(() -> {
                    runAutostack();
                });
                if (isPrimed()) {
                    Field.getOwner(this).play();
                    deprime();
                }
            }
            pulse.add(Command.AUTOSTACK);
        }
    }

    private void runAutostack() {
        stackAnimation.stop();
        stackAnimation.getKeyFrames().clear();
        movingCone.set(true);
        stackAnimation.getKeyFrames().add(new KeyFrame(Duration.seconds(this.properties.getRobotAutostackTime()), this::finishAutostack,
                new KeyValue(privateCone.centerXProperty(), properties.isRobotMogoFront() > 0 ? 70 : 25)));
        stackAnimation.play();
    }

    private void finishAutostack(ActionEvent e) {
        e.consume();
        privateCone.vanish();
        stackAnimation.stop();
        movingCone.set(false);
        privateMogo.get().stack(heldCone.get());
        heldCone.set(null);
    }

    /**
     * Tries to stack a cone on a nearby stationary goal.
     */
    public void statStack() {
        if (active.get()) {
            if (!movingCone.get() && heldCone.get() != null) {
                Platform.runLater(() -> {
                    runStatStack();
                });
                if (isPrimed()) {
                    Field.getOwner(this).play();
                    deprime();
                }
            }
            pulse.add(Command.STATSTACK);
        }
    }

    private void runStatStack() {
        StationaryGoal sg = Field.getOwner(this).huntStat(new Point2D(super.getCenterX(), super.getCenterY()),
                new Point2D(57.5 * Math.cos(Math.toRadians(node.getRotate())),
                        57.5 * Math.sin(Math.toRadians(node.getRotate()))), isRed());
        if (sg != null && sg.score() / 2 < properties.getRobotStatMaxStack()) {
            Point2D sgCenter = new Point2D(sg.getNode().getTranslateX() + 12.5, sg.getNode().getTranslateY() + 12.5);
            heldCone.get().setCenter(super.getCenterX() + 60 * (float) Math.cos(Math.toRadians(node.getRotate())),
                    super.getCenterY() + 60 * (float) Math.sin(Math.toRadians(node.getRotate())));
            privateCone.vanish();
            heldCone.get().reappear();
            heldCone.get().disableCollision();
            driveBaseMovable.set(false);
            movingCone.set(true);
            stackAnimation.stop();
            stackAnimation.getKeyFrames().clear();
            stackAnimation.getKeyFrames().add(new KeyFrame(Duration.seconds(this.properties.getRobotStatTime()), e -> finishStat(e, sg),
                    new KeyValue(heldCone.get().centerXProperty(), sgCenter.getX()), new KeyValue(heldCone.get().centerYProperty(), sgCenter.getY())));
            stackAnimation.play();
        }
    }

    private void finishStat(ActionEvent e, StationaryGoal sg) {
        e.consume();
        movingCone.set(false);
        stackAnimation.stop();
        driveBaseMovable.set(true);
        sg.stack(heldCone.get());
        heldCone.get().vanish();
        heldCone.set(null);
    }

    /**
     * Tries to load a driver load cone onto this robot's alliance loader.
     */
    public void load() {
        Platform.runLater(() -> {
            Field.getOwner(this).load(this);
        });
        if (isPrimed()) {
            Field.getOwner(this).play();
            deprime();
        }
        pulse.add(Command.LOAD);
    }

    /*
    ============================================================================
    
    Robot priming (field starts timer only when a robot moves/is deprimed)
    
    ============================================================================
     */
    private final BooleanProperty primed = new SimpleBooleanProperty();

    /**
     * Lets the robot know to tell the field to start its timer when this robot
     * moves.
     */
    public void prime() {
        primed.set(true);
    }

    /**
     * Determines if this robot is primed to notify the field.
     *
     * @return true if this robot is primed
     */
    public boolean isPrimed() {
        return primed.get();
    }

    /**
     * Lets the robot know that it is unnecessary to notify the field when this
     * robot moves.
     */
    public void deprime() {
        primed.set(false);
    }

    /*
    ============================================================================
    
    Pause/resume methods (retains state of animations)
    
    ============================================================================
     */
    private boolean mogoWas;
    private boolean stackWas;
    private boolean rerunWas;

    /**
     * Pauses the movement of this robot, including stack and mobile goal
     * animations.
     */
    public void pause() {
        if (rerun != null && !rerun.isDone()) {
            rerun.pause();
            rerunWas = true;
        }
        if (mogoAnimation.getStatus() == Animation.Status.RUNNING) {
            mogoAnimation.pause();
            mogoWas = true;
        }
        if (stackAnimation.getStatus() == Animation.Status.RUNNING) {
            stackAnimation.pause();
            stackWas = true;
        }
        active.set(false);
    }

    /**
     * Resumes movement of this robot, including stack and mobile goal
     * animations.
     */
    public void resume() {
        if (rerunWas) {
            rerun.readBack();
        }
        if (mogoWas) {
            mogoAnimation.play();
            mogoWas = false;
        }
        if (stackWas) {
            stackAnimation.play();
            stackWas = false;
        }
        active.set(true);
    }

    /**
     * This method always returns 0 for a robot. However, it does force the
     * robot to update whether it is parked or not. {@inheritDoc}
     *
     * @return 0
     */
    @Override
    public int score() {
        sr.setScoreType(inParkOne() || inParkTwo() ? ScoreType.PARKING : ScoreType.ZONE_NONE);
        return 0;
    }

    private boolean inParkOne() {
        return isRed() ? super.getCenterX() < 165 && super.getCenterY() > 75 && super.getCenterY() < 285
                : super.getCenterY() < 165 && super.getCenterX() > 75 && super.getCenterX() < 285;
    }

    private boolean inParkTwo() {
        return isRed() ? super.getCenterY() > 555 && super.getCenterX() > 425 && super.getCenterX() < 645
                : super.getCenterX() > 555 && super.getCenterY() > 425 && super.getCenterY() < 645;
    }

    /**
     * Sets the values of this robot to the specified values. This is meant for
     * quickly updating this robot after a robot build session. If any values
     * are null, the previous values for those quantities are used.
     *
     *
     * @param rp
     */
    public void acceptValues(RobotProperties rp) {
        RobotProperties temp = properties;
        properties = RobotProperties.getFilledVersion(rp, properties);
        if (properties.isRobotMogoFront() != temp.isRobotMogoFront()) {
            adjustInitRotate(180);
        }
        orps.update(properties);
    }

    /**
     * Gets a string that can be decoded later for the purposes of file saving.
     *
     * @return a string with all the data of this robot
     */
    public String fileData() {
        return properties.fileData();
    }

    /**
     * Fills the robot with the given data, encoded in a string originally
     * created by the {@link Robot#fileData()} method.
     *
     * @param r the robot to set the values of
     * @param fileData the encoded string
     */
    public static void fillRobot(Robot r, String fileData) {
        String[] values = fileData.split(" ");
        float rs = Float.parseFloat(values[0]);
        float rmit = Float.parseFloat(values[1]);
        float rat = Float.parseFloat(values[2]);
        float rst = Float.parseFloat(values[3]);
        int rmms = Integer.parseInt(values[4]);
        int rsms = Integer.parseInt(values[5]);
        boolean rmf = Boolean.parseBoolean(values[6]);
        r.acceptValues(RobotProperties.create(rs, rmit, rat, rst, rmms, rsms, rmf ? 1 : -1));
    }

    /*
    ============================================================================
    
    Making controller observable (for tabs)
    
    ============================================================================
     */
    //Link to controller table.
    private final ObservableKeyCodeStruct okcs = new ObservableKeyCodeStruct();

    private class ObservableKeyCodeStruct {

        private final ObjectProperty<KeyCode>[] keys = new ObjectProperty[9];

        public ObservableKeyCodeStruct() {
            for (int i = 0; i < keys.length; i++) {
                keys[i] = new SimpleObjectProperty<>();
            }
        }

        public void setKeys(KeyControl kc) {
            for (int i = 0; i < keys.length; i++) {
                keys[i].set(kc.keys()[i]);
            }
        }

        public ObjectProperty<KeyCode>[] keys() {
            return keys;
        }
    }

    public ObjectProperty<KeyCode> forwardKeyProperty() {
        return okcs.keys()[0];
    }

    public ObjectProperty<KeyCode> leftKeyProperty() {
        return okcs.keys()[1];
    }

    public ObjectProperty<KeyCode> backKeyProperty() {
        return okcs.keys()[2];
    }

    public ObjectProperty<KeyCode> rightKeyProperty() {
        return okcs.keys()[3];
    }

    public ObjectProperty<KeyCode> mobileGoalKeyProperty() {
        return okcs.keys()[4];
    }

    public ObjectProperty<KeyCode> autostackKeyProperty() {
        return okcs.keys()[5];
    }

    public ObjectProperty<KeyCode> coneKeyProperty() {
        return okcs.keys()[6];
    }

    public ObjectProperty<KeyCode> statKeyProperty() {
        return okcs.keys()[7];
    }

    public ObjectProperty<KeyCode> loadKeyProperty() {
        return okcs.keys()[8];
    }

    /*
    ============================================================================
    
    Making robot properties observable (for tabs)
    
    ============================================================================
     */
    private final ObservableRobotPropertiesStruct orps = new ObservableRobotPropertiesStruct();

    private class ObservableRobotPropertiesStruct {

        private final FloatProperty speed = new SimpleFloatProperty();
        private final FloatProperty mogointaketime = new SimpleFloatProperty();
        private final FloatProperty statstacktime = new SimpleFloatProperty();
        private final FloatProperty autostacktime = new SimpleFloatProperty();
        private final IntegerProperty maxmogostack = new SimpleIntegerProperty();
        private final IntegerProperty maxstatstack = new SimpleIntegerProperty();
        private final BooleanProperty mogofront = new SimpleBooleanProperty();

        /**
         * @return the speed
         */
        public FloatProperty getSpeed() {
            return speed;
        }

        /**
         * @return the mogointaketime
         */
        public FloatProperty getMogointaketime() {
            return mogointaketime;
        }

        /**
         * @return the statstacktime
         */
        public FloatProperty getStatstacktime() {
            return statstacktime;
        }

        /**
         * @return the autostacktime
         */
        public FloatProperty getAutostacktime() {
            return autostacktime;
        }

        /**
         * @return the maxmogostack
         */
        public IntegerProperty getMaxmogostack() {
            return maxmogostack;
        }

        /**
         * @return the maxstatstack
         */
        public IntegerProperty getMaxstatstack() {
            return maxstatstack;
        }

        /**
         * @return the mogofront
         */
        public BooleanProperty getMogofront() {
            return mogofront;
        }

        public void update(RobotProperties rp) {
            speed.set(rp.getRobotSpeed());
            mogointaketime.set(rp.getRobotMogoIntakeTime());
            statstacktime.set(rp.getRobotStatTime());
            autostacktime.set(rp.getRobotAutostackTime());
            maxmogostack.set(rp.getRobotMogoMaxStack());
            maxstatstack.set(rp.getRobotStatMaxStack());
            mogofront.set(rp.isRobotMogoFront() > 0);
        }
    }

    public FloatProperty speedProperty() {
        return orps.getSpeed();
    }

    public FloatProperty mogoIntakeTimeProperty() {
        return orps.getMogointaketime();
    }

    public FloatProperty autostackTimeProperty() {
        return orps.getAutostacktime();
    }

    public FloatProperty statStackTimeProperty() {
        return orps.getStatstacktime();
    }

    public IntegerProperty maxMogoStackProperty() {
        return orps.getMaxmogostack();
    }

    public IntegerProperty maxStatStackProperty() {
        return orps.getMaxstatstack();
    }

    public BooleanProperty mogoFrontProperty() {
        return orps.getMogofront();
    }

    /*
    ============================================================================
    
    Utility methods
    
    ============================================================================
     */
    /**
     * {@inheritDoc}
     */
    @Override
    public void resetProperties() {
        mogoWas = false;
        stackWas = false;
        rerunWas = false;
        mogoAnimation.stop();
        stackAnimation.stop();
        if (heldMogo.get() != null) {
            privateMogo.get().shiftStack(heldMogo.get());
            heldMogo.get().reset();
            heldMogo.set(null);
        }
        if (heldCone.get() != null) {
            heldCone.get().reset();
            heldCone.set(null);
        }
        privateCone.vanish();
        redMogo.vanish();
        blueMogo.vanish();
        movingMogo.set(false);
        movingCone.set(false);
        if (rerun != null) {
            rerun.stop();
            rerun.reset();
        }
        setController(controller);
        active.set(true);
        if (heldCone.get() != null) {
            Cone c = heldCone.get();
            heldCone.set(null);
            forceIntake(c);
        }
    }

    /**
     * Enables driver control of the robot.
     */
    public void driverControl() {
        if (rerun != null) {
            rerun.stop();
        }
        setController(controller);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public StackPane getNode() {
        return node;
    }
}
