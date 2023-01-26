/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml;

import javafx.fxml.FXML;
import javafx.scene.text.Text;

/**
 * FXML Controller class. Controls the "ScoreSheet.fxml" file. This updates the
 * fields on the score sheet so that it accurately displays the results of a
 * match.
 *
 * @author Prem Chintalapudi 5776E
 */
public class SkillsScoreSheetController {

    @FXML
    private Text skills20;
    @FXML
    private Text skills10;
    @FXML
    private Text skills5;
    @FXML
    private Text skillsCones;
    @FXML
    private Text skillsPark;
    @FXML
    private Text skillsTotal;

    public void update(int[] temp) {
        int[] vals = new int[5];
        if (temp != null) {
            System.arraycopy(temp, 0, vals, 0, temp.length > vals.length ? temp.length : vals.length);
        }
        skills20.setText(String.valueOf(vals[0]));
        skills10.setText(String.valueOf(vals[1]));
        skills5.setText(String.valueOf(vals[2]));
        skillsCones.setText(String.valueOf(vals[3]));
        skillsPark.setText(String.valueOf(vals[4]));
        skillsTotal.setText(String.valueOf(vals[0] * 20 + vals[1] * 10 + vals[2] * 5 + vals[3] * 2 + vals[4] * 2));
    }
}
