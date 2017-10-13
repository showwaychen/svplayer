package cxw.cn.svplayer;

/**
 * Created by cxw on 2017/9/17.
 */

public class PlayerConstants {
    public static final  String VersionCode = "1.1";
    //proerty set key
    public static final int  KeySetIslive = 1;

    //audio player type
    public static final int AudioPlayType_AudioTrack = 1;

    //player state

    //player event
    public static final int PE_None = 0;
    public static final int PE_Opening = 1;
    public static final int PE_Opened = 2;
    public static final int PE_OpenedError = 3;
    public static final int PE_Buffering = 4;
    public static final int PE_ReadyToPlay = 5;
    public static final int PE_Playing = 6;
    public static final int PE_Record_Fail = 7;
    public static final int PE_Recording = 8;
    public static final int PE_Snapshot_Sus = 9;
    public static final int PE_Snapshot_Fail = 10;
    public static final int PE_PlayError = 11;
    public static final int PE_PlaySeekFailed = 12;
    public static final int     PE_SeekFinished = 13;
    public static final int     PE_StopWithEof = 14;
    //log level
    public  static final int LS_SENSITIVE = 0;
    public  static final int LS_VERBOSE = 1;
    public  static final int LS_INFO = 2;
    public  static final int LS_WARNING = 3;
    public  static final int LS_ERROR = 4;
    public  static final int LS_NONE = 5;

}
