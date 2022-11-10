/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.fxml.Field;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ToggleButton;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class MatchController {

    @FXML
    private Button resetButton;
    @FXML
    private Button preAutonButton;
    @FXML
    private ToggleButton playingButton;
    @FXML
    private Button beginButton;

    @FXML
    private void initialize() {
        playingButton.selectedProperty().addListener((o, b, s) -> {
            if (s) {
                f.play();
            } else {
                f.pause();
            }
        });
    }

    @FXML
    private void reset(ActionEvent event) {
        event.consume();
        preAutonButton.setDisable(false);
        f.reset();
    }

    @FXML
    private void preAuton(ActionEvent event) {
        event.consume();
        f.preMatch();
    }

    @FXML
    private void begin(ActionEvent event) {
        event.consume();
        preAutonButton.setDisable(true);
        f.startMatch();
    }

    private Field f;

    public void insertField(Field f) {
        this.f = f;
        f.onGameStateChanged((b, s) -> {
            switch (s) {
                case PAUSED:
                    playingButton.setSelected(false);
                    break;
                case PLAYING:
                    playingButton.setSelected(true);
                    break;
                case STOPPED:
                    preAutonButton.setDisable(false);
            }
        });
    }
}
