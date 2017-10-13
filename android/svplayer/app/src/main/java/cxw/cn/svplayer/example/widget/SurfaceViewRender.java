package cxw.cn.svplayer.example.widget;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import cxw.cn.svplayer.NativeRender;

/**
 * Created by cxw on 2017/9/23.
 */

public class SurfaceViewRender extends SurfaceView implements SurfaceHolder.Callback{

    NativeRender mNativeRender = null;
    public  SurfaceViewRender(Context context)
    {
        super(context);
        initView();
    }

    public SurfaceViewRender(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView();
    }
    public SurfaceViewRender(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        initView();
    }

    private void initView()
    {
        getHolder().addCallback(this);
        mNativeRender = new NativeRender();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mNativeRender.startRender(holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        mNativeRender.requestResize(width, height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mNativeRender.destroyRender();
        mNativeRender = null;
    }

    public  NativeRender getNativeRender()
    {
        return mNativeRender;
    }
}
