package cxw.cn.svplayer.example.widget;

import android.content.Context;
import android.support.annotation.NonNull;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.util.HashSet;
import java.util.Set;


/**
 * Created by cxw on 2017/9/23.
 */

public class SurfaceViewRender extends SurfaceView implements  IRenderView{
    private SurfaceCallback mSurfaceHolder;
    private boolean mIsFormatChanged;
    private int mFormat;
    private int mWidth;
    private int mHeight;
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

    private static final class InternalSurfaceHolder implements IRenderView.ISurfaceHolder {
        private SurfaceHolder mSurfaceHolder;
        public InternalSurfaceHolder(SurfaceHolder surfaceHolder)
        {
            mSurfaceHolder = surfaceHolder;
        }
        @NonNull
        @Override
        public Surface getSurface() {
            return mSurfaceHolder.getSurface();
        }
    }
    private static final class SurfaceCallback implements SurfaceHolder.Callback {
        private SurfaceHolder mSurfaceHolder;
        private boolean mIsFormatChanged;
        private int mFormat;
        private int mWidth;
        private int mHeight;

        private Set<IRenderCallback> mRenderCallback = new HashSet<IRenderCallback>();


        public void addRenderCallback(@NonNull IRenderCallback callback) {
            mRenderCallback.add(callback);

            ISurfaceHolder surfaceHolder = null;
            if (mSurfaceHolder != null) {
                if (surfaceHolder == null)
                    surfaceHolder = new InternalSurfaceHolder(mSurfaceHolder);
                callback.onSurfaceCreated(surfaceHolder, mWidth, mHeight);
            }

            if (mIsFormatChanged) {
                if (surfaceHolder == null)
                    surfaceHolder = new InternalSurfaceHolder(mSurfaceHolder);
                callback.onSurfaceChanged(surfaceHolder, mWidth, mHeight);
            }
        }

        public void removeRenderCallback(@NonNull IRenderCallback callback) {
            mRenderCallback.remove(callback);
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            mSurfaceHolder = holder;
            mIsFormatChanged = false;
            mFormat = 0;
            mWidth = 0;
            mHeight = 0;

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder( mSurfaceHolder);
            for (IRenderCallback renderCallback : mRenderCallback) {
                renderCallback.onSurfaceCreated(surfaceHolder, 0, 0);
            }
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            mSurfaceHolder = null;
            mIsFormatChanged = false;
            mFormat = 0;
            mWidth = 0;
            mHeight = 0;

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder( mSurfaceHolder);
            for (IRenderCallback renderCallback : mRenderCallback) {
                renderCallback.onSurfaceDestroyed(surfaceHolder);
            }
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format,
                                   int width, int height) {
            mSurfaceHolder = holder;
            mIsFormatChanged = true;
            mFormat = format;
            mWidth = width;
            mHeight = height;

            // mMeasureHelper.setVideoSize(width, height);

            ISurfaceHolder surfaceHolder = new InternalSurfaceHolder(mSurfaceHolder);
            for (IRenderCallback renderCallback : mRenderCallback) {
                renderCallback.onSurfaceChanged(surfaceHolder, width, height);
            }
        }
    }
    private void initView()
    {
        mSurfaceHolder = new SurfaceCallback();
        getHolder().addCallback(mSurfaceHolder);
        //noinspection deprecation
        getHolder().setType(SurfaceHolder.SURFACE_TYPE_NORMAL);
    }

    @Override
    public View getView() {
        return this;
    }

    @Override
    public void addRenderCallback(@NonNull IRenderCallback callback) {
        mSurfaceHolder.addRenderCallback(callback);
    }

    @Override
    public void removeRenderCallback(@NonNull IRenderCallback callback) {
        mSurfaceHolder.removeRenderCallback(callback);
    }
}
