/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.game.objects;

import itzfx.Hitbox;
import itzfx.Mobile;
import itzfx.scoring.ScoreType;
import itzfx.scoring.ScoreReport;
import itzfx.scoring.Scoreable;
import java.io.IOException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.collections.FXCollections;
import javafx.collections.ListChangeListener;
import javafx.collections.ObservableList;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.layout.Pane;
import javafx.scene.layout.StackPane;
import javafx.scene.text.Text;

/**
 *
 * @author Prem Chintalapudi 5776E
 */
public final class StationaryGoal implements Scoreable {

    private final StackPane statGoal;

    private final ObservableList<Cone> stacked;
    private final IntegerProperty countModifier;
    private final Hitbox hitbox;

    private final boolean red;

    private final ScoreReport sr;

    /**
     *
     * @param layoutX
     * @param layoutY
     * @param red
     */
    public StationaryGoal(float layoutX, float layoutY, boolean red) {
        this.stacked = FXCollections.observableList(new ArrayList<>());
        this.countModifier = new SimpleIntegerProperty();
        Text stackLabel = new Text();
        statGoal = new StackPane();
        ContextMenu rightClick = rightClick();
        statGoal.setOnMousePressed(m -> {
            if (m.isSecondaryButtonDown()) {
                rightClick.show(statGoal, m.getScreenX(), m.getScreenY());
            } else {
                rightClick.hide();
            }
        });
        stackLabel.visibleProperty().bind(Bindings.createBooleanBinding(() -> stacked.size() + countModifier.get() > 0, stacked, countModifier));
        stackLabel.textProperty().bind(Bindings.createStringBinding(() -> String.valueOf(stacked.size() + countModifier.intValue()), stacked, countModifier));
        stackLabel.getStyleClass().add("stack-label");
        this.hitbox = new Hitbox(12.5f, Hitbox.CollisionType.STRONG, statGoal, Float.POSITIVE_INFINITY);
        Hitbox.register(hitbox);
        try {
            statGoal.getChildren().add(FXMLLoader.load(StationaryGoal.class.getResource("StationaryGoal.fxml")));
            Pane cone = FXMLLoader.load(StationaryGoal.class.getResource("StackedCone.fxml"));
            cone.visibleProperty().bind(stackLabel.visibleProperty());
            statGoal.getChildren().addAll(cone, stackLabel);
        } catch (IOException ex) {
            Logger.getLogger(StationaryGoal.class.getName()).log(Level.SEVERE, null, ex);
        }
        statGoal.getChildren().add(hitbox.getVisual());
        statGoal.setTranslateX(layoutX - 12.5);
        statGoal.setTranslateY(layoutY - 12.5);
        stacked.addListener((ListChangeListener.Change<? extends Cone> change) -> {
            change.next();
            if (change.wasAdded()) {
                change.getAddedSubList().forEach(c -> c.stack());
            } else if (change.wasRemoved()) {
                change.getRemoved().forEach(c -> c.destack());
            }
        });
        this.red = red;
        sr = new ScoreReport(this);
        sr.setScoreType(ScoreType.STAT_GOAL);
    }

    private ContextMenu rightClick() {
        ContextMenu rightClick = new ContextMenu();
        MenuItem stack = new MenuItem("Stack");
        stack.setOnAction(e -> countModifier.set(countModifier.get() + 1));
        rightClick.getItems().add(stack);
        MenuItem destack = new MenuItem("Destack");
        destack.setOnAction(e -> countModifier.set(countModifier.get() + stacked.size() > 0 ? countModifier.get() - 1 : -stacked.size()));
        rightClick.getItems().add(destack);
        return rightClick;
    }

    /**
     * Gets the {@link ScoreReport score reporter} that will be updated when
     * this stationary goal is scored.
     *
     * @return this stationary goal's score reporter
     */
    public ScoreReport getReporter() {
        return sr;
    }

    /**
     * "Stacks" a {@link Cone} on top of this stationary goal. In reality, the
     * given cone is supposed to be vanished, while the count displayed on top
     * of this stationary goal is incremented.
     *
     * @param cone the cone to stack
     */
    public void stack(Cone cone) {
        stacked.add(cone);
    }

    /**
     * "Destacks" a {@link Cone} from this stationary goal. Removes and returns
     * a cone that was placed on top of this stationary goal.
     *
     * @return the cone that was destacked.
     */
    public Cone destack() {
        return stacked.size() > 0 ? stacked.remove(0) : null;
    }

    /**
     * Resets this Stationary Goal. Any stacked {@link Cone cones} are
     * destacked, before having their own {@link Mobile#reset reset} methods
     * called.
     */
    public void reset() {
        Platform.runLater(() -> {
            while (stacked.size() > 0) {
                destack().reset();
            }
            countModifier.set(0);
        });
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public boolean isRed() {
        return red;
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int score() {
        return stacked.size() * 2;
    }

    /**
     * A method that returns the view of this object shown on the field. It may
     * be either FXML-based or based solely on JavaFX code.
     *
     * @return a {@link Node} representing this Stationary Goal
     */
    public Node getNode() {
        return statGoal;
    }
}
