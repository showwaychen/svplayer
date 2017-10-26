package cxw.cn.svplayer.example.widget;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageButton;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Locale;

import cxw.cn.svplayer.IMediaPlayerControl;
import cxw.cn.svplayer.PlayerConstants;
import cxw.cn.svplayer.example.R;

/**
 * Created by cxw on 2017/10/23.
 */

public class MediaPlayerControlView extends FrameLayout implements  View.OnClickListener, SeekBar.OnSeekBarChangeListener{

    View mInterView = null;
    ImageView miv_Pause = null;
    Button mbtn_FullScreen = null;
    SeekBar msb_Seeking = null;
    TextView mtv_CurTime = null;
    TextView mtv_TotalTime = null;
    TextView mtv_playinfo = null;

    IMediaPlayerControl mediaPlayerControler = null;
    boolean m_Pause = false;
    Handler mHandler = null;
    public MediaPlayerControlView(Context context) {
        super(context);
        init();
    }
    public MediaPlayerControlView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }
    public MediaPlayerControlView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }
    void init()
    {
        mInterView = inflate(getContext(), R.layout.widget_mediaplayercontrol, this);
        miv_Pause = (ImageButton)mInterView.findViewById(R.id.btnPause);
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

        findViewById(R.id.tv_captureimage).setOnClickListener(this);
        mHandler = new Handler() {
            @Override
            public void handleMessage(Message msg)
            {
                if (mediaPlayerControler != null) {
                    if ( mediaPlayerControler.isPlaying())
                    {

                        int curposition = mediaPlayerControler.getCurrentPosition();
                        msb_Seeking.setProgress(curposition);
                        mtv_CurTime.setText(secToString(curposition));
                    }
                    else
                    {
                        return ;
                    }
                }
                mHandler.sendEmptyMessageDelayed(0, 1000);
            }
        };
    }

    public void setMediaControler(IMediaPlayerControl controler)
    {
        mediaPlayerControler = controler;
    }
    @Override
    public void onClick(View v) {
        if (mediaPlayerControler == null)
        {
            return ;
        }
        int vid = v.getId();
        if (vid == R.id.btnPause)
        {
            m_Pause = !m_Pause;
            miv_Pause.setImageResource(m_Pause?R.drawable.play_btn:R.drawable.pause_btn);
            if (m_Pause)
            {
                mediaPlayerControler.pause();
            }
            else
            {
                mediaPlayerControler.resume();
            }

        }
        else if (vid == R.id.btn_fullscreen)
        {
            OnMax();
        }
        else if (vid == R.id.tv_captureimage)
        {
            if (mediaPlayerControler.isPlaying())
            {
                mediaPlayerControler.captureImage();
            }
            else
            {
                Toast.makeText(getContext(), "未播放中", Toast.LENGTH_SHORT).show();
            }
        }

    }
    boolean isLandscapeScreen(Activity activity)
    {
            Configuration mConfiguration = activity.getResources().getConfiguration();
            int ori = mConfiguration.orientation;
            return (ori == Configuration.ORIENTATION_LANDSCAPE)?true:false;
    }
    void OnMax()
    {
        Context context = getContext();
        if (context instanceof Activity)
        {
            Activity activity = (Activity)context;
            if (!isLandscapeScreen(activity))
            {
                mbtn_FullScreen.setBackgroundResource(R.drawable.video_zoom_icon);
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
            }
            else
            {
                mbtn_FullScreen.setBackgroundResource(R.drawable.video_full_screen);
                activity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
            }
        }

    }
    public  void recivePlayerEvent(int eventid, long nerror)
    {
        if (mediaPlayerControler == null)
        {
            return ;
        }
        if (eventid == PlayerConstants.PE_ReadyToPlay)
        {
            mtv_playinfo.setText("准备开始播放");
            msb_Seeking.setEnabled(true);
            msb_Seeking.setMax(mediaPlayerControler.getDuration());
            mtv_TotalTime.setText(secToString(mediaPlayerControler.getDuration()));
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
    public void setPlayerInfo(String info)
    {
        mtv_playinfo.setText(info);
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
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        if (mediaPlayerControler != null)
        {
            if (mediaPlayerControler.canSeek())
            {
                int seekseconds = seekBar.getProgress();
                mediaPlayerControler.seekTo(seekseconds);
                mtv_CurTime.setText(secToString(seekseconds));
            }
        }

    }
}
