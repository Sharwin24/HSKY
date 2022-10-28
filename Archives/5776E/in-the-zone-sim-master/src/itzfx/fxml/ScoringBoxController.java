/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml;

import itzfx.ControlMode;
import itzfx.fxml.timing.Clock;
import itzfx.scoring.ScoreAggregator;
import java.util.Collections;
import javafx.beans.binding.Bindings;
import javafx.beans.property.DoubleProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.control.ComboBox;
import javafx.scene.control.SelectionModel;
import javafx.scene.control.SplitPane;
import javafx.scene.layout.AnchorPane;
import javafx.scene.text.Text;

/**
 * FXML Controller class. Controls the "ScoringBox.fxml" file. Maintains live
 * match scoring and autonomous scoring, when pulsed. Uses an internal
 * {@link ScoreAggregator score aggregator} to check scores.
 *
 * @author Prem Chintalapudi 5776E
 */
public class ScoringBoxController {

    @FXML
    private Parent root;

    @FXML
    private Text redScore;

    @FXML
    private Text blueScore;

    @FXML
    private Text skillsScore;

    @FXML
    private AnchorPane timerPane;

    @FXML
    private SplitPane scoreDivider;
    
    @FXML
    private ComboBox<ControlMode> comboBox;

    private Clock clock;

    private final IntegerProperty rScore;
    private final IntegerProperty bScore;
    private final IntegerProperty sScore;

    private ScoreAggregator sa;

    /**
     * Creates a new ScoringBoxController. This is generally called by
     * {@link FXMLLoader}.
     */
    public ScoringBoxController() {
        rScore = new SimpleIntegerProperty();
        bScore = new SimpleIntegerProperty();
        sScore = new SimpleIntegerProperty();
    }

    @FXML
    private void initialize() {
        redScore.textProperty().bind(Bindings.createStringBinding(() -> String.valueOf(rScore.get()), rScore));
        blueScore.textProperty().bind(Bindings.createStringBinding(() -> String.valueOf(bScore.get()), bScore));
        skillsScore.textProperty().bind(Bindings.createStringBinding(() -> String.valueOf(sScore.get()), sScore));
        root.setUserData(this);
        clock = (Clock) timerPane.getUserData();
        Collections.addAll(comboBox.getItems(), ControlMode.values());
        comboBox.getSelectionModel().select(ControlMode.FREE_PLAY);
    }

    /**
     * Gets the {@link DoubleProperty time property} on which the displayed
     * {@link Clock clock} is based.
     *
     * @return the time property that is monitored by the clock
     */
    public IntegerProperty getTime() {
        return clock.getTime();
    }

    /**
     * Sets the aggregator that determines scores on demand.
     *
     * @param sa the aggregator to set
     */
    public void setAggregator(ScoreAggregator sa) {
        this.sa = sa;
    }

    /**
     * Gets the scoring aggregator
     *
     * @return the aggregator relied upon by this controller
     */
    public ScoreAggregator getAggregator() {
        return sa;
    }

    /**
     * Updates the displayed scores using a calculation that generates scores
     * during the autonomous period.
     */
    public void pulseAuton() {
        int[] temp = sa.calculateAuton();
        rScore.set(temp[0]);
        bScore.set(temp[1]);
    }

    /**
     * Selects the autonomous winner based on the higher number of autonomous
     * points. Automatically adds 10 points to the alliance's score.
     */
    public void determineAutonWinner() {
        sa.determineAutonWinner();
    }

    /**
     * Updates the displayed scores using a calculation that generates scores
     * during the driver control period.
     */
    public void pulse() {
        int[] temp = sa.calculateMatch();
        rScore.set(temp[0]);
        bScore.set(temp[1]);
        sScore.set(sa.calculateSkills());
    }

    public void emphasizeSkills() {
        scoreDivider.setDividerPositions(0);
    }

    public void emphasizeTeams() {
        scoreDivider.setDividerPositions(1);
    }

    public void emphasizeNone() {
        scoreDivider.setDividerPositions(.667f);
    }
    
    public SelectionModel<ControlMode> getControlModeSelectionModel() {
        return comboBox.getSelectionModel();
    }

    /**
     * Displays a score sheet, formatted like an actual referee's score sheet.
     * Does not work for skills score
     */
    @FXML
    private void generateReport() {
        sa.showReport();
    }

    @FXML
    private void generateSkillsReport() {
        sa.showSkillsReport();
    }
}
