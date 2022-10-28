/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.scoring;

import itzfx.fxml.FXMLController;
import itzfx.fxml.ScoreSheetController;
import itzfx.fxml.SkillsScoreSheetController;
import itzfx.utils.CssUtils;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonBar.ButtonData;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;

/**
 * This class tallies up scores from registered
 * {@link ScoreReport score reports} to give final red and blue match/autonomous
 * scores. Skills scoring will be added in a future release. A new skills score
 * user interface will be added to support skills scoring as well.
 *
 * @author Prem Chintalapudi 5776E
 */
public class ScoreAggregator {

    private final List<ScoreReport> reports;

    private Alliance autonomous = Alliance.NONE;

    /**
     * Constructs a new ScoreAggregator.
     */
    public ScoreAggregator() {
        reports = new ArrayList<>();
    }

    /**
     * Registeres a {@link ScoreReport score report} with this aggregator.
     * Registering a report means that this report will be scored, and its score
     * will be added to its corresponding alliance's score.
     *
     * @param sr the score report to register
     */
    public void registerReport(ScoreReport sr) {
        reports.add(sr);
    }

    /**
     * Calculates match score for the currently registered score reports.
     *
     * @return the calculated match scores in the order {red, blue}
     *
     * @see ScoreAggregator#calculate()
     */
    public int[] calculateMatch() {
        int vals[] = updatedReport();
        return new int[]{
            vals[0] * 20 + vals[2] * 10 + vals[4] * 5 + vals[6] * 2 + vals[8] * 5 + vals[10] * 2 + vals[12] * 10,
            vals[1] * 20 + vals[3] * 10 + vals[5] * 5 + vals[7] * 2 + vals[9] * 5 + vals[11] * 2 + vals[13] * 10
        };
    }

    private int calculateSkillsScore() {
        int[] vals = updatedSkillsReport();
        return vals[0] * 20 + vals[1] * 10 + vals[2] * 5 + vals[3] * 2 + vals[4] * 2;
    }

    /**
     * Calculates autonomous score (no parking bonus) for the currently
     * registered score reports.
     *
     * @return the calculated autonomous scores in the order {red, blue}
     *
     */
    public int[] calculateAuton() {
        int[] vals = updatedReport();
        return new int[]{
            vals[0] * 20 + vals[2] * 10 + vals[4] * 5 + vals[6] * 2 + vals[8] * 5,
            vals[1] * 20 + vals[3] * 10 + vals[5] * 5 + vals[7] * 2 + vals[9] * 5
        };
    }

    private Boolean[] highStack(AtomicInteger aiR, AtomicInteger aiB) {
        MaxableInt r20 = new MaxableInt();
        MaxableInt b20 = new MaxableInt();
        MaxableInt r10 = new MaxableInt();
        MaxableInt b10 = new MaxableInt();
        MaxableInt r5 = new MaxableInt();
        MaxableInt b5 = new MaxableInt();
        MaxableInt redStat = new MaxableInt();
        MaxableInt blueStat = new MaxableInt();
        reports.forEach(sr -> {
            switch (sr.getType()) {
                case ZONE_20:
                    if (sr.getOwner().isRed()) {
                        r20.max(sr.getOwner().score());
                    } else {
                        b20.max(sr.getOwner().score());
                    }
                    break;
                case ZONE_10:
                    if (sr.getOwner().isRed()) {
                        r10.max(sr.getOwner().score());
                    } else {
                        b10.max(sr.getOwner().score());
                    }
                    break;
                case ZONE_5:
                    if (sr.getOwner().isRed()) {
                        r5.max(sr.getOwner().score());
                    } else {
                        b5.max(sr.getOwner().score());
                    }
                    break;
                case STAT_GOAL:
                    if (sr.getOwner().isRed()) {
                        redStat.max(sr.getOwner().score());
                    } else {
                        blueStat.max(sr.getOwner().score());
                    }
            }
        });
        return new Boolean[]{add(r20, b20, aiR, aiB), add(r10, b10, aiR, aiB), add(r5, b5, aiR, aiB), add(redStat, blueStat, aiR, aiB)};
    }

    /**
     * Sets the autonomous winner of this ScoreAggregator based on which
     * alliance has more points currently. That alliance gets a 10 point bonus.
     */
    public void determineAutonWinner() {
        int[] temp = calculateAuton();
        autonomous = temp[0] > temp[1] ? Alliance.RED : (temp[1] > temp[0] ? Alliance.BLUE : Alliance.NONE);
    }
    
    private static enum Alliance {
        RED, BLUE, NONE;
    }

    /**
     * Clears the autonomous winner, in preparation for a new match.
     */
    public void clearAuton() {
        autonomous = null;
    }

    private static Boolean add(MaxableInt red, MaxableInt blue, AtomicInteger aiR, AtomicInteger aiB) {
        if (red.get() > blue.get()) {
            aiR.addAndGet(5);
            return true;
        } else if (red.get() < blue.get()) {
            aiB.addAndGet(5);
            return false;
        } else {
            return null;
        }
    }

    /**
     * Calculates the skills score.
     *
     * @return the calculated skills score
     */
    public int calculateSkills() {
        return calculateSkillsScore();
    }

    private int[] updatedReport() {
        // red20, blue20, red10, blue10, red5, blue5, redCones, blueCones, redStacks, blueStacks, redParks, blueParks, redAuton, blueAuton;
        AtomicInteger[] scores = new AtomicInteger[14];
        for (int i = 0; i < scores.length; i++) {
            scores[i] = new AtomicInteger();
        }
        reports.forEach(r -> {
            if (r.getOwner().isRed()) {
                scores[6].addAndGet(r.getOwner().score() / 2);
                switch (r.getType()) {
                    case ZONE_20:
                        scores[0].set(1);
                        break;
                    case ZONE_10:
                        scores[2].incrementAndGet();
                        break;
                    case ZONE_5:
                        scores[4].incrementAndGet();
                        break;
                    case PARKING:
                        scores[10].incrementAndGet();
                        break;
                    case ZONE_NONE:
                    case STAT_GOAL:
                    default:
                        break;
                }
            } else {
                scores[7].addAndGet(r.getOwner().score() / 2);
                switch (r.getType()) {
                    case ZONE_20:
                        scores[1].set(1);
                        break;
                    case ZONE_10:
                        scores[3].incrementAndGet();
                        break;
                    case ZONE_5:
                        scores[5].incrementAndGet();
                        break;
                    case PARKING:
                        scores[11].incrementAndGet();
                        break;
                    case ZONE_NONE:
                    case STAT_GOAL:
                    default:
                        break;
                }
            }
        });
        Boolean[] stacks = highStack(new AtomicInteger(), new AtomicInteger());
        for (Boolean b : stacks) {
            if (b != null) {
                if (b) {
                    scores[8].incrementAndGet();
                } else {
                    scores[9].incrementAndGet();
                }
            }
        }
        if (autonomous != Alliance.NONE) {
            scores[autonomous == Alliance.RED ? 12 : 13].set(1);
        }
        return Arrays.stream(scores).mapToInt(AtomicInteger::intValue).toArray();
    }

    /**
     * Shows a {@link ScoreSheet score sheet} in a {@link Dialog}. This is
     * mostly for visual looks and UX enhancement.
     *
     * @see ScoreSheetController
     */
    public void showReport() {
        FXMLLoader loader = new FXMLLoader(ScoreAggregator.class.getResource("/itzfx/fxml/ScoreSheet.fxml"));
        try {
            Pane load = (loader.load());
            StackPane report = new StackPane(load);
            report.setPadding(new Insets(10));
            ScoreSheetController ssc = loader.getController();
            ssc.update(updatedReport());
            Alert show = new Alert(Alert.AlertType.CONFIRMATION, "", new ButtonType("Copy", ButtonData.OK_DONE), ButtonType.CANCEL);
            show.setGraphic(getGraphic());
            show.setTitle("Match Score Report");
            show.setHeaderText("Match Report");
            show.getDialogPane().setContent(report);
            CssUtils.styleDialog(show);
            show.showAndWait().filter(bt -> bt.getButtonData() == ButtonData.OK_DONE)
                    .ifPresent(bt -> FXMLController.copy(FXMLController.takeScreenshot(report)));
        } catch (IOException ex) {
            Logger.getLogger(ScoreAggregator.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private Node getGraphic() throws IOException {
        Node n = FXMLLoader.load(ScoreAggregator.class.getResource("/itzfx/fxml/ScoreSheet.fxml"));
        n.setScaleX(0.1);
        n.setScaleY(0.1);
        return new Group(n);
    }


    private int[] updatedSkillsReport() {
        //red20, blue20, red10, blue10, red5, blue5, redCones, blueCones, redParks, blueParks;
        AtomicInteger[] scores = new AtomicInteger[10];
        for (int i = 0; i < scores.length; i++) {
            scores[i] = new AtomicInteger();
        }
        reports.forEach(r -> {
            if (r.getOwner().isRed()) {
                scores[6].addAndGet(r.getOwner().scoreSkills() / 2);
                switch (r.getType()) {
                    case ZONE_20:
                        scores[0].set(1);
                        break;
                    case ZONE_10:
                        scores[2].incrementAndGet();
                        break;
                    case ZONE_5:
                        scores[4].incrementAndGet();
                        break;
                    case PARKING:
                        scores[8].incrementAndGet();
                        break;
                    case ZONE_NONE:
                    case STAT_GOAL:
                    default:
                        break;
                }
            } else {
                scores[7].addAndGet(r.getOwner().score() / 2);
                switch (r.getType()) {
                    case ZONE_20:
                        scores[1].set(1);
                        break;
                    case ZONE_10:
                        scores[3].incrementAndGet();
                        break;
                    case ZONE_5:
                        scores[5].incrementAndGet();
                        break;
                    case PARKING:
                        scores[9].incrementAndGet();
                        break;
                    case ZONE_NONE:
                    case STAT_GOAL:
                    default:
                        break;
                }
            }
        });
        return new int[]{scores[0].intValue() + scores[1].intValue(), scores[2].intValue() + scores[3].intValue(),
            scores[4].intValue() + scores[5].intValue(), scores[6].intValue() + scores[7].intValue(),
            scores[8].intValue() + scores[9].intValue()};
    }

    public void showSkillsReport() {
        FXMLLoader loader = new FXMLLoader(ScoreAggregator.class.getResource("/itzfx/fxml/SkillsScoreSheet.fxml"));
        try {
            Pane load = (loader.load());
            StackPane report = new StackPane(load);
            report.setPadding(new Insets(10));
            loader.<SkillsScoreSheetController>getController().update(updatedSkillsReport());
            Alert show = new Alert(Alert.AlertType.CONFIRMATION, "", new ButtonType("Copy", ButtonData.OK_DONE), ButtonType.CANCEL);
            show.setGraphic(getSkillsGraphic());
            show.setTitle("Skills Score Report");
            show.setHeaderText("Skills Report");
            show.getDialogPane().setContent(report);
            CssUtils.styleDialog(show);
            show.showAndWait().map(ButtonType::getButtonData).filter(bd -> bd == ButtonData.OK_DONE)
                    .ifPresent(bt -> FXMLController.copy(FXMLController.takeScreenshot(report)));
        } catch (IOException ex) {
            Logger.getLogger(ScoreAggregator.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
    private Node getSkillsGraphic() throws IOException {
        Node n = FXMLLoader.load(ScoreAggregator.class.getResource("/itzfx/fxml/SkillsScoreSheet.fxml"));
        n.setScaleX(0.1);
        n.setScaleY(0.1);
        return new Group(n);
    }

    private static class MaxableInt {

        private int integer;

        public int get() {
            return integer;
        }

        public int max(int compare) {
            return integer = Math.max(integer, compare);
        }
    }
}
