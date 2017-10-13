package cxw.cn.svplayer.example.activity;

import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.NavUtils;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.LogRecord;

import cxw.cn.svplayer.CommonSetting;
import cxw.cn.svplayer.PlayerConstants;
import cxw.cn.svplayer.PlayerEventListener;
import cxw.cn.svplayer.SVideoPlayer;
import cxw.cn.svplayer.example.R;
import cxw.cn.svplayer.example.widget.SurfaceViewRender;
import cxw.cn.svplayer.example.widget.TextureViewRender;

/**
 * Created by cxw on 2017/10/2.
 */

public class VideoPlayerActivity extends AppCompatActivity implements View.OnClickListener, SeekBar.OnSeekBarChangeListener,PlayerEventListener {

    FrameLayout mVideoLayout = null;
    FrameLayout mfl_VideoParent = null;
    SurfaceViewRender mSfvRender = null;
    TextureViewRender mTVRender = null;
    SVideoPlayer mPlayer = null;
    ImageView miv_Pause = null;
    Button mbtn_FullScreen = null;
    SeekBar msb_Seeking = null;
    TextView mtv_CurTime = null;
    TextView mtv_TotalTime = null;
    TextView mtv_playinfo = null;
    public static final String ARG_URL = "url";
    String mPlayUrl = null;

    Handler mHandler = null;
    Timer mTimer = new Timer();
    TimerTask mTimerTask = null;
    public static void active(Context context, String url)
    {
        Intent intent = new Intent(context, VideoPlayerActivity.class);
        intent.putExtra(ARG_URL, url);
        context.startActivity(intent);
    }
    boolean isLandscapeScreen()
    {
        Configuration mConfiguration = this.getResources().getConfiguration();
        int ori = mConfiguration.orientation;
        return (ori == Configuration.ORIENTATION_LANDSCAPE)?true:false;
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_videopaly);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        mPlayUrl = getIntent().getStringExtra(ARG_URL);
        initView();
        initPlayer();
    }
    boolean mDestroy = false;
    @Override
    protected void onDestroy() {

        super.onDestroy();
    }
    @Override
    protected void onPause() {
        super.onPause();
        if (mPlayer == null)
        {
            return ;
        }
        if (isFinishing())
        {
            mDestroy = true;
            mHandler.removeMessages(0);
            SVideoPlayer tmpplay = mPlayer;
            mPlayer = null;
            tmpplay.destroyPlayer();
            mSfvRender = null;
            mTVRender = null;
        }
        else
        {
            mPlayer.pausePlayer();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mPlayer == null)
        {
            return ;
        }
        mPlayer.resumePlayer();
    }
    void initView()
    {
        mVideoLayout = (FrameLayout) findViewById(R.id.videoFrameLayout);
        mfl_VideoParent = (FrameLayout)findViewById(R.id.fl_videoparent);
        miv_Pause = (ImageView) findViewById(R.id.btnPause);
        miv_Pause.setOnClickListener(this);
        mbtn_FullScreen = (Button) findViewById(R.id.btn_fullscreen);
        mbtn_FullScreen.setOnClickListener(this);
        msb_Seeking = (SeekBar)findViewById(R.id.seekBar);
        msb_Seeking.setMax(0);
        msb_Seeking.setProgress(0);
        msb_Seeking.setEnabled(false);
        msb_Seeking.setOnSeekBarChangeListener(this);


        mtv_CurTime = (TextView)findViewById(R.id.tv_curTime);
        mtv_TotalTime = (TextView)findViewById(R.id.tv_totalTime);

        mtv_playinfo = (TextView)findViewById(R.id.tv_playinfo);

    }
    void initPlayer()
    {
        CommonSetting.nativeToggleFFmpegLog(true);
        CommonSetting.nativeSetLogLevel(PlayerConstants.LS_SENSITIVE);

        mSfvRender = new SurfaceViewRender(this);
        mVideoLayout.addView(mSfvRender);
//        mTVRender = new TextureViewRender(this);
//        mVideoLayout.addView(mTVRender);
        mPlayer = new SVideoPlayer();
        mPlayer.setEventlisten(this);
        mPlayer.setRender(mSfvRender.getNativeRender());
        mPlayer.setUrl(mPlayUrl);
        mPlayer.startPlayer();
//
        mPlayer.setAudioPlayerType(PlayerConstants.AudioPlayType_AudioTrack);

        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg)
            {
                if (mPlayer != null)
                {
                    int curposition = mPlayer.getCurrentPosition();
                    msb_Seeking.setProgress(curposition);
                    mtv_CurTime.setText(secToString(curposition));
                    mHandler.sendEmptyMessageDelayed(0, 1000);
                }

            }
        };
    }
    void OnMax()
    {
        if (!isLandscapeScreen())
        {
            mbtn_FullScreen.setBackgroundResource(R.drawable.video_zoom_icon);
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        }
        else
        {
            mbtn_FullScreen.setBackgroundResource(R.drawable.video_full_screen);
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        }
    }
    @Override
    public void onConfigurationChanged(Configuration newConfig) {
//
        RelativeLayout.LayoutParams lp = (RelativeLayout.LayoutParams)mfl_VideoParent.getLayoutParams();
        if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT)//竖屏
        {
            DisplayMetrics metrice = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getMetrics(metrice);

            lp.height =(int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 200, metrice);
            mfl_VideoParent.setLayoutParams(lp);
        }
        else
        {
            WindowManager wm = getWindowManager();
            lp.height = wm.getDefaultDisplay().getHeight();

            mfl_VideoParent.setLayoutParams(lp);
        }
        super.onConfigurationChanged(newConfig);
    }
    boolean m_Pause = false;
    @Override
    public void onClick(View view) {
        int vid = view.getId();
        if (vid == R.id.btnPause)
        {
            if (mPlayer == null)
            {
                return ;
            }
            m_Pause = !m_Pause;
             miv_Pause.setImageResource(m_Pause?R.drawable.play_btn:R.drawable.pause_btn);
            if (m_Pause)
            {
                mPlayer.pausePlayer();
            }
            else
            {
                mPlayer.resumePlayer();
            }
//            if ()
//            mPlayer.startPlayer();
//            mPlayer.muteAudio(true);

        }
        else if (vid == R.id.btn_fullscreen)
        {
            OnMax();
//            mPlayer.muteAudio(false);
        }
    }
    static String secToString(float second) {

        long millis = (long)(second * 1000);
        boolean negative = millis < 0;
        millis = java.lang.Math.abs(millis);

        millis /= 1000;
        int sec = (int) (millis % 60);
        millis /= 60;
        int min = (int) (millis % 60);
        millis /= 60;
        int hours = (int) millis;

        String time;
        DecimalFormat format = (DecimalFormat) NumberFormat.getInstance(Locale.US);
        format.applyPattern("00");

        if (millis > 0) {
            time = (negative ? "-" : "") + hours + ":" + format.format(min) + ":" + format.format(sec);
        } else {
            time = (negative ? "-" : "") + min + ":" + format.format(sec);
        }

        return time;
    }
    @Override
    public void onProgressChanged(SeekBar seekBar, int i, boolean b) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        int seekseconds = seekBar.getProgress();
        mPlayer.seekPlayer(seekseconds);
        mtv_CurTime.setText(secToString(seekseconds));
    }

    @Override
    public void onPlayerEvent(final int eventid, long resdata) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (eventid == PlayerConstants.PE_ReadyToPlay)
                {
                    mtv_playinfo.setText("准备开始播放");
                    msb_Seeking.setEnabled(true);
                    msb_Seeking.setMax(mPlayer.getDuration());
                    mtv_TotalTime.setText(secToString(mPlayer.getDuration()));
                    mHandler.sendEmptyMessageDelayed(0, 1000);

                }
                else if(eventid == PlayerConstants.PE_Opening)
                {
                    mtv_playinfo.setText("打开中..");
                }
                else if (eventid == PlayerConstants.PE_OpenedError)
                {
                    mtv_playinfo.setText("打开失败");
                }
                else if (eventid == PlayerConstants.PE_PlayError)
                {
                    mtv_playinfo.setText("播放出错");
                }
                else if (eventid == PlayerConstants.PE_PlaySeekFailed)
                {
                    mtv_playinfo.setText("定位失败");
                }
                else if (eventid == PlayerConstants.PE_SeekFinished)
                {
                    mtv_playinfo.setText("定位成功");
                }
                else if (eventid == PlayerConstants.PE_Playing)
                {
                    mtv_playinfo.setText("");
                }
                else if (eventid == PlayerConstants.PE_StopWithEof)
                {
                    mtv_playinfo.setText("播放结束");
                }
            }
        });
    }
}
