/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.game.objects;

import itzfx.Hitbox;
import itzfx.Mobile;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.layout.StackPane;

/**
 * The class representing a Cone. Cones are represented by the FXML code in
 * "Cone.fxml". They can be stacked and pushed around.
 *
 * @author Prem Chintalapudi 5776E
 */
public final class Cone extends Mobile {

    private final StackPane cone;
    private final Hitbox hitbox;

    private final BooleanProperty stacked;

    /**
     * Creates a new cone at the specified coordinates.
     *
     * @param layoutX the x coordinate the cone is created at
     * @param layoutY the y coordinate the cone is created at
     */
    public Cone(float layoutX, float layoutY) {
        super(layoutX, layoutY);
        try {
            Parent loaded = FXMLLoader.load(Cone.class.getResource("Cone.fxml"));
            cone = new StackPane(loaded);
            hitbox = new Hitbox(15, Hitbox.CollisionType.STRONG, this, 1);
            Hitbox.register(hitbox);
            cone.getChildren().add(hitbox.getVisual());
            stacked = new SimpleBooleanProperty();
            super.registerProperties();
            super.centerXProperty().addListener(Mobile.limitToField(15, super.centerXProperty()));
            super.centerYProperty().addListener(Mobile.limitToField(15, super.centerYProperty()));
            super.centerXProperty().addListener(super.exclude20(15));
            super.centerYProperty().addListener(super.exclude20(15));
        } catch (IOException ex) {
            Logger.getLogger(Cone.class.getName()).log(Level.SEVERE, null, ex);
            throw new RuntimeException(ex);
        }
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
     */
    @Override
    public boolean canCollide() {
        return hitbox.canCollide();
    }

    /**
     * Gets the property determining whether or not this cone is stacked.
     *
     * @return the property monitoring whether or not this cone has been stacked
     */
    public BooleanProperty stackedProperty() {
        return stacked;
    }

    /**
     * Determines whether this cone has been stacked.
     *
     * @return true if this cone has been stacked
     */
    public boolean isStacked() {
        return stacked.get();
    }

    /**
     * Sets the stacked property of this cone to {@code true}.
     */
    public void stack() {
        stacked.set(true);
    }

    /**
     * Sets the stacked property of this cone to {@code false}.
     */
    public void destack() {
        stacked.set(false);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    protected void resetProperties() {
        stacked.set(false);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public Parent getNode() {
        return cone;
    }
}
