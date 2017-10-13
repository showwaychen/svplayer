package cxw.cn.svplayer.example;

import android.app.Application;

import com.squareup.leakcanary.LeakCanary;
import com.squareup.leakcanary.RefWatcher;

/**
 * Created by user on 2017/10/11.
 */

public class MainApplication extends Application {
    private RefWatcher mRefWatcher;
    private static MainApplication instance;
    @Override public void onCreate() {
        super.onCreate();
//        if (LeakCanary.isInAnalyzerProcess(this)) {
//            // This process is dedicated to LeakCanary for heap analysis.
//            // You should not init your app in this process.
//            return;
//        }
//        mRefWatcher = LeakCanary.install(this);
        instance = this;
        // Normal app init code...
    }
    public static MainApplication getInstance() {
        return instance;
    }
    public static RefWatcher getRefWatcher() {
        return getInstance().mRefWatcher;
    }
}
