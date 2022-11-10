/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

import java.util.function.BiConsumer;

/**
 *
 * @author prem
 */
public interface ModeManager {

    ControlMode getControlMode();

    void setControlMode(ControlMode mode);

    void onControlModeChange(BiConsumer<ControlMode, ControlMode> action);

    void startMatch();

    void stopMatch();

    void play();

    void pause();

    int getTimeRemaining();//In deciseconds
    
    void reset();
    
    void onGameStateChanged(BiConsumer<GameState, GameState> action);
}
