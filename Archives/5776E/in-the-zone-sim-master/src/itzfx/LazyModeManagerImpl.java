/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

import itzfx.media.Sounds;
import itzfx.media.SoundHandler;
import java.util.EnumSet;
import java.util.concurrent.TimeUnit;
import java.util.function.BiConsumer;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.util.Duration;

/**
 *
 * @author prem
 */
public class LazyModeManagerImpl implements LazyModeManager {

    private final SoundHandler soundHandler = new SoundHandler();

    private boolean shutUp;

    public LazyModeManagerImpl() {
        Timeline runDownClock = new Timeline();
        soundHandler.mute();
        EnumSet.allOf(Sounds.class).forEach(soundHandler::play);
        Start.PULSER.schedule(soundHandler::unmute, 5, TimeUnit.SECONDS);
        onGameStateChanged((b, s) -> {
            switch (s) {
                case WAITING:
                    runDownClock.stop();
                    runDownClock.getKeyFrames().setAll(new KeyFrame(Duration.millis(getTimeRemaining() * 100), e -> finished(), new KeyValue(timeRemainingProperty, 0)));
                    break;
                case PLAYING:
                    if (getControlMode() != ControlMode.FREE_PLAY) {
                        soundHandler.play(Sounds.START);
                    }
                    if (timerActive(getControlMode())) {
                        runDownClock.play();
                    }
                    break;
                case PAUSED:
                    soundHandler.play(Sounds.PAUSED);
                    runDownClock.pause();
                    break;
                case STOPPED:
                    runDownClock.stop();
                    if (switchOver) {
                        soundHandler.play(Sounds.PAUSED);
                        Start.PULSER.schedule(() -> Platform.runLater(() -> {
                            setControlMode(ControlMode.DRIVER_CONTROL);
                            runDownClock.getKeyFrames().add(new KeyFrame(Duration.seconds(75), e -> soundHandler.play(Sounds.WARNING)));
                        }), 5, TimeUnit.SECONDS);
                    } else {
                        if (shutUp) {
                            //I will not be silenced more
                            shutUp = false;
                        } else {
                            soundHandler.play(Sounds.END);
                        }
                    }
                    break;
                default:
                    break;
            }
        });
        onControlModeChange((b, s) -> {
            reset();
        });
    }

    private final IntegerProperty timeRemainingProperty = new SimpleIntegerProperty();
    private final IntegerProperty startingInProperty = new SimpleIntegerProperty(5);

    @Override
    public IntegerProperty timeRemainingProperty() {
        return timeRemainingProperty;
    }

    @Override
    public IntegerProperty startingInProperty() {
        return startingInProperty;
    }

    private final ObjectProperty<ControlMode> modeProperty = new SimpleObjectProperty<>();

    @Override
    public ControlMode getControlMode() {
        return modeProperty.get();
    }

    @Override
    public void setControlMode(ControlMode mode) {
        modeProperty.set(mode);
    }

    @Override
    public final void onControlModeChange(BiConsumer<ControlMode, ControlMode> action) {
        modeProperty.addListener((o, b, s) -> action.accept(b, s));
    }

    private final ObjectProperty<GameState> gameStateProperty = new SimpleObjectProperty<>(GameState.STOPPED);

    private boolean switchOver;

    @Override
    public void startMatch() {
        setControlMode(ControlMode.AUTON);
        switchOver = true;
    }

    @Override
    public void stopMatch() {
        switchOver = false;
        setGameState(GameState.STOPPED);
    }

    @Override
    public void play() {
        setGameState(GameState.PLAYING);
    }

    @Override
    public void pause() {
        setGameState(GameState.PAUSED);
    }

    private void finished() {
        setGameState(GameState.STOPPED);
    }

    private void setGameState(GameState gameState) {
        this.gameStateProperty.set(gameState);
    }

    @Override
    public final void onGameStateChanged(BiConsumer<GameState, GameState> onChanged) {
        this.gameStateProperty.addListener((o, b, s) -> onChanged.accept(b, s));
    }

    private final Timeline loadingTimeline = new Timeline();

    private void beginLoading() {
        cancelled = true;
        stopLoading();
        startingInProperty.set(2);
        loadingTimeline.getKeyFrames().setAll(new KeyFrame(Duration.seconds(3), e -> stopLoading(), new KeyValue(startingInProperty, -1)));
        loadingTimeline.play();
    }

    private boolean cancelled;

    private void stopLoading() {
        loadingTimeline.stop();
        if (!cancelled) {
            play();
        }
        cancelled = false;
    }

    @Override
    public void reset() {
        cancelled = true;
        shutUp = true;
        stopLoading();
        stopMatch();
        timeRemainingProperty.set(timeRemainingHashTable(getControlMode()));
        if (isAutostart(getControlMode())) {
            beginLoading();
        }
        setGameState(GameState.WAITING);
    }

    private boolean muted;

    @Override
    public void mute() {
        if (muted) {
            soundHandler.mute();
        } else {
            soundHandler.unmute();
        }
    }

    private static boolean isAutostart(ControlMode mode) {
        return mode == ControlMode.AUTON || mode == ControlMode.PROGRAMMING_SKILLS;
    }

    private static boolean timerActive(ControlMode mode) {
        switch (mode) {
            case AUTON:
            case DRIVER_CONTROL:
            case PROGRAMMING_SKILLS:
            case DRIVER_SKILLS:
                return true;
            case FREE_PLAY:
            default:
                return false;
        }
    }

    //Returns decisecond time
    private static int timeRemainingHashTable(ControlMode mode) {
        switch (mode) {
            case FREE_PLAY:
                return 8888;
            case AUTON:
                return 150;
            case DRIVER_CONTROL:
                return 1050;
            case PROGRAMMING_SKILLS:
            case DRIVER_SKILLS:
                return 600;
            default:
                return -1;
        }
    }
}
