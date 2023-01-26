/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.scoring;

/**
 * An enumeration describing the additional points that may be earned due to
 * location for a {@link Scoreable}.
 *
 * @author Prem Chintalapudi 5776E
 */
public enum ScoreType {

    /**
     * No extra points are earned for this scoreable.
     */
    ZONE_NONE(0),
    /**
     * This is a {@link MobileGoal mobile goal} in its own 20 point zone. 20
     * extra points are earned for its alliance.
     */
    ZONE_20(20),
    /**
     * This is a {@link MobileGoal mobile goal} in its own 10 point zone. 10
     * extra points are earned for its alliance.
     */
    ZONE_10(10),
    /**
     * This is a {@link MobileGoal mobile goal} in its own 5 point zone. 5 extra
     * points are earned for its alliance.
     */
    ZONE_5(5),
    /**
     * This is a {@link StationaryGoal stationary goal}. No extra points are
     * earned.
     */
    STAT_GOAL(0),
    /**
     * This is a {@link Robot} parked in one of its starting tiles. 2 extra
     * points are earned for its alliance.
     */
    PARKING(2);

    private final int score;

    private ScoreType(int score) {
        this.score = score;
    }

    /**
     * Gets the extra points to be added to this alliance's total.
     *
     * @return the equivalent number of extra points, based on the type of this
     * {@link ScoreType}.
     */
    public int getScore() {
        return score;
    }
}
