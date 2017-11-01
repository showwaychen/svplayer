package cxw.cn.svplayer.example.activity;

import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.net.Uri;
import android.nfc.Tag;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.NonNull;
import android.support.v4.app.NavUtils;
import android.support.v4.media.session.MediaControllerCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.util.Log;
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
import android.widget.Toast;

import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Locale;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.LogRecord;

import cxw.cn.svplayer.CommonSetting;
import cxw.cn.svplayer.IMediaPlayerControl;
import cxw.cn.svplayer.PlayerConstants;
import cxw.cn.svplayer.PlayerEventListener;
import cxw.cn.svplayer.SMediaPlayer;
import cxw.cn.svplayer.SVideoPlayer;
import cxw.cn.svplayer.example.R;
import cxw.cn.svplayer.example.dialog.ImageViewDialog;
import cxw.cn.svplayer.example.widget.IRenderView;
import cxw.cn.svplayer.example.widget.MediaPlayerControlView;
import cxw.cn.svplayer.example.widget.SurfaceViewRender;
import cxw.cn.svplayer.example.widget.TextureViewRender;

/**
 * Created by cxw on 2017/10/2.
 */

public class VideoPlayerActivity extends AppCompatActivity implements View.OnClickListener,PlayerEventListener ,IMediaPlayerControl, IRenderView.IRenderCallback{

    static String TAG = VideoPlayerActivity.class.getCanonicalName();
    FrameLayout mVideoLayout = null;
    FrameLayout mfl_VideoParent = null;
    IRenderView mRenderView = null;
    SMediaPlayer mPlayer = null;
    public static final String ARG_URL = "url";
    String mPlayUrl = null;
    MediaPlayerControlView mMediaControlView = null;

    public static void active(Context context, String url)
    {
        Intent intent = new Intent(context, VideoPlayerActivity.class);
        intent.putExtra(ARG_URL, url);
        context.startActivity(intent);
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_videopaly);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Intent intent = getIntent();
        String action = intent.getAction();
        if (intent.ACTION_VIEW.equals(action))
        {
            mPlayUrl = intent.getDataString();
            Log.d(TAG, "view action url = "+ mPlayUrl);
        }
        else
        {
            mPlayUrl = getIntent().getStringExtra(ARG_URL);
        }
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
        mMediaControlView.setMediaControler(null);

        if (isFinishing())
        {
            mRenderView.removeRenderCallback(this);
            mDestroy = true;
            SMediaPlayer tmpplay = mPlayer;
            mPlayer = null;
            tmpplay.destroyPlayer();
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
        mMediaControlView.setMediaControler(this);
        mPlayer.resumePlayer();
    }
    void initView()
    {
        mVideoLayout = (FrameLayout) findViewById(R.id.videoFrameLayout);
        mfl_VideoParent = (FrameLayout)findViewById(R.id.fl_videoparent);
        mMediaControlView = (MediaPlayerControlView)findViewById(R.id.vw_mediacontrol);
        mMediaControlView.setMediaControler(this);
    }
    IRenderView createRenderView(int type)
    {
        if (type == 1)
        {
            return new SurfaceViewRender(this);
        }
        else if (type == 2)
        {
            return new TextureViewRender(this);
        }
        return null;
    }
    void initPlayer()
    {
        CommonSetting.nativeToggleFFmpegLog(false);
        CommonSetting.nativeSetLogLevel(PlayerConstants.LS_SENSITIVE);
        mRenderView = createRenderView(2);
        mRenderView.addRenderCallback(this);
        mVideoLayout.addView(mRenderView.getView());
        mPlayer = new SMediaPlayer();
        mPlayer.setEventlisten(this);
        mPlayer.setUrl(mPlayUrl);
        mPlayer.setAudioPlayerType(PlayerConstants.AudioPlayType_AudioTrack);
        mPlayer.setShowMode(PlayerConstants.kShowModeAspectAuto);
        mPlayer.startPlayer();
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

    }


    @Override
    public void onPlayerEvent(final int eventid, final long resdata) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (eventid == PlayerConstants.PE_Snapshot_Sus)
                {
                    ImageViewDialog display = new ImageViewDialog(VideoPlayerActivity.this, R.style.dialog);
                    display.setImagePath(mCaptureFilename);
                    display.setCanceledOnTouchOutside(true);//设置点击Dialog外部任意区域关闭Dialog
                    display.show();
                    Toast.makeText(VideoPlayerActivity.this, "截图成功", Toast.LENGTH_SHORT).show();
                }
                else if (eventid == PlayerConstants.PE_Snapshot_Fail)
                {
                    Toast.makeText(VideoPlayerActivity.this, "截图失败", Toast.LENGTH_SHORT).show();
                }
                else
                {
                    mMediaControlView.recivePlayerEvent(eventid, resdata);
                }

            }
        });
    }

    @Override
    public int start() {
        return mPlayer.startPlayer();
    }

    @Override
    public int pause() {
        return mPlayer.pausePlayer();
    }

    @Override
    public int resume() {
        return mPlayer.resumePlayer();
    }

    String mCaptureFilename;
    @Override
    public int captureImage() {
        mCaptureFilename = Environment.getExternalStorageDirectory() + "/svplayercapture.jpeg";
        return mPlayer.captureImage(mCaptureFilename);
    }

    @Override
    public int getDuration() {
        if (mPlayer != null)
        {
            return mPlayer.getDuration();
        }
        return 0;
    }

    @Override
    public int getCurrentPosition()
    {
        if (mPlayer != null)
        return mPlayer.getCurrentPosition();
        return 0;
    }

    @Override
    public void seekTo(int pos) {
        mPlayer.seekPlayer(pos);
    }

    @Override
    public boolean isPlaying() {
        boolean isplaying = mPlayer.isPlaying();
        return isplaying;
    }

    @Override
    public boolean canPause() {
        return true;
    }

    @Override
    public boolean canSeek() {
        return true;
    }

    @Override
    public void onSurfaceCreated(@NonNull IRenderView.ISurfaceHolder holder, int width, int height) {
        mPlayer.setDisplay(holder.getSurface());
        mPlayer.resizeDisplay(width, height);

    }

    @Override
    public void onSurfaceChanged(@NonNull IRenderView.ISurfaceHolder holder, int width, int height) {
        mPlayer.resizeDisplay(width, height);
    }

    @Override
    public void onSurfaceDestroyed(@NonNull IRenderView.ISurfaceHolder holder) {
        if (mPlayer != null)
        {
            mPlayer.setDisplay(null);
        }
    }
}
