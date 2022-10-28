/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.build;

import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.layout.Pane;

/**
 * FXML Controller class. Controls file "MogoConfig.fxml". Creates and maintains
 * image/options in "Mobile Goal Configuration" tab of the robot builder.
 *
 * @author Prem Chintalapudi 5776E
 */
public class MogoConfig {

    @FXML
    private Pane root;

    @FXML
    private CheckBox front;

    @FXML
    private TextField robotInTime;

    @FXML
    private void initialize() {
        robotInTime.setTextFormatter(new DecimalFormatter());
        root.setUserData(this);
    }

    /**
     * Gets the entered time for the mobile goal intake time. If the user has
     * not entered anything, this returns null.
     *
     * @return the value the user entered, or null
     */
    public float getMogoTime() {
        if (robotInTime.getText().isEmpty()) {
            return Float.MIN_VALUE;
        } else {
            return Float.parseFloat(robotInTime.getText());
        }
    }

    /**
     * Gets whether or not the user decided if their robot would intake from the
     * front or the back.
     *
     * @return true if this is a front-intaking robot, false if not, or null if
     * this is indeterminate
     */
    public int isFrontMogo() {
        return front.isIndeterminate() ? Integer.MIN_VALUE : front.isSelected() ? 1 : -1;
    }

    private static class DecimalFormatter extends TextFormatter<Double> {

        public DecimalFormatter() {
            super(c -> {
                c.setText(c.getText().replaceAll("[^0-9.]", ""));
                return c;
            });
        }
    }
}
