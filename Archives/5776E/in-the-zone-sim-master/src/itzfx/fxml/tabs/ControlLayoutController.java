/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.Robot;
import javafx.css.PseudoClass;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.layout.AnchorPane;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class ControlLayoutController {

    private static final PseudoClass ACTIVE = PseudoClass.getPseudoClass("active");

    @FXML
    private AnchorPane key0;
    @FXML
    private AnchorPane key1;
    @FXML
    private AnchorPane key2;
    @FXML
    private AnchorPane key3;
    @FXML
    private AnchorPane key4;
    @FXML
    private AnchorPane key5;
    @FXML
    private AnchorPane key6;
    @FXML
    private AnchorPane key7;
    @FXML
    private AnchorPane key8;
    @FXML
    private KeyController key0Controller;
    @FXML
    private KeyController key1Controller;
    @FXML
    private KeyController key2Controller;
    @FXML
    private KeyController key3Controller;
    @FXML
    private KeyController key4Controller;
    @FXML
    private KeyController key5Controller;
    @FXML
    private KeyController key6Controller;
    @FXML
    private KeyController key7Controller;
    @FXML
    private KeyController key8Controller;

    public void insertRobot(Robot r) {
        r.forwardKeyProperty().addListener((o, b, s) -> key1Controller.setText(format(s)));
        r.autostackKeyProperty().addListener((o, b, s) -> key2Controller.setText(format(s)));
        r.leftKeyProperty().addListener((o, b, s) -> key3Controller.setText(format(s)));
        r.backKeyProperty().addListener((o, b, s) -> key4Controller.setText(format(s)));
        r.rightKeyProperty().addListener((o, b, s) -> key5Controller.setText(format(s)));
        r.coneKeyProperty().addListener((o, b, s) -> key6Controller.setText(format(s)));
        r.loadKeyProperty().addListener((o, b, s) -> key7Controller.setText(format(s)));
        r.statKeyProperty().addListener((o, b, s) -> key8Controller.setText(format(s)));
        r.mobileGoalKeyProperty().addListener((o, b, s) -> key0Controller.setText(format(s)));
        key1Controller.setText(format(r.forwardKeyProperty().get()));
        key2Controller.setText(format(r.autostackKeyProperty().get()));
        key3Controller.setText(format(r.leftKeyProperty().get()));
        key4Controller.setText(format(r.backKeyProperty().get()));
        key5Controller.setText(format(r.rightKeyProperty().get()));
        key6Controller.setText(format(r.coneKeyProperty().get()));
        key7Controller.setText(format(r.loadKeyProperty().get()));
        key8Controller.setText(format(r.statKeyProperty().get()));
        key0Controller.setText(format(r.mobileGoalKeyProperty().get()));
        EventHandler<KeyEvent> lightUp = k -> {
            if (k.getCode() == r.forwardKeyProperty().get()) {
                handleKey(key1, k);
            } else if (k.getCode() == r.autostackKeyProperty().get()) {
                handleKey(key2, k);
            } else if (k.getCode() == r.leftKeyProperty().get()) {
                handleKey(key3, k);
            } else if (k.getCode() == r.backKeyProperty().get()) {
                handleKey(key4, k);
            } else if (k.getCode() == r.rightKeyProperty().get()) {
                handleKey(key5, k);
            } else if (k.getCode() == r.coneKeyProperty().get()) {
                handleKey(key6, k);
            } else if (k.getCode() == r.loadKeyProperty().get()) {
                handleKey(key7, k);
            } else if (k.getCode() == r.statKeyProperty().get()) {
                handleKey(key8, k);
            } else if (k.getCode() == r.mobileGoalKeyProperty().get()) {
                handleKey(key0, k);
            }
        };
        r.getNode().sceneProperty().addListener((o, b, s) -> {
            if (b != null) {
                b.removeEventHandler(KeyEvent.KEY_PRESSED, lightUp);
                b.removeEventHandler(KeyEvent.KEY_RELEASED, lightUp);
            }
            s.addEventHandler(KeyEvent.KEY_PRESSED, lightUp);
            s.addEventHandler(KeyEvent.KEY_RELEASED, lightUp);
        });
    }

    private void handleKey(AnchorPane key, KeyEvent k) {
        if (k.getEventType() == KeyEvent.KEY_PRESSED) {
            key.pseudoClassStateChanged(ACTIVE, true);
        } else if (k.getEventType() == KeyEvent.KEY_RELEASED) {
            key.pseudoClassStateChanged(ACTIVE, false);
        }
    }

    private String format(KeyCode keyCode) {
        switch (keyCode) {
            case UP:
                return String.valueOf('\u8593');
            case DOWN:
                return String.valueOf('\u8595');
            case LEFT:
                return String.valueOf('\u8592');
            case RIGHT:
                return String.valueOf('\u8594');
            case NUMPAD0:
            case NUMPAD1:
            case NUMPAD2:
            case NUMPAD3:
            case NUMPAD4:
            case NUMPAD5:
            case NUMPAD6:
            case NUMPAD7:
            case NUMPAD8:
            case NUMPAD9:
                return "N" + keyCode.getName().substring(keyCode.getName().length() - 1);
            case DIGIT0:
            case DIGIT1:
            case DIGIT2:
            case DIGIT3:
            case DIGIT4:
            case DIGIT5:
            case DIGIT6:
            case DIGIT7:
            case DIGIT8:
            case DIGIT9:
                return "D" + keyCode.getName().substring(keyCode.getName().length() - 1);
            case QUOTE:
                return "\'";
            case CLOSE_BRACKET:
                return "]";
            case OPEN_BRACKET:
                return "[";
            case SEMICOLON:
                return ";";
            case SLASH:
                return "/";
            case PERIOD:
                return ".";
            case COMMA:
                return ",";
            default:
                return keyCode.getName();
        }
    }
}
