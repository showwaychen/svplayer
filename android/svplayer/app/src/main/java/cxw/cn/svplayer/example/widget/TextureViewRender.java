package cxw.cn.svplayer.example.widget;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Set;

import cxw.cn.svplayer.NativeRender;

/**
 * Created by cxw on 2017/10/1.
 */

public class TextureViewRender extends TextureView implements  IRenderView{
    SurfaceCallback mSurfaceCallBack  = null;
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

    @Override
    public View getView() {
        return this;
    }

    @Override
    public void addRenderCallback(@NonNull IRenderCallback callback) {
        mSurfaceCallBack.addRenderCallback(callback);
    }

    @Override
    public void removeRenderCallback(@NonNull IRenderCallback callback) {
        mSurfaceCallBack.removeRenderCallback(callback);
    }

    private static final class InternalSurfaceHolder implements IRenderView.ISurfaceHolder {
        private SurfaceTexture mSurfaceTexture;
        public InternalSurfaceHolder(SurfaceTexture surfaceTexture)
        {
            mSurfaceTexture = surfaceTexture;
        }
        @NonNull
        @Override
        public Surface getSurface() {
            return new Surface(mSurfaceTexture);
        }
    }
    class SurfaceCallback implements TextureView.SurfaceTextureListener
    {
        private SurfaceTexture mSurfaceTexture;
        private boolean mIsFormatChanged;
        private int mWidth;
        private int mHeight;
        Set<IRenderCallback> mCallBacks = new HashSet<IRenderCallback>();
        public void addRenderCallback(@NonNull IRenderCallback callback) {
                mCallBacks.add(callback);
            ISurfaceHolder surfaceHolder = null;
            if (mSurfaceTexture != null) {
                if (surfaceHolder == null)
                    surfaceHolder = new InternalSurfaceHolder(mSurfaceTexture);
                callback.onSurfaceCreated(surfaceHolder, mWidth, mHeight);
            }

            if (mIsFormatChanged) {
                if (surfaceHolder == null)
                    surfaceHolder = new InternalSurfaceHolder(mSurfaceTexture);
                callback.onSurfaceChanged(surfaceHolder, mWidth, mHeight);
            }
        }
        public void removeRenderCallback(@NonNull IRenderCallback callback) {
                mCallBacks.remove(callback);
        }
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surfaceTexture, int i, int i1) {
            mSurfaceTexture = surfaceTexture;
            mIsFormatChanged = false;
            mWidth = i;
            mHeight = i1;
            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder(mSurfaceTexture);
            for (IRenderCallback callback:mCallBacks)
            {
                callback.onSurfaceCreated(surfaceHolder, i, i1);
            }
        }

        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surfaceTexture, int i, int i1) {
            mSurfaceTexture = surfaceTexture;
            mIsFormatChanged = true;
            mWidth = i;
            mHeight = i1;

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder(mSurfaceTexture);
            for (IRenderCallback renderCallback : mCallBacks) {
                renderCallback.onSurfaceChanged(surfaceHolder, i, i1);
            }
        }

        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surfaceTexture) {
            mSurfaceTexture = surfaceTexture;
            mIsFormatChanged = false;
            mWidth = 0;
            mHeight = 0;

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder(mSurfaceTexture);
            for (IRenderCallback renderCallback : mCallBacks) {
                renderCallback.onSurfaceDestroyed(surfaceHolder);
            }
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
    }

}
