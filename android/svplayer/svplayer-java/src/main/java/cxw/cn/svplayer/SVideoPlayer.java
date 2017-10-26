package cxw.cn.svplayer;


import java.util.EventListener;

/**
 * Created by cxw on 2017/9/17.
 */

public class SVideoPlayer {

    native long  newNativeObject();
    native void nativeSetUrl(String url);
    native int nativeSetupAudioPlay(int type);
    native void nativePropertySetInt(int key, int value);
    native int nativeStartPlayer();
    native int nativePausePlayer();
    native int nativeResumePlayer();
    native int nativeStopPlayer();
    native int nativeSeekPlayer(int seconds);
    native int nativeMuteAudio(boolean bmute);
    native int nativeGetState();
    native int nativeGetCurrentPositon();
    native int nativeGetDuration();
    native boolean nativeIsPlaying();

    native  void nativeSetVideoRender(long render);
    native void nativeSetAudioPlayer(int ntype);
    native  int nativeCaptureImage(String filepath);
    native void nativeDestroy();


    static
    {
        System.loadLibrary("svplayer");
    }
    long m_nativeObject = 0;
    String m_url;
    PlayerEventListener m_EventObsever = null;
    public SVideoPlayer()
    {
        m_nativeObject = newNativeObject();
    }
    public void setUrl(String surl)
    {
        m_url = surl;
        nativeSetUrl(m_url);
    }

    public void setRender(NativeRender render)
    {
        if (render != null)
        {
            nativeSetVideoRender(render.getNativeObject());
        }
    }
    public void setAudioPlayerType(int type)
    {
        if (type == PlayerConstants.AudioPlayType_AudioTrack)
        {
            nativeSetAudioPlayer(type);
        }
    }
    public int startPlayer()
    {
        int ret = nativeStartPlayer();
        return ret;
    }

    public int pausePlayer()
    {
        int ret = nativePausePlayer();
        return ret;
    }
    public int resumePlayer()
    {
        int ret = nativeResumePlayer();
        return ret;
    }

    public int stopPlayer()
    {
        int ret = nativeStopPlayer();
        return ret;
    }

  public void propertySetInt(int key, int value)
  {

  }
    public int seekPlayer(int seconds)
    {
        int ret = nativeSeekPlayer(seconds);
        return ret;
    }
    public int getState()
    {
        return nativeGetState();
    }



    public void muteAudio(boolean bMute)
    {
        nativeMuteAudio(bMute);
    }
    public int getDuration()
    {
        return nativeGetDuration();
    }
    public int getCurrentPosition()
    {
        return nativeGetCurrentPositon();
    }

    public boolean isPlaying()
    {
        return nativeIsPlaying();
    }
    public int captureImage(String filename)
    {
        return nativeCaptureImage(filename);
    }
    public void destroyPlayer()
    {
        stopPlayer();
        nativeDestroy();
        m_nativeObject = 0;
    }
    public void setEventlisten(PlayerEventListener el)
    {
        m_EventObsever = el;
    }
    //native call
    private void evenCallBack(int eventid, long resdata)
    {

        if (m_EventObsever != null)
        {
            m_EventObsever.onPlayerEvent(eventid, resdata);
        }
    }




}
