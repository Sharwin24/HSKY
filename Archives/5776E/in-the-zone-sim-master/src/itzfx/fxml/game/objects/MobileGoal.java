/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.game.objects;

import itzfx.Hitbox;
import itzfx.Mobile;
import itzfx.Robot;
import itzfx.scoring.ScoreType;
import itzfx.scoring.ScoreReport;
import itzfx.scoring.Scoreable;
import java.io.IOException;
import java.util.ArrayList;
import javafx.beans.binding.Bindings;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener.Change;
import javafx.collections.ObservableList;
import javafx.fxml.FXMLLoader;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
import javafx.scene.text.Text;

/**
 * This class is the root class for all mobile goals. It handles stacking and
 * scoring, as well as collisions.
 *
 * @author Prem Chintalapudi 5776E
 */
public abstract class MobileGoal extends Mobile implements Scoreable {

    private final ObservableList<Cone> stacked;
    private final IntegerProperty countModifier;
    private Hitbox hitbox;
    private final Text stackLabel;

    private final ScoreReport sr;

    /**
     * Constructs a new Mobile Goal at the given coordinates.
     *
     * @param layoutX the specified x coordinate
     * @param layoutY the specified y coordinate
     */
    protected MobileGoal(float layoutX, float layoutY) {
        super(layoutX, layoutY);
        this.stacked = FXCollections.observableList(new ArrayList<>());
        this.stackLabel = new Text();
        sr = new ScoreReport(this);
        countModifier = new SimpleIntegerProperty();
    }

    /**
     * {@inheritDoc}
     *
     * Adds stacking effects to the node returned by {@link Mobile#getNode()}
     */
    @Override
    protected void registerProperties() {
        try {
            Pane cone = FXMLLoader.load(MobileGoal.class.getResource("StackedCone.fxml"));
            cone.visibleProperty().bind(stackLabel.visibleProperty());
            getNode().getChildren().add(cone);
        } catch (IOException ex) {
        }
        stackLabel.visibleProperty().bind(Bindings.createBooleanBinding(() -> stacked.size() + countModifier.intValue() > 0, stacked, countModifier));
        stackLabel.textProperty().bind(Bindings.createStringBinding(() -> String.valueOf(stacked.size() + countModifier.intValue()), stacked, countModifier));
        stackLabel.getStyleClass().add("stack-label");
        stacked.addListener((Change<? extends Cone> change) -> {
            change.next();
            if (change.wasAdded()) {
                change.getAddedSubList().forEach(Cone::stack);
            } else if (change.wasRemoved()) {
                change.getRemoved().forEach(Cone::destack);
            }
        });
        this.hitbox = new Hitbox(25, Hitbox.CollisionType.STRONG, this, 15);
        Hitbox.register(hitbox);
        getNode().getChildren().addAll(stackLabel, hitbox.getVisual());
        super.registerProperties();
        super.centerXProperty().addListener(Mobile.limitToField(25, super.centerXProperty()));
        super.centerYProperty().addListener(Mobile.limitToField(25, super.centerYProperty()));
        super.centerXProperty().addListener(super.exclude20(25));
        super.centerYProperty().addListener(super.exclude20(25));
    }

    /**
     * Adds "Stack" and "Destack" options.
     *
     * @param rightClick {@inheritDoc}
     */
    @Override
    protected void rightClickOptions(ContextMenu rightClick) {
        MenuItem stack = new MenuItem("Stack");
        stack.setOnAction(e -> countModifier.set(countModifier.get() + 1));
        rightClick.getItems().add(stack);
        MenuItem destack = new MenuItem("Destack");
        destack.setOnAction(e -> countModifier.set(countModifier.get() + stacked.size() > 0 ? countModifier.get() - 1 : -stacked.size()));
        rightClick.getItems().add(destack);
    }

    /**
     * Gets the {@link ScoreReport score reporter} that will be updated when
     * this mobile goal is scored.
     *
     * @return this mobile goal's score reporter
     */
    public ScoreReport getReporter() {
        return sr;
    }

    /**
     * "Stacks" a {@link Cone} on top of this mobile goal. In reality, the given
     * cone is supposed to be vanished, while the count displayed on top of this
     * mobile goal is incremented.
     *
     * @param cone the cone to stack
     */
    public void stack(Cone cone) {
        stacked.add(cone);
    }

    /**
     * "Destacks" a {@link Cone} from this mobile goal. Removes and returns a
     * cone that was placed on top of this mobile goal.
     *
     * @return the cone that was destacked.
     */
    public Cone destack() {
        return stacked.size() > 0 ? stacked.remove(0) : null;
    }

    /**
     * Shifts the whole {@link Cone cone} stack onto another mobile goal. This
     * is generally used when shifting between a {@link Robot robot}'s private
     * mobile goals and a mobile goal on the field.
     *
     * @param other the mobile goal to shift this mobile goal's stack onto
     */
    public void shiftStack(MobileGoal other) {
        while (stacked.size() > 0) {
            other.stack(stacked.remove(0));
        }
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void disableCollision() {
        hitbox.disable();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void enableCollision() {
        hitbox.enable();
    }

    /**
     * {@inheritDoc}
     *
     * @return {@inheritDoc}
     */
    @Override
    public boolean canCollide() {
        return hitbox.canCollide();
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public void permaDisableCollisions() {
        Hitbox.unregister(hitbox);
    }

    /**
     * {@inheritDoc}
     *
     * Converts this mobile goal's list to a stack of 0, resets the count
     * modifier, and resets all the previously held cones.
     */
    @Override
    protected void resetProperties() {
        while (stacked.size() > 0) {
            destack().reset();
        }
        countModifier.set(0);
    }

    /**
     * {@inheritDoc}
     *
     * @return {@inheritDoc}
     */
    @Override
    public abstract StackPane getNode();

    /**
     * {@inheritDoc}
     *
     * @return {@inheritDoc}
     */
    @Override
    public int score() {
        sr.setScoreType(determineScoringZone(false));
        return stacked.size() * 2;
    }

    @Override
    public int scoreSkills() {
        sr.setScoreType(determineScoringZone(true));
        return stacked.size() * 2;
    }

    /**
     * A method that determines which point zone this mobile goal is in. This
     * method will return the corresponding {@link ScoreType} to the current
     * zone. It is called during scoring.
     *
     * @param skills if this is a skills run.
     * @return the score type that represents the zone this mobile goal is in.
     */
    protected abstract ScoreType determineScoringZone(boolean skills);

    protected static ScoreType inLowerLeftCorner(Mobile m) {
        if (!m.isVanished()) {
            if (m.getCenterY() - m.getCenterX() > 600 - 15) {
                return ScoreType.ZONE_20;
            } else if (m.getCenterY() - m.getCenterX() > 480 - 15) {
                return ScoreType.ZONE_10;
            } else if (m.getCenterY() - m.getCenterX() > 360 - 15) {
                return ScoreType.ZONE_5;
            }
        }
        return ScoreType.ZONE_NONE;
    }

    protected static ScoreType inUpperRightCorner(Mobile m) {
        if (!m.isVanished()) {
            if (m.getCenterX() - m.getCenterY() > 600 - 15) {
                return ScoreType.ZONE_20;
            } else if (m.getCenterX() - m.getCenterY() > 480 - 15) {
                return ScoreType.ZONE_10;
            } else if (m.getCenterX() - m.getCenterY() > 360 - 15) {
                return ScoreType.ZONE_5;
            }
        }
        return ScoreType.ZONE_NONE;
    }
}
