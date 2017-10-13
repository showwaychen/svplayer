package cxw.cn.svplayer.example;

import android.Manifest;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

import cxw.cn.svplayer.CommonSetting;
import cxw.cn.svplayer.PlayerConstants;
import cxw.cn.svplayer.SVideoPlayer;
import cxw.cn.svplayer.example.activity.FileExplorerActivity;
import cxw.cn.svplayer.example.activity.UrlSampleActivity;
import cxw.cn.svplayer.example.widget.SurfaceViewRender;
import cxw.cn.svplayer.example.widget.TextureViewRender;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{




    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        String sdcardpath = Environment.getExternalStorageDirectory() + "";
        setContentView(R.layout.activity_main);
        initView();
        permissionCheck();

    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    void initView()
    {
        findViewById(R.id.tv_urlsample).setOnClickListener(this);
        findViewById(R.id.tv_fileexplorer).setOnClickListener(this);
    }

    @Override
    public void onClick(View v) {

        int vid = v.getId();
        if (vid == R.id.tv_urlsample)
        {
            UrlSampleActivity.active(this);
        }
        else if (vid == R.id.tv_fileexplorer)
        {
            FileExplorerActivity.active(this);
        }
    }
    private static final int MY_PERMISSIONS_REQUEST_READ_EXTERNAL_STORAGE = 1;

    void permissionCheck()
    {
        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.READ_EXTERNAL_STORAGE)
                != PackageManager.PERMISSION_GRANTED) {
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.READ_EXTERNAL_STORAGE)) {
                // TODO: show explanation
            } else {
                ActivityCompat.requestPermissions(this,
                        new String[]{Manifest.permission.READ_EXTERNAL_STORAGE},
                        MY_PERMISSIONS_REQUEST_READ_EXTERNAL_STORAGE);
            }
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_READ_EXTERNAL_STORAGE: {
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted, yay! Do the task you need to do.
                } else {
                    // permission denied, boo! Disable the functionality that depends on this permission.
                }
            }
        }
    }
}
