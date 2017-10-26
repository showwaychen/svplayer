package cxw.cn.svplayer;

import android.view.Surface;

/**
 * Created by cxw on 2017/9/19.
 */

public class NativeRender implements GlRenderThread.GLRenderer {

    GlRenderThread mGlRenderThread = null;
    Surface mSurface;
    long mNativeObject = 0;
    static
    {
        System.loadLibrary("svplayer");
    }
    native  long nativeCreateObject();
//    native void  nativeSetRotation(int rotation);
    public native int nativeRenderInit();
    public native int nativeRenderDeinit();
    public native int nativeRenderResize(int width, int height);
    public native int  nativeRenderFrame();
    public native void  nativeDestroy();
    public long getNativeObject()
    {
        return mNativeObject;
    }
    public NativeRender()
    {
        mNativeObject = nativeCreateObject();
    }
    public  void setRotation(int rotation)
    {

    }

    public void requestResize(int width, int height)
    {
        mGlRenderThread.requestResize(width, height);
    }

    public void startRender(Surface surface)
    {
        if (mGlRenderThread == null)
        {
            mSurface = surface;
            mGlRenderThread = new GlRenderThread(mSurface, this);
            mGlRenderThread.start();
        }
    }
    //native call
    public void requestRender()
    {
        if (mGlRenderThread != null)
        {
            mGlRenderThread.requestRender();
        }
    }
    //native call
    private int startRendering()
    {
        return 0;
    }

    //native call
    public int stopRendering()
    {
        mGlRenderThread.stopRender();
        mGlRenderThread = null;
        return 0;
    }

    public  void destroyRender()
    {
        mGlRenderThread.stopRender();
        mGlRenderThread = null;
        nativeDestroy();
        mNativeObject = 0;
    }
    @Override
    public void init() {
        nativeRenderInit();
    }

    @Override
    public void resize(int width, int height) {
        nativeRenderResize(width, height);
    }

    @Override
    public void drawFrame() {
        nativeRenderFrame();
    }

    @Override
    public void deinit() {
        nativeRenderDeinit();
        mGlRenderThread = null;
        mSurface = null;
    }
}
