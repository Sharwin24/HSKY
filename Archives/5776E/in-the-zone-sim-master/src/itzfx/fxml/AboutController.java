/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml;

import itzfx.Start;
import javafx.fxml.FXML;

/**
 *
 * @author prem
 */
public class AboutController {
    
    @FXML
    private void gameManual() {
        Start.navigate("https://content.vexrobotics.com/docs/vrc-inthezone/VRC-InTheZone-GameManual-20170817.pdf");
    }
    
    @FXML
    private void github() {
        Start.navigate("https://github.com/5776E/In-The-Zone-Simulation");
    }
}
