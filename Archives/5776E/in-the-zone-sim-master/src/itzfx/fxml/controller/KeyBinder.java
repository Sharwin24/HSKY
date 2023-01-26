/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.controller;

import itzfx.KeyControl;
import java.util.Arrays;
import javafx.fxml.FXML;
import javafx.scene.control.ListView;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;

/**
 * FXML Controller class. Controls the "KeyBinder.fxml" file. This is the main
 * class for creating a new controller format. This class should be instantiated
 * prior to loading the controlled FXML file. Keys may be selected through key
 * presses OR through mouse clicks.
 *
 * @author Prem Chintalapudi 5776E
 */
public class KeyBinder {

    @FXML
    private ListView<String> left;
    @FXML
    private AnchorPane center;

    private Keyboard keyboard;

    private final KeyCode[] keys;

    /**
     * Constructs a new KeyBinder with the given key set.
     *
     * @param kc the original set of keys
     */
    public KeyBinder(KeyControl kc) {
        keys = kc.keys();
    }

    @FXML
    private void initialize() {
        this.keyboard = (Keyboard) center.getUserData();
        disableHotkeys();
        left.getItems().addAll("Forward  " + keys[0], "Left Turn  " + keys[1], "Backward  " + keys[2], "Right Turn  " + keys[3]);
        left.getItems().addAll("Mobile Goal Intake/Outtake  " + keys[4], "Autostack on Mobile Goal  " + keys[5],
                "Intake/Outtake a Cone  " + keys[6], "Stack on a Stationary Goal  " + keys[7], "Load a Driver Load  " + keys[8]);
        Arrays.stream(keys).peek(keyboard::selected).forEach(k -> keyboard.save());
        left.addEventFilter(KeyEvent.KEY_PRESSED, k -> {
            int index = left.getSelectionModel().getSelectedIndex();
            if (index != -1) {
                String replace = left.getItems().get(index).split("  ")[0];
                for (int i = 0; i < keys.length; i++) {
                    if (i != index && keys[i].equals(k.getCode())) {
                        return;
                    }
                }
                keyboard.remove(keys[index]);
                keys[index] = k.getCode();
                left.getItems().set(index, replace + "  " + k.getCode());
                keyboard.selected(k.getCode());
            }
        });
        left.addEventFilter(MouseEvent.MOUSE_RELEASED, m -> {
            keyboard.save();
            keyboard.selected(keys[left.getSelectionModel().getSelectedIndex()]);
        });
        center.setOnMousePressed(m -> {
            keyboard.save();
            keyboard.deselect();
            left.getSelectionModel().clearSelection();
        });
    }
    
    private void disableHotkeys() {
        keyboard.disable(KeyCode.DIGIT0);
        keyboard.disable(KeyCode.DIGIT1);
        keyboard.disable(KeyCode.DIGIT2);
        keyboard.disable(KeyCode.DIGIT3);
        keyboard.disable(KeyCode.DIGIT4);
    }

    /**
     * Gets a new control format, represented as a KeyControl, from the selected
     * keys in this KeyBinder.
     *
     * @return a KeyControl that consists of the selected keys
     */
    public KeyControl getKC() {
        return new KeyControl(keys);
    }
}
