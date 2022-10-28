/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.Robot;
import itzfx.data.FileUI;
import itzfx.fxml.FXMLController;
import itzfx.fxml.controller.KeyBinder;
import itzfx.utils.CssUtils;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class RobotInfoWorkerController {

    @FXML
    private AnchorPane controlLayout;
    @FXML
    private ControlLayoutController controlLayoutController;

    @FXML
    private void create(MouseEvent event) {
        event.consume();
        FXMLLoader loader = new FXMLLoader(FXMLController.class.getResource("/itzfx/fxml/controller/KeyBinder.fxml"));
        KeyBinder kb = new KeyBinder(r.getController());
        loader.setController(kb);
        try {
            AnchorPane load = loader.load();
            Alert show = new Alert(Alert.AlertType.INFORMATION, "", ButtonType.APPLY, ButtonType.CANCEL);
            show.setHeaderText("Create a new controller");
            show.setTitle("Controller Creator");
            show.getDialogPane().setContent(load);
            CssUtils.styleDialog(show);
            show.showAndWait().map(ButtonType::getButtonData).filter(ButtonBar.ButtonData.APPLY::equals).ifPresent(bt -> {
                r.setController(kb.getKC());
                FileUI.saveKeyControl(kb.getKC(), r.getNode().getScene().getWindow());
            });
        } catch (IOException ex) {
            Logger.getLogger(FXMLController.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    @FXML
    private void load(MouseEvent event) {
        event.consume();
        FileUI.getKeyControl(r, r.getNode().getScene().getWindow());
    }

    private Robot r;

    public void insertRobot(Robot r) {
        this.r = r;
        controlLayoutController.insertRobot(r);
    }
}
