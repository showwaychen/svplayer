package cxw.cn.svplayer.example.widget;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.TextureView;

import cxw.cn.svplayer.NativeRender;

/**
 * Created by cxw on 2017/10/1.
 */

public class TextureViewRender extends TextureView {
    SurfaceCallback mSurfaceCallBack  = null;
    NativeRender mNativeRender = null;
    public TextureViewRender(Context context) {
        super(context);
        initView(context);

    }

    public TextureViewRender(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView(context);

    }

    public TextureViewRender(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initView(context);
    }
    class SurfaceCallback implements TextureView.SurfaceTextureListener
    {

        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {
                mNativeRender.startRender(new Surface(surfaceTexture));
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {
            mNativeRender.requestResize(i, i1);
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
            mNativeRender.destroyRender();
            mNativeRender = null;
            return true;
        }

        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surfaceTexture) {

        }
    }
    private void initView(Context context)
    {
        mSurfaceCallBack = new SurfaceCallback();
        setSurfaceTextureListener(mSurfaceCallBack);
        mNativeRender = new NativeRender();
    }

    public  NativeRender getNativeRender()
    {
        return mNativeRender;
    }
}
