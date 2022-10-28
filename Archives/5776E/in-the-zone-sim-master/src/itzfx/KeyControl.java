/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx;

import java.util.Arrays;
import java.util.Objects;
import java.util.stream.Collectors;
import javafx.scene.input.KeyCode;

/**
 * This class represents a controller format for the user. It determines which
 * keys correspond to actions taken by the controlling robot. Objects of this
 * type are immutable.
 *
 * @author Prem Chintalapudi 5776E
 */
public final class KeyControl {

    private final KeyCode forward, left, backward, right, mogo, autostack, cone, stat, load;

    private static final KeyControl SINGLE, DUAL_1, DUAL_2, QUAD_3, QUAD_4, BLANK;

    static {
        SINGLE = new KeyControl((KeyCode[]) null);
        DUAL_1 = new KeyControl(KeyCode.W, KeyCode.A, KeyCode.S, KeyCode.D, KeyCode.Q, KeyCode.E, KeyCode.X, KeyCode.C, KeyCode.F);
        DUAL_2 = new KeyControl(KeyCode.NUMPAD8, KeyCode.NUMPAD4, KeyCode.NUMPAD5, KeyCode.NUMPAD6, KeyCode.NUMPAD7, KeyCode.NUMPAD9, KeyCode.NUMPAD1, KeyCode.NUMPAD3, KeyCode.NUMPAD2);
        QUAD_3 = new KeyControl(KeyCode.Y, KeyCode.G, KeyCode.H, KeyCode.J, KeyCode.T, KeyCode.U, KeyCode.B, KeyCode.M, KeyCode.N);
        QUAD_4 = new KeyControl(KeyCode.P, KeyCode.L, KeyCode.SEMICOLON, KeyCode.QUOTE, KeyCode.O, KeyCode.OPEN_BRACKET, KeyCode.COMMA, KeyCode.SLASH, KeyCode.PERIOD);
        BLANK = new KeyControl(KeyCode.UNDEFINED, KeyCode.UNDEFINED, KeyCode.UNDEFINED, KeyCode.UNDEFINED, KeyCode.UNDEFINED, KeyCode.UNDEFINED, KeyCode.UNDEFINED, KeyCode.UNDEFINED, KeyCode.UNDEFINED);
    }

    /**
     * Constructs a new KeyControl with the given keys. It automatically fills
     * in defaults if not enough keys are specified, or if the passed array
     * itself is null.
     *
     * @param keys the keys to link to actions
     */
    public KeyControl(KeyCode... keys) {
        if (keys == null) {
            keys = new KeyCode[0];
        }
        KeyCode[] read = new KeyCode[9];
        System.arraycopy(keys, 0, read, 0, keys.length > read.length ? read.length : keys.length);
        forward = read[0] == null ? KeyCode.W : read[0];
        left = read[1] == null ? KeyCode.A : read[1];
        backward = read[2] == null ? KeyCode.S : read[2];
        right = read[3] == null ? KeyCode.D : read[3];
        mogo = read[4] == null ? KeyCode.M : read[4];
        autostack = read[5] == null ? KeyCode.I : read[5];
        cone = read[6] == null ? KeyCode.O : read[6];
        stat = read[7] == null ? KeyCode.K : read[7];
        load = read[8] == null ? KeyCode.L : read[8];
    }

    /**
     * Gets the keys that are linked in this KeyControl to specified actions.
     * The keys are listed in the order of FORWARD, LEFT_TURN, BACKWARD,
     * RIGHT_TURN, MOBILE_GOAL_TOGGLE, AUTOSTACK, CONE_TOGGLE, STATIONARY_GOAL,
     * LOAD_DRIVER LOAD.
     *
     * @return an array of keys that are linked to specified actions
     */
    public KeyCode[] keys() {
        return new KeyCode[]{forward, left, backward, right, mogo, autostack, cone, stat, load};
    }

    /**
     * A predefined set of KeyControls, best suited for multiplayer on a single
     * keyboard. Included are a single-player version and a blank version, to
     * remove controls for the linked robot.
     */
    public static enum Defaults {

        /**
         * A control format for single player on a single keyboard.
         */
        SINGLE(KeyControl.SINGLE),
        /**
         * A control format for player 1 on a keyboard with up to 4 players.
         */
        DUAL_1(KeyControl.DUAL_1),
        /**
         * A control format for player 2 on a keyboard with up to 4 players.
         */
        DUAL_2(KeyControl.DUAL_2),
        /**
         * A control format for player 3 on a keyboard with up to 4 players.
         */
        QUAD_3(KeyControl.QUAD_3),
        /**
         * A control format for player 4 on a keyboard with up to 4 players.
         */
        QUAD_4(KeyControl.QUAD_4),
        /**
         * A blank control format to remove any control of a robot.
         */
        BLANK(KeyControl.BLANK);

        private final KeyControl kc;

        private Defaults(KeyControl kc) {
            this.kc = kc;
        }

        /**
         * Gets the control format for this Default.
         *
         * @return the control format
         */
        public KeyControl getKC() {
            return kc;
        }
    }

    /**
     * Gets a string that can be decoded later for the purposes of file saving.
     *
     * @return a string with all the data of this control format
     */
    public String fileData() {
        return Arrays.stream(keys()).map(k -> k.getName()).collect(Collectors.joining(" "));
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 89 * hash + Objects.hashCode(this.forward);
        hash = 89 * hash + Objects.hashCode(this.left);
        hash = 89 * hash + Objects.hashCode(this.backward);
        hash = 89 * hash + Objects.hashCode(this.right);
        hash = 89 * hash + Objects.hashCode(this.mogo);
        hash = 89 * hash + Objects.hashCode(this.autostack);
        hash = 89 * hash + Objects.hashCode(this.cone);
        hash = 89 * hash + Objects.hashCode(this.stat);
        hash = 89 * hash + Objects.hashCode(this.load);
        return hash;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final KeyControl other = (KeyControl) obj;
        if (this.forward != other.forward) {
            return false;
        }
        if (this.left != other.left) {
            return false;
        }
        if (this.backward != other.backward) {
            return false;
        }
        if (this.right != other.right) {
            return false;
        }
        if (this.mogo != other.mogo) {
            return false;
        }
        if (this.autostack != other.autostack) {
            return false;
        }
        if (this.cone != other.cone) {
            return false;
        }
        if (this.stat != other.stat) {
            return false;
        }
        if (this.load != other.load) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "KeyControl{" + "forward=" + forward + ", left=" + left + ", backward=" + backward + ", right=" + right + ", mogo=" + mogo + ", autostack=" + autostack + ", cone=" + cone + ", stat=" + stat + ", load=" + load + '}';
    }

    /**
     * Converts data from a file into a recognizable control format.
     *
     * @param fileData the data to convert
     * @return the new KeyControl representing a control format
     */
    public static KeyControl getKeyControl(String fileData) {
        String[] data = fileData.split(" ");
        if (data.length > 9) {
            return Defaults.valueOf(data[9]).getKC();
        }
        return new KeyControl(Arrays.stream(data).map(KeyCode::getKeyCode).toArray(KeyCode[]::new));
    }
}
