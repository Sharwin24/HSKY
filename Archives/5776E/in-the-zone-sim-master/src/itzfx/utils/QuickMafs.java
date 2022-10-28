/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.utils;

/**
 *
 * @author prem
 */
public class QuickMafs {

    public static float invSqRoot(float x) {
        float xhalf = 0.5f * x;
//        long i = Double.floatToLongBits(x);
//        i = 0x5fe6ec85e7de30daL - (i >> 1);
//        x = Double.longBitsToDouble(i);
        x = Float.intBitsToFloat(0x5f3759df - (Float.floatToIntBits(x) >> 1));
        x *= (1.5f - xhalf * x * x);
        return x;
//        return (float) (1 / Math.sqrt(x));
    }

    public static float square(float d) {
        return d * d;
    }
}
