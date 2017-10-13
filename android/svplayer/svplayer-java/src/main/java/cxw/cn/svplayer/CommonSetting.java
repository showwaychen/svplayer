package cxw.cn.svplayer;

/**
 * Created by cxw on 2017/10/1.
 */

public class CommonSetting {
    static
    {
        System.loadLibrary("svplayer");
    }
    public  static native void nativeSetLogLevel(int level);
    public static native  void nativeToggleFFmpegLog(boolean enable);
}
