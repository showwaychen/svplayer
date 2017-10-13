package cxw.cn.svplayer;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

/**
 * Created by cxw on 2017/9/17.
 */

public class AudioTrackPlay {

    static final  String TAG = AudioTrackPlay.class.getCanonicalName();
    private static final int        AUDIO_FORMATE      = AudioFormat.ENCODING_PCM_16BIT;
    public static final Object      lock1              = new Object();
    private AudioTrack mAudioTrack        = null;
    private int                     mMinBuffSize;
    private int                     mAudioChannels;
    private int                     mAudioChannelCount;
    private int                     mAudioSamplerate;
    int InitAudioTrack(int channels, int sampleRate)
    {
        mAudioSamplerate = sampleRate;
        mAudioChannelCount = channels;

        if (mAudioChannelCount > 1) {
            mAudioChannels = AudioFormat.CHANNEL_OUT_STEREO;
        } else {
            mAudioChannels = AudioFormat.CHANNEL_OUT_MONO;
        }

        mMinBuffSize = AudioTrack.getMinBufferSize(mAudioSamplerate, mAudioChannels, AUDIO_FORMATE);

        synchronized (lock1) {
            try {
                mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, mAudioSamplerate,
                        mAudioChannels, AUDIO_FORMATE, mMinBuffSize * 2, AudioTrack.MODE_STREAM);
                // int rate = AudioTrack.getNativeOutputSampleRate(AudioManager.STREAM_MUSIC);
                // mAudioTrack.setPlaybackRate(rate * 2);
                mAudioTrack.play();
//                sonic = new Sonic(mAudioSamplerate, mAudioChannelCount);
//                sonic.setSpeed(2);
            } catch (Exception e) {
                Log.e(TAG, "can not create audiotrack");
                mAudioTrack = null;
                return -1;
            }
        }

        return (mMinBuffSize * 2);
    }
    int AudioPlay(byte[] data, int bytes)
    {
        synchronized (lock1) {
            if (mAudioTrack != null) {
//                if (speed_up == 1) {
//                    byte modifiedSamples[] = new byte[4096];
//                    sonic.putBytes(data, bytes);
//                    int available = sonic.availableBytes();
//                    if (available > 0) {
//                        if (modifiedSamples.length < available) {
//                            modifiedSamples = new byte[available * 2];
//                        }
//                        sonic.receiveBytes(modifiedSamples, available);
//                        mAudioTrack.write(modifiedSamples, 0, available);
//                    }
//                } else {
                    mAudioTrack.write(data, 0, bytes);
//                }
            }
        }
        return 0;
    }
}
