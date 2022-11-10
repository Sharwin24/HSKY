/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.game.objects;

import itzfx.Mobile;
import javafx.fxml.FXML;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.Pane;

/**
 * A class that adds drag functionality to some FXML loads. Specifically, this
 * class is known to be the controller class for {@link Cone Cones} and
 * {@link MobileGoal Mobile Goals}.
 *
 * @author Prem Chintalapudi 5776E
 */
public class MobileController {

    @FXML
    private Pane mobile;

    private final java.awt.Point.Float mouse = new java.awt.Point.Float();

    @FXML
    private void onPress(MouseEvent m) {
        if (m.isPrimaryButtonDown()) {
            mouse.x = (float) m.getX();
            mouse.y = (float) m.getY();
        }
    }

    @FXML
    private void onDragged(MouseEvent m) {
        if (m.isPrimaryButtonDown()) {
            ((Mobile) mobile.getParent().getUserData()).shiftCenter((float) m.getX() - mouse.x, (float) m.getY() - mouse.y);
        }
    }
}
