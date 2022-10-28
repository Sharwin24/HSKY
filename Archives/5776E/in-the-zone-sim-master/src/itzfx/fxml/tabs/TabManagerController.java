/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import itzfx.Robot;
import itzfx.fxml.Field;
import java.util.Collection;
import javafx.fxml.FXML;
import javafx.scene.layout.AnchorPane;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class TabManagerController {
    
    @FXML
    private AnchorPane robotInfo;
    @FXML
    private RobotInfoController robotInfoController;
    @FXML
    private AnchorPane auton;
    @FXML
    private AutonTabController autonController;
    @FXML
    private AnchorPane robotData;
    @FXML
    private RobotDataController robotDataController;
    @FXML
    private AnchorPane match;
    @FXML
    private MatchController matchController;
    @FXML
    private AnchorPane programData;
    @FXML
    private SystemStatsController programDataController;
    
    public void insert(Field f, Collection<? extends Robot> c) {
        robotInfoController.injectRobots(c);
        robotDataController.injectRobots(c);
        autonController.insertRobots(c);
        matchController.insertField(f);
    }
}
