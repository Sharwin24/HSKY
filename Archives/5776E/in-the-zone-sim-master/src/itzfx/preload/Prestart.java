/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.preload;

import itzfx.Start;
import itzfx.utils.CssUtils;
import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.application.Preloader;
import javafx.application.Preloader.ProgressNotification;
import javafx.application.Preloader.StateChangeNotification;
import javafx.beans.binding.Bindings;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.stage.Stage;
import javafx.util.Duration;

/**
 * FXML Controller Class. Controls "Prestart.fxml". Used to customize UX and
 * hide the fact that a reset must be done 3 seconds in to ensure that start-up
 * collisions don't mess with initial position.
 *
 * @author Prem Chintalapudi 5776E
 */
public class Prestart extends Preloader {

    @FXML
    private ProgressBar bar;
    private Stage stage;

    @FXML
    private Label loading;

    /**
     * {@inheritDoc}
     */
    @Override
    public void start(Stage stage) {
        this.stage = stage;
        stage.setTitle("In The Zone - Starting...");
        try {
            FXMLLoader loader = new FXMLLoader(Prestart.class.getResource("Prestart.fxml"));
            loader.setController(this);
            Parent p = loader.load();
            Scene s = new Scene(p);
            CssUtils.addStyleSheet(s);
            stage.setScene(s);
        } catch (IOException ex) {
            Logger.getLogger(Prestart.class.getName()).log(Level.SEVERE, null, ex);
        }
        tl = new Timeline();
        tl.getKeyFrames().add(new KeyFrame(Duration.millis(4500), e -> stopTimelines(), new KeyValue(bar.progressProperty(), 1)));
        tl.play();
        StringProperty sp = new SimpleStringProperty(getNextFalseFile());
        loading.textProperty().bind(Bindings.createStringBinding(() -> "Loading " + sp.get() + ".jfx...", sp));
        future = Start.PULSER.scheduleAtFixedRate(() -> Platform.runLater(() -> sp.set(getNextFalseFile())), 2000, 85, TimeUnit.MILLISECONDS);
        stage.setOnHidden(w -> {
            if (suddenClose) {
                System.exit(0);
            }
        });
        stage.show();
    }

    private Timeline tl;
    private Future<?> future;

    private void stopTimelines() {
        tl.stop();
        future.cancel(true);
    }

    private static final List<String> FALSE_FILES = Arrays.asList("Core-files",
            "JavaFX 8.0", "sun.misc.Unsafe", "Netbeans IDE", "Eclipse", "5776E",
            "DVHS Robotics", "College Essays", "Lol you saw this", "Give me the monitor",
            "Got into MIT", "\"The physics\"", "Robots are 21 pounds", "Broken cortexes",
            "V5", "Rerun works!!", "Some random code", "You can't see me", "Why are you even reading this?",
            "Just get on with life", "'; DROP TABLE bots;", "Waitlisted John Hopkin",
            "UCSD Yield Protected", "Learn to code boi", "Sleepy people are watching",
            "USABO Finalist", "chinpreme.js", "I should be working on my essay",
            "This list is too long");

    private static Iterator<String> ffIterator = FALSE_FILES.iterator();

    private static String getNextFalseFile() {
        if (!ffIterator.hasNext()) {
            ffIterator = FALSE_FILES.iterator();
        }
        return ffIterator.next();
    }

    private boolean suddenClose = true;

    /**
     * {@inheritDoc}
     */
    @Override
    public void handleStateChangeNotification(StateChangeNotification scn) {
        if (scn.getType() == StateChangeNotification.Type.BEFORE_START) {
            suddenClose = false;
            stage.hide();
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void handleProgressNotification(ProgressNotification pn) {
        //Nothing really happens
    }
    
    public void close() {
        suddenClose = false;
        stage.close();
    }
}
