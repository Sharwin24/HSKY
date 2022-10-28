/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml.tabs;

import javafx.fxml.FXML;
import javafx.scene.text.Text;

/**
 * FXML Controller class
 *
 * @author prem
 */
public class KeyController {
    
    @FXML
    private Text key;
    
    public void setText(String text) {
        text = text == null ? "[?]" : text;
        key.setText(text.length() > 3 ? "[?]" : text);
    }
}
