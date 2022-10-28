/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.Robot;
import itzfx.data.FileUI;
import itzfx.fxml.FXMLController;
import itzfx.fxml.build.RobotBuilder;
import itzfx.utils.CssUtils;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Label;
import javafx.scene.control.TabPane;
import javafx.scene.input.MouseEvent;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class RobotDataWorkerController {

    @FXML
    private Label speed;
    @FXML
    private Label autostackTime;
    @FXML
    private Label mogoTime;
    @FXML
    private Label statStackTime;
    @FXML
    private Label mogoFront;
    @FXML
    private Label maxMogoCount;
    @FXML
    private Label maxStackCount;
    
    private Robot r;
    
    public void insertRobot(Robot r) {
        this.r = r;
        link();
    }
    
    private void link() {
        speed.textProperty().bind(Bindings.createStringBinding(() -> r.speedProperty().get() + " in/sec", r.speedProperty()));
        autostackTime.textProperty().bind(Bindings.createStringBinding(() -> r.autostackTimeProperty().get() + " sec", r.autostackTimeProperty()));
        mogoTime.textProperty().bind(Bindings.createStringBinding(() -> r.mogoIntakeTimeProperty().get() + " sec", r.mogoIntakeTimeProperty()));
        statStackTime.textProperty().bind(Bindings.createStringBinding(() -> r.statStackTimeProperty().get() + " sec", r.statStackTimeProperty()));
        mogoFront.textProperty().bind(Bindings.createStringBinding(() -> r.mogoFrontProperty().get() ? "At Front" : "At Back", r.mogoFrontProperty()));
        maxMogoCount.textProperty().bind(Bindings.createStringBinding(() -> r.maxMogoStackProperty().get() + " cones", r.maxMogoStackProperty()));
        maxStackCount.textProperty().bind(Bindings.createStringBinding(() -> r.maxStatStackProperty().get() + " cones", r.maxStatStackProperty()));
    }

    @FXML
    private void create(MouseEvent event) {
        event.consume();
        FXMLLoader loader = new FXMLLoader(FXMLController.class.getResource("/itzfx/fxml/build/RobotBuilder.fxml"));
        try {
            TabPane p = loader.load();
            RobotBuilder rb = loader.getController();
            Alert show = new Alert(Alert.AlertType.INFORMATION, "", ButtonType.APPLY, ButtonType.CANCEL);
            CssUtils.styleDialog(show);
            show.setHeaderText("Build a Robot");
            show.setTitle("Robot Builder");
            show.getDialogPane().setContent(p);
            show.getDialogPane().setPrefHeight(500);
            show.showAndWait().map(ButtonType::getButtonData).filter(ButtonBar.ButtonData.APPLY::equals).ifPresent(bt -> {
                rb.submit();
                rb.fillRobot(r);
                FileUI.saveRobot(r, r.getNode().getScene().getWindow());
            });
        } catch (IOException ex) {
            Logger.getLogger(FXMLController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    @FXML
    private void load(MouseEvent event) {
        event.consume();
        FileUI.fillRobot(r, r.getNode().getScene().getWindow());
    }
    
}
