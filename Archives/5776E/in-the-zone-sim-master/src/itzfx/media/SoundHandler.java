/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package itzfx.media;

import java.net.URISyntaxException;
import java.net.URL;
import java.util.EnumMap;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;
import javafx.util.Duration;

/**
 *
 * @author prem
 */
public class SoundHandler implements AutoCloseable {

    private final EnumMap<Sounds, MediaPlayer> soundPlayers;

    public SoundHandler() {
        soundPlayers = new EnumMap<>(Sounds.class);
        soundPlayers.put(Sounds.PAUSED, generateMediaPlayer(retrieveSoundFile("Pause.wav"), Duration.seconds(3)));
        soundPlayers.put(Sounds.END, generateMediaPlayer(retrieveSoundFile("Stop.wav"), Duration.seconds(3)));
        soundPlayers.put(Sounds.START, generateMediaPlayer(retrieveSoundFile("Start.wav"), Duration.seconds(3)));
        soundPlayers.put(Sounds.WARNING, generateMediaPlayer(retrieveSoundFile("Warning.wav"), Duration.seconds(3)));
    }

    private URL retrieveSoundFile(String fileName) {
        return SoundHandler.class.getResource("/itzfx/media/sound/" + fileName);
    }

    private MediaPlayer generateMediaPlayer(URL url, Duration stopTime) {
        try {
            MediaPlayer mp = new MediaPlayer(new Media(url.toURI().toString()));
            mp.setStopTime(stopTime);
            return mp;
        } catch (URISyntaxException ex) {
            return null;
        }
    }

    public void play(Sounds sound) {
        MediaPlayer mp = soundPlayers.get(sound);
        mp.seek(Duration.ZERO);
        mp.play();
    }

    @Override
    public void close() {
        soundPlayers.values().forEach(MediaPlayer::dispose);
    }
    
    public void mute() {
        soundPlayers.values().forEach(mp -> mp.setMute(true));
    }
    
    public void unmute() {
        soundPlayers.values().forEach(mp -> mp.setMute(false));
    }
}
