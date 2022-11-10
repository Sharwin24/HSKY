/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

import com.sun.javafx.application.LauncherImpl;
import itzfx.fxml.FXMLController;
import itzfx.preload.Prestart;
import itzfx.rerun.Translate;
import itzfx.utils.CssUtils;
import java.io.IOException;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.LockSupport;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.beans.property.DoubleProperty;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;
import javafx.util.Duration;

/**
 *
 * @author Prem Chintalapudi 5776E
 */
public class Start extends Application {

    private FXMLController fxml;

    public Start() {
        instance = this;
    }

    //Because I can
//    public static final Unsafe UNSAFE;
//
//    static {
//        try {
//            java.lang.reflect.Field f = Unsafe.class.getDeclaredField("theUnsafe");
//            f.setAccessible(true);
//            UNSAFE = (Unsafe) f.get(null);
//        } catch (NoSuchFieldException | SecurityException | IllegalArgumentException | IllegalAccessException ex) {
//            Logger.getLogger(Start.class.getName()).log(Level.SEVERE, null, ex);
//            System.err.println(ex);
//            throw new RuntimeException();
//        }
//    }
    private Parent p;

    /**
     * {@inheritDoc}
     */
    @Override
    public void init() {
        try {
            FXMLLoader loader = new FXMLLoader(Start.class.getResource("fxml/FXML.fxml"));
            p = loader.load();
            p.setOnMousePressed(m -> p.requestFocus());
//            addZoomListeners(p);
            fxml = loader.getController();
            long start = System.currentTimeMillis();
            do {
                LockSupport.parkUntil(this, 3500 + start - System.currentTimeMillis());
            } while (System.currentTimeMillis() < 3500 + start);
        } catch (IOException ex) {
            Logger.getLogger(Start.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    private static void addZoomListeners(Node n) {
        final float minZoomSoft, minZoomHard;
        final float maxZoomSoft, maxZoomHard;
        minZoomHard = 0.5f;
        minZoomSoft = 0.71f;
        maxZoomSoft = 1.41f;
        maxZoomHard = 2;
        n.scaleYProperty().bindBidirectional(n.scaleXProperty());
        n.setOnZoom(ze -> {
            if (n.getScaleX() < maxZoomHard && n.getScaleX() > minZoomHard) {
                n.setScaleX(ze.getZoomFactor() * n.getScaleX());
            }
        });
        n.setOnZoomFinished(ze -> {
            if (n.getScaleX() < minZoomSoft) {
                restoreZoom(minZoomSoft, n.scaleXProperty());
            }
            if (n.getScaleX() > maxZoomSoft) {
                restoreZoom(maxZoomSoft, n.scaleXProperty());
            }
        });
        addDoubleTapToRestoreZoom(n);
        cancelSingleFingerScroll(n);
    }

    private static void restoreZoom(float restorationValue, DoubleProperty scale) {
        Timeline tl = new Timeline();
        tl.getKeyFrames().add(new KeyFrame(Duration.millis(200), e -> tl.stop(), new KeyValue(scale, restorationValue)));
        tl.play();
    }

    private static void cancelSingleFingerScroll(Node n) {
        n.setOnScroll(se -> {
            if (se.getTouchCount() == 1) {
                se.consume();
            }
        });
    }

    private static void addDoubleTapToRestoreZoom(Node n) {
        AtomicInteger touchCount = new AtomicInteger();
        AtomicBoolean held = new AtomicBoolean();
        n.setOnTouchPressed(te -> {
            held.set(true);
        });
        n.setOnTouchReleased(te -> {
            if (held.getAndSet(false)) {
                if (touchCount.incrementAndGet() == 2) {
                    restoreZoom(1, n.scaleXProperty());
                }
                PULSER.schedule(touchCount::decrementAndGet, 150, TimeUnit.MILLISECONDS);
            }
            te.consume();
        });
    }

    private boolean dark = true;

    /**
     * {@inheritDoc}
     */
    @Override
    public void start(Stage primaryStage) {
        primaryStage.setTitle("In The Zone (ITZ)");
        primaryStage.getIcons().add(new Image(Start.class.getResourceAsStream("images/icon.png")));
        final float width = 1600, height = 900;
        AnchorPane.setLeftAnchor(p, 0d);
        AnchorPane.setTopAnchor(p, 0d);
        AnchorPane.setRightAnchor(p, 0d);
        AnchorPane.setBottomAnchor(p, 0d);
        AnchorPane windowScale = new AnchorPane(p);
        Scene scene = new Scene(windowScale, width, height);
        CssUtils.addStyleSheet(scene);
        scene.addEventHandler(KeyEvent.KEY_PRESSED, k -> {
            if (k.isControlDown() && k.isShiftDown()) {
                switch (k.getCode()) {
                    case I:
                        CssUtils.switchStyleSheet((dark = !dark) ? "default" : "dark");
                        break;
                    case T:
                        Translate.userTranslateToTime(primaryStage);
                        break;
                }
            }
        });
        addRotateListeners(scene);
        KeyBuffer.initialize(scene);
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    public void restart(Stage primaryStage) {
        start(primaryStage);
        Start.PULSER.schedule(() -> Platform.runLater(fxml.getFieldController()::reset), 1250, TimeUnit.MILLISECONDS);
    }

    private void addRotateListeners(Scene scene) {
        scene.addEventHandler(KeyEvent.KEY_PRESSED, k -> {
            if (k.isAltDown() && k.isShiftDown() && k.getCode() != null) {
                switch (k.getCode()) {
                    case RIGHT:
                        fxml.setRotate(90);
                        k.consume();
                        break;
                    case LEFT:
                        fxml.setRotate(-90);
                        k.consume();
                        break;
                    case UP:
                        fxml.setRotate(0);
                        k.consume();
                        break;
                    case DOWN:
                        fxml.setRotate(180);
                        k.consume();
                        break;
                    default:
                        break;
                }
            }
        });
    }

    /**
     * Program start point.
     *
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        LauncherImpl.launchApplication(Start.class, Prestart.class, args);
    }

    /**
     * Signal boolean to check if this program has shut down or not.
     */
    public static boolean SHUTDOWN;

    /**
     * Main thread pool for scheduling tasks.
     */
    public static final ScheduledExecutorService PULSER = Executors.newSingleThreadScheduledExecutor();

    public static void debug() {
        System.out.println("*************************************************");
        StackTraceElement[] array = Thread.currentThread().getStackTrace();
        System.out.println("Class called: " + array[2].getClassName());
        System.out.println("Method called: " + array[2].getMethodName());
        System.out.println("Calling class: " + array[3].getClassName());
        System.out.println("Calling method: " + array[3].getMethodName());
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void stop() {
        PULSER.shutdownNow();
        SHUTDOWN = true;
        fxml.close();
    }

    private static Start instance;

    public static void navigate(String url) {
        instance.getHostServices().showDocument(url);
    }
}
