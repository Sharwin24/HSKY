/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.build;

import javafx.fxml.FXML;
import javafx.scene.control.TextField;
import javafx.scene.control.TextFormatter;
import javafx.scene.layout.Pane;

/**
 * FXML Controller class. Controls "StackConfig.fxml". Creates and maintains
 * image/options in the "Stack Configuration" tab of the robot builder.
 *
 * @author Prem Chintalapudi 5776E
 */
public class StackConfig {

    @FXML
    private Pane root;

    @FXML
    private TextField mogoStack;

    @FXML
    private TextField statStack;

    @FXML
    private void initialize() {
        mogoStack.setTextFormatter(new IntFormatter());
        statStack.setTextFormatter(new IntFormatter());
        root.setUserData(this);
    }

    /**
     * Gets the entered maximum mobile goal stack of the robot. This value will
     * be null if the user did not enter anything.
     *
     * @return the user-entered value, or null
     */
    public int getMogoStack() {
        if (mogoStack.getText().isEmpty()) {
            return Integer.MIN_VALUE;
        } else {
            return Integer.parseInt(mogoStack.getText());
        }
    }

    /**
     * Gets the entered maximum stationary goal stack of the robot. This value
     * will be null if the user did not enter anything.
     *
     * @return the user-entered value, or null
     */
    public int getStatStack() {
        if (statStack.getText().isEmpty()) {
            return Integer.MIN_VALUE;
        } else {
            return Integer.parseInt(statStack.getText());
        }
    }

    private static class IntFormatter extends TextFormatter<Integer> {

        public IntFormatter() {
            super(c -> {
                c.setText(c.getText().replaceAll("[^0-9]", ""));
                return c;
            });
        }
    }
}
