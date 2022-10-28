/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.game.objects;

import itzfx.scoring.ScoreType;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.layout.StackPane;

/**
 * The class representing a Red {@link MobileGoal Mobile Goal}. Red mobile goals
 * are represented by the FXML code in the file "RedMobileGoal.fxml". They can
 * be stacked on and pushed.
 *
 * @author Prem Chintalapudi 5776E
 */
public class RedMobileGoal extends MobileGoal {

    private final StackPane mogo;

    /**
     * Constructs a new red {@link MobileGoal mobile goal} at the given
     * coordinates.
     *
     * @param layoutX the x coordinate at which to place this mobile goal
     * @param layoutY the y coordinate at which to place this mobile goal
     */
    public RedMobileGoal(float layoutX, float layoutY) {
        super(layoutX, layoutY);
        mogo = new StackPane();
        try {
            Parent load = FXMLLoader.load(RedMobileGoal.class.getResource("RedMobileGoal.fxml"));
            mogo.getChildren().add(load);
        } catch (IOException ex) {
            Logger.getLogger(RedMobileGoal.class.getName()).log(Level.SEVERE, null, ex);
        }
        super.registerProperties();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public ScoreType determineScoringZone(boolean skills) {
        ScoreType st = MobileGoal.inLowerLeftCorner(this);
        if (skills && st == ScoreType.ZONE_NONE) {
            st = MobileGoal.inUpperRightCorner(this);
        }
        return st;
    }

    /**
     * Returns true, as this is a <b>red</b> {@link MobileGoal mobile goal}.
     *
     * @return true
     */
    @Override
    public boolean isRed() {
        return true;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public StackPane getNode() {
        return mogo;
    }
}
