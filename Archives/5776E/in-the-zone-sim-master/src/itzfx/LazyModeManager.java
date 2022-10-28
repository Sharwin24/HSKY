/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

import javafx.beans.property.IntegerProperty;

/**
 *
 * @author prem
 */
public interface LazyModeManager extends ModeManager {
    
    IntegerProperty timeRemainingProperty();
    
    @Override
    default int getTimeRemaining() {
        return timeRemainingProperty().get();
    }

    IntegerProperty startingInProperty();

    void mute();
}
