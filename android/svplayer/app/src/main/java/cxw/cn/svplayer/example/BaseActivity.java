package cxw.cn.svplayer.example;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

/**
 * Created by cxw on 2017/10/2.
 */

public class BaseActivity extends AppCompatActivity {

    protected int OnContentLayoutID()
    {
        return R.layout.activity_base;
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(OnContentLayoutID());
    }
}
