/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.scoring;

/**
 * The interface marking which objects are scoreable in a game. Any object which
 * implements this interface should also register its own score report with the
 * owner {@link Field field}'s {@link ScoreAggregator score aggregator}.
 *
 * @author Prem Chintalapudi 5776E
 */
public interface Scoreable {

    /**
     * Scores this Scoreable. This method should return a score for the number
     * of cones stacked multiplied by 2 <b>only</b>. If there are no cones
     * stacked, this should return 0.
     *
     * @return the cone score of this Scoreable
     */
    public int score();

    /**
     * Determines the alliance this Scoreable is on. This is used to determine
     * which alliance this scoreable's points go to in a match.
     *
     * @return true if this is on the red alliance, false if this is on the blue
     * alliance.
     */
    public boolean isRed();
    
    /**
     * Scores this Scoreable as if it were in skills score. 
     * @return 
     */
    public default int scoreSkills() {
        return score();
    }
}
