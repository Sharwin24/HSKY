/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.game.objects;

import itzfx.Hitbox;
import itzfx.fxml.Field;
import java.io.IOException;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Point2D;
import javafx.scene.Node;
import javafx.scene.layout.StackPane;

/**
 * The class representing a Loader. Loaders are represented by the FXML code in
 * the file "Loader.fxml". Driver loads spawn on loaders to bring them into
 * play.
 *
 * @author Prem Chintalapudi 5776E
 */
public class Loader {

    private final StackPane loader;

    private final boolean red;

    private Hitbox hitbox;

    /**
     * Constructs a new loader with center at the specified coordinates.
     *
     * @param layoutX the x coordinate of the center
     * @param layoutY the y coordinate of the center
     * @param red whether or not this loader is on the red alliance or the blue
     * alliance
     */
    public Loader(float layoutX, float layoutY, boolean red) {
        try {
            loader = new StackPane();
            loader.getChildren().add(FXMLLoader.load(Loader.class.getResource("Loader.fxml")));
            loader.setTranslateX(layoutX);
            loader.setTranslateY(layoutY);
            hitbox = new Hitbox(15, Hitbox.CollisionType.STRONG, loader, Float.POSITIVE_INFINITY);
            hitbox.setXSupplier(() -> (float) loader.getTranslateX());
            hitbox.setYSupplier(() -> (float) loader.getTranslateY());
            Hitbox.register(hitbox);
            loader.getChildren().add(hitbox.getVisual());
            this.red = red;
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    /**
     * Loads a driver load {@link Cone}, if present, onto this loader.
     *
     * @return a cone loaded onto this loader, if present
     */
    public Cone load() {
        if (!Field.getOwner(this).hasCone(this)) {
            Cone load = Field.getOwner(this).getLoadableCone(red);
            if (load != null) {
                Platform.runLater(() -> {
                    load.disableCollision();
                    load.setCenter((float) getCenter().getX(), (float) getCenter().getY());
                });
                return load;
            }
        }
        return null;
    }

    /**
     * Gets the center coordinates of this loader.
     *
     * @return the center of this loader, as a point.
     */
    public Point2D getCenter() {
        return new Point2D(red ? 15 : 302, red ? 302 : 15);
    }

    /**
     * Gets a visual representation of this loader. This is loaded from the FXML
     * file "Loader.fxml".
     *
     * @return a visual representation of this loader.
     */
    public Node getNode() {
        return loader;
    }
}
