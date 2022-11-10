/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.Robot;
import itzfx.data.FileUI;
import itzfx.rerun.Translate;
import java.io.File;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ToggleButton;
import javafx.scene.text.Text;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class AutonTabWorkerController {

    @FXML
    private CheckBox recordingCheckBox;
    @FXML
    private Text autonName;

    private static final String DEFAULT_AUTON_TEXT = "No Autonomous Selected";

    @FXML
    private void initialize() {
        autonName.textProperty().addListener((o, b, s) -> {
            if (DEFAULT_AUTON_TEXT.equals(s)) {
                autonName.getStyleClass().add("error-text");
                autonName.getStyleClass().remove("success-text");
            } else {
                autonName.getStyleClass().remove("error-text");
                autonName.getStyleClass().add("success-text");
            }
        });
        recordingCheckBox.selectedProperty().addListener((o, b, s) -> {
            if (s) {
                r.record();
            } else {
                r.stopRecording();
                FileUI.saveRerun(r, r.getNode().getScene().getWindow());
            }
        });
    }

    private Robot r;

    public void setRobot(Robot r) {
        this.r = r;
    }

    @FXML
    private void handleAutonChange() {
        File f = FileUI.getRerun(r, r.getNode().getScene().getWindow());
        if (f != null) {
            String text = f.getName();
            autonName.setText(text.substring(0, text.indexOf('.')));
        }
    }
    
    @FXML
    private void translateTime() {
        Translate.userTranslateToTime(autonName.getScene().getWindow());
    }
}
