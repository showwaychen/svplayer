package cxw.cn.svplayer;

/**
 * Created by cxw on 2017/10/23.
 */

public interface IMediaPlayerControl {
    int    start();
    int    pause();
    int    resume();
    int    captureImage();
    int     getDuration();
    int     getCurrentPosition();
    void    seekTo(int pos);
    boolean isPlaying();
    boolean canPause();
    boolean canSeek();

}
