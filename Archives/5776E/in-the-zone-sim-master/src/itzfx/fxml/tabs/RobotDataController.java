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
public class RobotDataController {
    
    @FXML
    private AnchorPane worker1;
    @FXML
    private AnchorPane worker2;
    @FXML
    private AnchorPane worker3;
    @FXML
    private AnchorPane worker4;
    @FXML
    private RobotDataWorkerController worker1Controller;
    @FXML
    private RobotDataWorkerController worker2Controller;
    @FXML
    private RobotDataWorkerController worker3Controller;
    @FXML
    private RobotDataWorkerController worker4Controller;
    
    public void injectRobots(Collection<? extends Robot> c) {
        Iterator<? extends Robot> i = c.iterator();
        worker1Controller.insertRobot(i.next());
        worker2Controller.insertRobot(i.next());
        worker3Controller.insertRobot(i.next());
        worker4Controller.insertRobot(i.next());
    }
}
