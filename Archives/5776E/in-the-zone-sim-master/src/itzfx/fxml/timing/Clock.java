/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.timing;

import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.value.ObservableValue;
import javafx.fxml.FXML;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.shape.Circle;

/**
 * FXML Controller class. Controls "Clock.fxml". Basically a low-overhead
 * digitized field timer. This clock has a granularity of at maximum 0.1
 * seconds, so any more specific times will not be observed.
 *
 * @author Prem Chintalapudi 5776E
 */
public class Clock {

    @FXML
    private AnchorPane root;

    private Digit min;
    private Digit secD;
    private Digit sec;
    private Digit secd;

    @FXML
    private AnchorPane minPane;
    @FXML
    private AnchorPane secDPane;
    @FXML
    private AnchorPane secPane;
    @FXML
    private AnchorPane secdPane;

    @FXML
    private VBox colon;

    @FXML
    private Circle period;

    private final IntegerProperty time = new SimpleIntegerProperty();

    @FXML
    private void initialize() {
        root.setUserData(this);
        min = (Digit) minPane.getUserData();
        secD = (Digit) secDPane.getUserData();
        sec = (Digit) secPane.getUserData();
        secd = (Digit) secdPane.getUserData();
        time.addListener(this::display);
        time.set(0);
    }

    private void display(ObservableValue<? extends Number> obs, Number old, Number next) {
        if (next.intValue() != 8888) {
            int minutes = next.intValue() / 600;
            int decaSeconds = next.intValue() % 600 / 100;
            int secs = next.intValue() % 100 / 10;
            int deciSeconds = next.intValue() % 10;
            min.display(minutes);
            secD.display(decaSeconds);
            sec.display(secs);
            secd.display(deciSeconds);
        } else {
            min.display(0);
            secD.display(0);
            sec.display(0);
            secd.display(0);
        }
    }

    /**
     * Gets the property that this clock observes to display the time. This
     * property is measured in seconds.
     *
     * @return the property observed by this clock
     */
    public IntegerProperty getTime() {
        return time;
    }
}
