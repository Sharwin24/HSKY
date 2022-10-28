/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.Robot;
import java.util.Collection;
import java.util.Iterator;
import javafx.fxml.FXML;
import javafx.scene.layout.AnchorPane;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class AutonTabController {
    
    @FXML
    private AnchorPane worker1;
    @FXML
    private AutonTabWorkerController worker1Controller;
    @FXML
    private AnchorPane worker2;
    @FXML
    private AutonTabWorkerController worker2Controller;
    @FXML
    private AnchorPane worker3;
    @FXML
    private AutonTabWorkerController worker3Controller;
    @FXML
    private AnchorPane worker4;
    @FXML
    private AutonTabWorkerController worker4Controller;
    
    public void insertRobots(Collection<? extends Robot> c) {
        Iterator<? extends Robot> i = c.iterator();
        worker1Controller.setRobot(i.next());
        worker2Controller.setRobot(i.next());
        worker3Controller.setRobot(i.next());
        worker4Controller.setRobot(i.next());
    }
}
