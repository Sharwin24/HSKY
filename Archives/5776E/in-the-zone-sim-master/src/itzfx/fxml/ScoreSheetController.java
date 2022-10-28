/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.fxml;

import javafx.fxml.FXML;
import javafx.scene.image.ImageView;
import javafx.scene.text.Text;

/**
 * FXML Controller class. Controls the "ScoreSheet.fxml" file. This updates the
 * fields on the score sheet so that it accurately displays the results of a
 * match.
 *
 * @author Prem Chintalapudi 5776E
 */
public class ScoreSheetController {

    @FXML
    private Text red20;
    @FXML
    private Text blue20;
    @FXML
    private Text red10;
    @FXML
    private Text blue10;
    @FXML
    private Text red5;
    @FXML
    private Text blue5;
    @FXML
    private Text redCones;
    @FXML
    private Text blueCones;
    @FXML
    private Text redStacks;
    @FXML
    private Text blueStacks;
    @FXML
    private ImageView redAuton;
    @FXML
    private ImageView blueAuton;
    @FXML
    private Text redPark;
    @FXML
    private Text bluePark;
    @FXML
    private Text redTotal;
    @FXML
    private Text blueTotal;

    /**
     *
     * @param temp the codes for values to insert into the score sheet. They are
     * listed as follows:
     * <p>
     * Mobile Goals in:
     * </p><p>
     * Red 20 point zone
     * </p><p>
     * Blue 20 point zone
     * </p><p>
     * Red 10 point zone
     * </p><p>
     * Blue 10 point zone
     * </p><p>
     * Red 5 point zone
     * </p><p>
     * Blue 5 point zone
     * </p><p>
     * Red cones stacked
     * </p><p>
     * Blue cones stacked
     * </p><p>
     * Red # of high stacks
     * </p><p>
     * Blue # of high stacks
     * </p><p>
     * Red # of robots parked
     * </p><p>
     * Blue # of robots parked
     * </p><p>
     * Red Autonomous bonus (1 if true)
     * </p><p>
     * Blue Autonomous bonus (1 if true)
     * </p>
     */
    public void update(int[] temp) {
        int[] vals = new int[14];
        if (temp != null) {
            System.arraycopy(temp, 0, vals, 0, temp.length > vals.length ? temp.length : vals.length);
        }
        red20.setText(String.valueOf(vals[0]));
        blue20.setText(String.valueOf(vals[1]));
        red10.setText(String.valueOf(vals[2]));
        blue10.setText(String.valueOf(vals[3]));
        red5.setText(String.valueOf(vals[4]));
        blue5.setText(String.valueOf(vals[5]));
        redCones.setText(String.valueOf(vals[6]));
        blueCones.setText(String.valueOf(vals[7]));
        redStacks.setText(String.valueOf(vals[8]));
        blueStacks.setText(String.valueOf(vals[9]));
        redAuton.setVisible(vals[10] > 0);
        blueAuton.setVisible(vals[10] < 0);
        redPark.setText(String.valueOf(vals[11]));
        bluePark.setText(String.valueOf(vals[12]));
        redTotal.setText(String.valueOf(vals[0] * 20 + vals[2] * 10 + vals[4] * 5 + vals[6] * 2 + vals[8] * 5 + vals[10] * 2 + vals[12] * 10));
        blueTotal.setText(String.valueOf(vals[1] * 20 + vals[3] * 10 + vals[5] * 5 + vals[7] * 2 + vals[9] * 5 + vals[11] * 2 + vals[13] * 10));
    }
}
