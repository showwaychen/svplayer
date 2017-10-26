package cxw.cn.svplayer;

import android.view.Surface;

/**
 * Created by user on 2017/10/25.
 */
//wrapper of svideopalyer
public class SMediaPlayer {
    SVideoPlayer mPlayer = null;
    NativeRender mRender = null;
    public SMediaPlayer()
    {
        mPlayer = new SVideoPlayer();
        mRender = new NativeRender();
        mPlayer.setRender(mRender);
    }

    public void setUrl(String surl)
    {
        mPlayer.setUrl(surl);
    }

    public void setDisplay(Surface surface)
    {
        if (surface == null)
        {
            mRender.stopRendering();
        }else
        {
            mRender.startRender(surface);
        }
    }
    public void resizeDisplay(int width, int height)
    {
        mRender.requestResize(width, height);
    }
    public void setAudioPlayerType(int type)
    {
        mPlayer.setAudioPlayerType(type);
    }
    public int startPlayer()
    {
        return mPlayer.startPlayer();
    }

    public int pausePlayer()
    {
        return mPlayer.pausePlayer();
    }
    public int resumePlayer()
    {
        return mPlayer.resumePlayer();
    }

    public int stopPlayer()
    {
        mRender.stopRendering();
        return  mPlayer.stopPlayer();
    }

    public void propertySetInt(int key, int value)
    {
        mPlayer.propertySetInt(key, value);
    }
    public int seekPlayer(int seconds)
    {
       return mPlayer.seekPlayer(seconds);
    }
    public int getState()
    {
        return mPlayer.getState();
    }

    public void muteAudio(boolean bMute)
    {
        mPlayer.muteAudio(bMute);
    }
    public int getDuration()
    {
        return mPlayer.getDuration();
    }
    public int getCurrentPosition()
    {
        return mPlayer.getCurrentPosition();
    }

    public boolean isPlaying()
    {
        return mPlayer.isPlaying();
    }
    public int captureImage(String filename)
    {
        return mPlayer.captureImage(filename);
    }
    public void destroyPlayer()
    {
        mRender.destroyRender();
        mRender = null;
       mPlayer.destroyPlayer();
        mPlayer = null;
    }
    public void setEventlisten(PlayerEventListener el)
    {
        mPlayer.setEventlisten(el);
    }
}
