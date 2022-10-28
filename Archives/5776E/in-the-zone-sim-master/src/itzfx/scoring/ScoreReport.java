/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.scoring;

/**
 * The main reporting tool for all objects implementing the {@link Scoreable}
 * interface. Each of these must be registered with a
 * {@link ScoreAggregator score aggregator} for its score to be tallied.
 *
 * @author Prem Chintalapudi 5776E
 */
public class ScoreReport {

    private final Scoreable owner;
    private ScoreType st;

    /**
     * Creates a new score report, with the specified {@link Scoreable} owner.
     *
     * @param owner the Scoreable owner whose characteristics are used to help
     * score it.
     */
    public ScoreReport(Scoreable owner) {
        this.owner = owner;
        st = ScoreType.ZONE_NONE;
    }

    /**
     * Sets the extra points to be earned by the {@link Scoreable} owner.
     *
     * @param st the Score Type to set
     */
    public void setScoreType(ScoreType st) {
        this.st = st;
    }

    /**
     * Gets the {@link Scoreable} owner, which should have scored the points
     * being recorded by this report.
     *
     * @return the owner that scored these points
     */
    public Scoreable getOwner() {
        return owner;
    }

    /**
     * Gets the extra points, wrapped in a {@link ScoreType}.
     *
     * @return the set ScoreType
     *
     * @see ScoreReport#setScoreType(itzfx.scoring.ScoreType)
     */
    public ScoreType getType() {
        return st;
    }
}
