/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.controller;

import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;
import javafx.css.PseudoClass;
import javafx.fxml.FXML;
import javafx.scene.Group;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.StackPane;
import javafx.scene.shape.Rectangle;

/**
 * FXML Controller class. Controls "Keyboard.fxml". Runs mouse-clicking and
 * highlighting of keys during KeyControl construction. Every key has been
 * sheathed in its own rectangle (this took quite a while).
 *
 * @author Prem Chintalapudi 5776E
 */
public class Keyboard {

    @FXML
    private AnchorPane root;

    @FXML
    private Group keyGroup;

    private Map<KeyCode, Rectangle> mappings;

    @FXML
    private void initialize() {
        root.setUserData(this);
        List<KeyCode> row1 = Arrays.asList(KeyCode.ESCAPE, KeyCode.F1, KeyCode.F2, KeyCode.F3, KeyCode.F4,
                KeyCode.F5, KeyCode.F6, KeyCode.F7, KeyCode.F8, KeyCode.F9, KeyCode.F10,
                KeyCode.F11, KeyCode.F12, KeyCode.PRINTSCREEN, KeyCode.SCROLL_LOCK, KeyCode.PAUSE);
        List<KeyCode> row2 = Arrays.asList(KeyCode.BACK_QUOTE, KeyCode.DIGIT1, KeyCode.DIGIT2, KeyCode.DIGIT3,
                KeyCode.DIGIT4, KeyCode.DIGIT5, KeyCode.DIGIT6, KeyCode.DIGIT7, KeyCode.DIGIT8,
                KeyCode.DIGIT9, KeyCode.DIGIT0, KeyCode.MINUS, KeyCode.EQUALS, KeyCode.BACK_SPACE,
                KeyCode.INSERT, KeyCode.HOME, KeyCode.PAGE_UP, KeyCode.NUM_LOCK,
                KeyCode.DIVIDE, KeyCode.MULTIPLY, KeyCode.SUBTRACT);
        List<KeyCode> row3 = Arrays.asList(KeyCode.TAB, KeyCode.Q, KeyCode.W, KeyCode.E, KeyCode.R, KeyCode.T,
                KeyCode.Y, KeyCode.U, KeyCode.I, KeyCode.O, KeyCode.P, KeyCode.OPEN_BRACKET,
                KeyCode.CLOSE_BRACKET, KeyCode.BACK_SLASH, KeyCode.DELETE, KeyCode.END,
                KeyCode.PAGE_DOWN, KeyCode.NUMPAD7, KeyCode.NUMPAD8, KeyCode.NUMPAD9,
                KeyCode.PLUS);
        List<KeyCode> row4 = Arrays.asList(KeyCode.CAPS, KeyCode.A, KeyCode.S, KeyCode.D, KeyCode.F, KeyCode.G,
                KeyCode.H, KeyCode.J, KeyCode.K, KeyCode.L, KeyCode.SEMICOLON, KeyCode.QUOTE,
                KeyCode.ENTER, KeyCode.NUMPAD4, KeyCode.NUMPAD5, KeyCode.NUMPAD6);
        List<KeyCode> row5 = Arrays.asList(KeyCode.SHIFT, KeyCode.Z, KeyCode.X, KeyCode.C, KeyCode.V, KeyCode.B,
                KeyCode.N, KeyCode.M, KeyCode.COMMA, KeyCode.PERIOD, KeyCode.SLASH,
                KeyCode.AGAIN, KeyCode.UP, KeyCode.NUMPAD1, KeyCode.NUMPAD2, KeyCode.NUMPAD3,
                KeyCode.COLORED_KEY_1);
        List<KeyCode> row6 = Arrays.asList(KeyCode.CONTROL, KeyCode.META, KeyCode.ALT, KeyCode.SPACE, KeyCode.ROMAN_CHARACTERS,
                KeyCode.JAPANESE_HIRAGANA, KeyCode.ALPHANUMERIC, KeyCode.ALL_CANDIDATES, KeyCode.LEFT,
                KeyCode.DOWN, KeyCode.RIGHT, KeyCode.NUMPAD0, KeyCode.DECIMAL);
        Iterator<KeyCode> it = Arrays.asList(row1, row2, row3, row4, row5, row6).stream().flatMap(r -> r.stream()).iterator();
        mappings = keyGroup.getChildren().stream().sequential().map(StackPane.class::cast).map(StackPane::getChildren)
                .map(l -> l.get(0)).filter(Rectangle.class::isInstance).map(Rectangle.class::cast)
                .collect(Collectors.toMap(r -> it.next(), Function.identity()));
        mappings.entrySet().stream().forEach(e -> e.getValue().addEventHandler(MouseEvent.MOUSE_PRESSED, m -> {
            m.consume();
            ((BorderPane) root.getParent()).getLeft().fireEvent(new KeyEvent(null, null, KeyEvent.KEY_PRESSED, null, null, e.getKey(), false, false, false, false));
        }));
    }

    private static final PseudoClass SELECTED = PseudoClass.getPseudoClass("selected");
    private static final PseudoClass SAVED = PseudoClass.getPseudoClass("saved");

    private Rectangle selected;

    /**
     * Removes the indication that this key has been selected.
     *
     * @param k the KeyCode of the key to remove
     */
    public void remove(KeyCode k) {
        Rectangle r = mappings.get(k);
        if (r != null) {
            r.pseudoClassStateChanged(SELECTED, false);
            r.pseudoClassStateChanged(SAVED, false);
            r.setMouseTransparent(false);
            if (r == selected) {
                deselect();
            }
        }
    }

    /**
     * Highlights a key to indicate that this key has focus.
     *
     * @param k the KeyCode of the key to highlight
     */
    public void selected(KeyCode k) {
        deselect();
        selected = mappings.get(k);
        if (selected != null) {
            selected.pseudoClassStateChanged(SELECTED, true);
            selected.setMouseTransparent(true);
        }
    }

    /**
     * Hides the currently focused key (indicating that focus has moved away
     * from it).
     */
    public void deselect() {
        if (selected != null) {
            selected.pseudoClassStateChanged(SELECTED, false);
            selected.setMouseTransparent(false);
            selected = null;
        }
    }
    
    public void disable(KeyCode k) {
        Rectangle r = mappings.get(k);
        if (r != null) {
            if (r == selected) {
                deselect();
            }
            r.setDisable(true);
        }
    }
    
    public void enable(KeyCode k) {
        Rectangle r;
        if ((r = mappings.get(k)) != null) {
            r.setDisable(false);
        }
    }

    /**
     * Fills a key with a less opaque white fill, indicating that it has been
     * saved as a bind to a function.
     */
    public void save() {
        if (selected != null) {
            selected.pseudoClassStateChanged(SAVED, true);
            Rectangle r = selected;
            deselect();
            r.setMouseTransparent(true);
        }
    }
}
