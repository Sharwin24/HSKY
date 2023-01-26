/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.build;

import itzfx.Robot;
import itzfx.RobotProperties;
import javafx.fxml.FXML;
import javafx.scene.control.TabPane;
import javafx.scene.layout.Pane;

/**
 * FXML Controller class. Controls "RobotBuilder.fxml". Main header for visuals
 * attempting to show a robot builder.
 *
 * @author Prem Chintalapudi 5776E
 */
public class RobotBuilder {

    @FXML
    private TabPane root;
    @FXML
    private Pane rcPane;
    @FXML
    private Pane mcPane;
    @FXML
    private Pane scPane;

    private RobotConfig rc;
    private MogoConfig mc;
    private StackConfig sc;

    @FXML
    private void initialize() {
        rc = (RobotConfig) rcPane.getUserData();
        mc = (MogoConfig) mcPane.getUserData();
        sc = (StackConfig) scPane.getUserData();
        root.setUserData(this);
    }

    private float speed;
    private float autostack;
    private float statstack;
    private float mogotime;
    private int maxmogo;
    private int maxstat;
    private int front;

    /**
     * Gets all values from the different tabs and saves them in this object.
     * This method <b>must</b> be called prior to filling robots.
     */
    public void submit() {
        speed = rc.getSpeed();
        autostack = rc.getAutostackTime();
        statstack = rc.getStatTime();
        mogotime = mc.getMogoTime();
        front = mc.isFrontMogo();
        maxmogo = sc.getMogoStack();
        maxstat = sc.getStatStack();
    }

    /**
     * Sets a robot's values to the values entered by the user. If the user has
     * not entered a value, the robot should interpret that as keeping its
     * previous value.
     *
     * @param r the robot to set the values of
     */
    public void fillRobot(Robot r) {
        r.acceptValues(RobotProperties.create(speed, mogotime, autostack, statstack, maxmogo, maxstat, front));
    }
}
