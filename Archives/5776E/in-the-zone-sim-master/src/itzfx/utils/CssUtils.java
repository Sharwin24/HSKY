/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.utils;

import java.time.LocalTime;
import java.time.temporal.ChronoUnit;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;

/**
 *
 * @author prem
 */
public class CssUtils {

    private static final ObjectProperty<String> STYLESHEET = new SimpleObjectProperty<>("/itzfx/fxml/css/default-mode.bss");
    private static final Map<String, String> STYLESHEETS = new HashMap<>();
    private static final String FIELD_COLORS = "/itzfx/fxml/css/global.bss";

    static {
        STYLESHEETS.put("default", "/itzfx/fxml/css/default-mode.bss");
        STYLESHEETS.put("dark", "/itzfx/fxml/css/dark-mode.bss");
    }

    private static final Thread THEME_AUTO_SWITCHER = new Thread(() -> {
        while (true) {
            if (LocalTime.now().getHour() < 22 && LocalTime.now().getHour() > 5) {
                switchStyleSheet("default");
                long a = Math.abs(LocalTime.now().until(LocalTime.of(22, 0), ChronoUnit.MILLIS));
                synchronized (STYLESHEET) {
                    try {
                        STYLESHEET.wait(a);
                    } catch (InterruptedException ex) {
                    }
                }
            } else {
                switchStyleSheet("dark");
                long a = Math.abs(LocalTime.now().until(LocalTime.of(6, 0), ChronoUnit.MILLIS));
                synchronized(STYLESHEET) {
                    try {
                        STYLESHEET.wait(a);
                    } catch (InterruptedException ex) {
                    }
                }
            }

        }
    });

    static {
        THEME_AUTO_SWITCHER.setDaemon(true);
        THEME_AUTO_SWITCHER.start();
    }

    public static void switchStyleSheet(String key) {
        STYLESHEET.set(STYLESHEETS.getOrDefault(key, "/itzfx/fxml/css/global.bss"));
    }

    public static void addStyleSheet(Parent p) {
        p.getStylesheets().addAll(FIELD_COLORS, STYLESHEET.get());
        STYLESHEET.addListener((o, b, s) -> {
            p.getStylesheets().remove(b);
            p.getStylesheets().add(s);
        });
    }

    public static void addStyleSheet(Scene scene) {
        scene.getStylesheets().addAll(FIELD_COLORS, STYLESHEET.get());
        STYLESHEET.addListener((o, b, s) -> {
            scene.getStylesheets().remove(b);
            scene.getStylesheets().add(s);
        });
    }

    public static void styleDialog(Dialog d) {
        addStyleSheet(d.getDialogPane());
        d.getDialogPane().getStyleClass().add("default-background");
        Optional.ofNullable(d.getDialogPane().lookupButton(ButtonType.CANCEL)).map(Node::getStyleClass).ifPresent(c -> c.add("cancel-button"));
        Optional.ofNullable(d.getDialogPane().lookupButton(ButtonType.YES)).map(Node::getStyleClass).ifPresent(c -> c.add("yes-button"));
        Optional.ofNullable(d.getDialogPane().lookupButton(ButtonType.NO)).map(Node::getStyleClass).ifPresent(c -> c.add("no-button"));
    }
}
