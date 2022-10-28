/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.timing;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.layout.AnchorPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Polygon;

/**
 * FXML Controller class. Controls "Digit.fxml". Base class for displaying a
 * single digit without high rendering costs. This render relies on a display
 * that looks like a 7-light LED display.
 *
 * @author Prem Chintalapudi 5776E
 */
public class Digit {
    
    @FXML
    private AnchorPane root;

    @FXML
    private Polygon top;
    @FXML
    private Polygon leftUp;
    @FXML
    private Polygon middle;
    @FXML
    private Polygon rightDown;
    @FXML
    private Polygon bottom;
    @FXML
    private Polygon leftDown;
    @FXML
    private Polygon rightUp;

    @FXML
    private void initialize() {
        root.setUserData(this);
    }

    private void manipulate(boolean top, boolean leftUp, boolean middle, boolean rightDown, boolean bottom, boolean leftDown, boolean rightUp) {
        Platform.runLater(() -> {
            this.top.setDisable(!top);
            this.leftUp.setDisable(!leftUp);
            this.middle.setDisable(!middle);
            this.rightDown.setDisable(!rightDown);
            this.bottom.setDisable(!bottom);
            this.leftDown.setDisable(!leftDown);
            this.rightUp.setDisable(!rightUp);
        });
    }

    /**
     * Configures this object to create a visual representation of the passed
     * digit. This will throw an AssertionError if the passed value is not a
     * valid digit.
     *
     * @param digit the digit to display
     *
     * @throws AssertionError if the given value is <b>not</b> a single digit
     */
    public void display(int digit) {
        assert digit > -1 && digit < 10;
        switch (digit) {
            case 1:
                manipulate(false, false, false, true, false, false, true);
                break;
            case 2:
                manipulate(true, false, true, false, true, true, true);
                break;
            case 3:
                manipulate(true, false, true, true, true, false, true);
                break;
            case 4:
                manipulate(false, true, true, true, false, false, true);
                break;
            case 5:
                manipulate(true, true, true, true, true, false, false);
                break;
            case 6:
                manipulate(true, true, true, true, true, true, false);
                break;
            case 7:
                manipulate(true, false, false, true, false, false, true);
                break;
            case 8:
                manipulate(true, true, true, true, true, true, true);
                break;
            case 9:
                manipulate(true, true, true, true, true, false, true);
                break;
            case 0:
                manipulate(true, true, false, true, true, true, true);
        }
    }
}
