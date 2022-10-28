/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

import itzfx.utils.QuickMafs;
import javafx.application.Platform;
import javafx.beans.binding.Bindings;
import javafx.beans.binding.FloatBinding;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.FloatProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleFloatProperty;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.scene.Node;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.input.MouseEvent;

/**
 * This is the base class for all movable field objects. It defines methods for
 * getting and setting the center of a Mobile, as well as collisions, providing
 * a {@link ContextMenu}, and providing resettability.
 *
 * @author Prem Chintalapudi 5776E
 */
public abstract class Mobile {

    private final FloatProperty centerX;
    private final FloatProperty centerY;

    private final float layoutX;
    private final float layoutY;
    private float initRotate;

    /**
     * Constructs a new Mobile at the specified coordinates with initial rotate
     * 0.
     *
     * @param layoutX the initial x coordinate
     * @param layoutY the initial y coordinate
     */
    protected Mobile(float layoutX, float layoutY) {
        this(layoutX, layoutY, 0);
    }

    /**
     * Constructs a new Mobile at the specified coordinates with the specified
     * initial rotate.
     *
     * @param layoutX the initial x coordinate
     * @param layoutY the initial y coordinate
     * @param initRotate the initial rotate, in degrees
     */
    protected Mobile(float layoutX, float layoutY, float initRotate) {
        centerX = new SimpleFloatProperty();
        centerY = new SimpleFloatProperty();
        this.layoutX = layoutX;
        this.layoutY = layoutY;
        this.initRotate = initRotate;
    }

    /**
     * Creates the properties associated with this Mobile. Subclasses that
     * choose to override this method <b>must</b> call
     * {@code super.registerProperties()} to observe proper behavior relating to
     * centering and context menu display.
     */
    protected void registerProperties() {
        getNode().translateXProperty().bind(translateXBind());
        getNode().translateYProperty().bind(translateYBind());
        getNode().setUserData(this);
        ContextMenu rightClick = new ContextMenu();
        MenuItem delete = new MenuItem("Delete");
        delete.getStyleClass().add("redden");
        delete.setOnAction(e -> vanish());
        rightClick.getItems().add(delete);
        rightClickOptions(rightClick);
        getNode().addEventHandler(MouseEvent.MOUSE_PRESSED, m -> {
            if (m.isSecondaryButtonDown()) {
                rightClick.show(getNode(), m.getScreenX(), m.getScreenY());
            } else {
                rightClick.hide();
            }
        });
        reset();
    }

    /**
     * A method by which subclasses may add options to the {@link ContextMenu}
     * shown when the user right-clicks on this node.
     *
     * @param rightClick the Context Menu to add options to
     */
    protected void rightClickOptions(ContextMenu rightClick) {
    }

    /**
     * A method that returns the binding to which the translateX property of
     * this node will be bound. This binding determines how the center of this
     * Mobile is affected by the methods provided in this class.
     *
     * @return a {@link FloatBinding} that relates the {@link Mobile#centerX}
     * property to the node's translateX property.
     */
    protected FloatBinding translateXBind() {
        return Bindings.createFloatBinding(() -> {
            Node n = getNode();
            return centerX.get() - ((float) n.getBoundsInLocal().getWidth() / 2 + (float) n.getBoundsInLocal().getMinX());
        }, centerX);
    }

    /**
     * A method that returns the binding to which the translateY property of
     * this node will be bound. This binding determines how the center of this
     * Mobile is affected by the methods provided in this class.
     *
     * @return a {@link FloatBinding} that relates the {@link Mobile#centerY}
     * property to the node's translateY property.
     */
    protected FloatBinding translateYBind() {
        return Bindings.createFloatBinding(() -> {
            Node n = getNode();
            return centerY.get() - ((float) n.getBoundsInLocal().getHeight() / 2 + (float) n.getBoundsInLocal().getMinY());
        }, centerY);
    }

    /**
     * Shifts the center of this Mobile by the specified amounts. This is a
     * convenience method for the equivalent calls
     * {@code shiftRight(float x); shiftDown(float y);}.
     *
     * @param x the amount to shift the Mobile right by
     * @param y the amount to shift the Mobile down by
     */
    public final void shiftCenter(float x, float y) {
        centerX.set(centerX.get() + x);
        centerY.set(centerY.get() + y);
    }

    /**
     * Sets the center of this Mobile to the specified coordinates. This is a
     * convenience method for the equivalent calls
     * {@code setX(float x); setY(float y);}.
     *
     * @param x the new x coordinate of the center
     * @param y the new y coordinate of the center
     */
    public final void setCenter(float x, float y) {
        centerX.set(x);
        centerY.set(y);
    }

    /**
     * Sets the center x coordinate of this Mobile.
     *
     * @param x the new x coordinate of the center
     */
    public final void setX(float x) {
        centerX.set(x);
    }

    /**
     * Sets the center y coordinate of this Mobile.
     *
     * @param y the new y coordinate of the center
     */
    public final void setY(float y) {
        centerY.set(y);
    }

    /**
     * Shifts the center of this Mobile to the right by the specified amount.
     *
     * @param x the amount by which to shift this Mobile to the right
     */
    public final void shiftRight(float x) {
        centerX.set(centerX.get() + x);
    }

    /**
     * Shifts the center of this Mobile down by the specified amount.
     *
     * @param y the amount by which to shift this Mobile down
     */
    public final void shiftDown(float y) {
        centerY.set(centerY.get() + y);
    }

    /**
     * Gets the current center x coordinate of this Mobile.
     *
     * @return the center x coordinate
     */
    public final float getCenterX() {
        return (float) centerX.get();
    }

    /**
     * Gets the current center y coordinate of this Mobile.
     *
     * @return the center y coordinate
     */
    public final float getCenterY() {
        return centerY.get();
    }

    /**
     * Gets the {@link FloatProperty} that records the center x position.
     *
     * @return the property maintaining the center x position
     */
    public final FloatProperty centerXProperty() {
        return this.centerX;
    }

    /**
     * Gets the {@link FloatProperty} that records the center y position.
     *
     * @return the property maintaining the center y position
     */
    public final FloatProperty centerYProperty() {
        return this.centerY;
    }

    /**
     * Resets this Mobile. To customize resetting, subclasses may override the
     * {@link Mobile#resetProperties()} method.
     */
    public final void reset() {
        resetProperties();
        centerX.set(layoutX);
        centerY.set(layoutY);
        getNode().setRotate(initRotate);
        reappear();
    }

    /**
     * A customizable method for subclasses to fully reset their properties. The
     * center resetting occurs after this method has been called, so any moving
     * while resetting is allowed.
     */
    protected void resetProperties() {
    }

    /**
     * Makes this Mobile disappear from the field. This also removes any
     * collision effects to prevent invisible collisions. The
     * {@link Mobile#cleanUp()} method offers subclasses the option to release
     * any resources prior to vanishing.
     */
    public final void vanish() {
        Platform.runLater(() -> {
            cleanUp();
            disableCollision();
            getNode().setVisible(false);
            vanished.set(true);
        });
    }

    /**
     * Makes this Mobile reappear on the field. This re-adds collision effects,
     * even if they were temporarily disabled before. To permanently disable
     * collisions for an object, {@link Mobile#permaDisableCollisions()}
     * implementations should remove collisions in a way that this method cannot
     * reinitialize them.
     */
    public final void reappear() {
        Platform.runLater(() -> {
            enableCollision();
            getNode().setVisible(true);
            vanished.set(false);
        });
    }

    private final BooleanProperty vanished = new SimpleBooleanProperty();

    /**
     * Determines if this Mobile is visible on the field.
     *
     * @return true if this Mobile is <b>not</b> visible on the field
     */
    public boolean isVanished() {
        return vanished.get();
    }

    /**
     * The {@link BooleanProperty} that monitors the vanished state of this
     * Mobile.
     *
     * @return the property tracking changes to the vanished state of this
     * Mobile
     */
    public BooleanProperty vanishedProperty() {
        return vanished;
    }

    /**
     * A method called prior to vanishing. It is intended for subclasses to
     * override this method to clean themselves up (e.g. in class {@link Robot},
     * any held mobile goals are released nearest where they were held).
     */
    protected void cleanUp() {
    }

    /**
     * A method that returns the view of this object shown on the field. It may
     * be either FXML-based or based solely on JavaFX code.
     *
     * @return a {@link Node} representing this Mobile
     */
    public abstract Node getNode();

    /**
     * A method that re-enables collision. Implementations often trigger
     * {@link Hitbox#enable()} on the owned hitboxes.
     */
    public abstract void enableCollision();

    /**
     * A method that temporarily disables collision. Implementations often
     * trigger {@link Hitbox#disable()} on the owned hitboxes.
     */
    public abstract void disableCollision();

    /**
     * A method that permanently disables collision. Implementations often
     * unregister their hitboxes with {@link Hitbox#unregister(itzfx.Hitbox)}.
     */
    public abstract void permaDisableCollisions();

    /**
     * A method determining whether or not this Mobile can collide (whether its
     * owned hitboxes are enabled and registered).
     *
     * @return true if the owned hitboxes are <b>both</b> owned and registered
     */
    public abstract boolean canCollide();

    /**
     * An optional listener to limit objects to within the field perimeter.
     * There is an exception to allow objects past a certain distance on the top
     * and the left. This is intended to allow the driver loads to remain
     * outside the field perimeter. It must be attached to both the centerX and
     * centerY properties to function properly.
     *
     * @param radius the radius of the object, so that the center does not get
     * too close to the edge of the field
     * @param limit the property to limit
     * @return a {@link ChangeListener} that can be attached to the centerX and
     * centerY properties of a Mobile.
     */
    protected static ChangeListener<Number> limitToField(float radius, FloatProperty limit) {
        return (ObservableValue<? extends Number> obs, Number old, Number next) -> {
            if (next.floatValue() < radius && next.floatValue() > -50) {
                limit.set((float) radius + (float) Math.random());
            } else if (next.floatValue() + radius > 720) {
                limit.set(720 - (float) radius - (float) Math.random());
            }
        };
    }

    /**
     * Keeps objects out of the 20 point zone. There is an exception allowed
     * past 15/sqrt(2) so that placed mobile goals are not rejected. It must be
     * attached to both the centerX and the centerY properties of this Mobile to
     * function properly.
     *
     * @param radius the radius of this Mobile
     * @return a {@link ChangeListener} that prevents objects from naturally
     * entering the 20 point zone
     */
    protected final ChangeListener<Number> exclude20(float radius) {
        float sqrt2 = (float)Math.sqrt(2);
        return (ObservableValue<? extends Number> obs, Number old, Number next) -> {
            if (getCenterX() - radius < 120 && getCenterY() - getCenterX() > 600 - radius && getCenterY() - getCenterX() < 615 - radius) {
                float dist = lineDistance(getCenterX(), getCenterY(), 1, 600 - radius);
                shiftRight(dist / sqrt2);
                shiftDown(-dist / sqrt2);
            }
            if (getCenterY() - radius < 120 && getCenterX() - getCenterY() > 600 - radius && getCenterX() - getCenterY() < 615 - radius) {
                float dist = lineDistance(getCenterX(), getCenterY(), 1, -600 + radius);
                shiftRight(-dist / sqrt2);
                shiftDown(dist / sqrt2);
            }
        };
    }

    private float lineDistance(float x, float y, float m, float k) {
        return (float) Math.abs(k + m * x - y) * QuickMafs.invSqRoot(1 + QuickMafs.square(m));
    }

    protected void adjustInitRotate(float extra) {
        initRotate += extra;
        getNode().setRotate(getNode().getRotate() + extra);
    }
    
    public void resetRotate() {
        getNode().setRotate(initRotate);
    }
}
