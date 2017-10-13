package cxw.cn.svplayer;

import android.graphics.SurfaceTexture;
import android.opengl.EGL14;
import android.opengl.EGLExt;
import android.opengl.GLUtils;
import android.util.Log;
import android.view.Surface;

import java.util.Objects;
import java.util.concurrent.atomic.AtomicBoolean;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by cxw on 2017/9/19.
 */

//public class GlRenderThread  extends Thread{
//    private static final String TAG = "GlRenderThread";
//
//    // 外部设置
////    private KuPlayRendererCallback mRenderer = null;
//    private Object mSyncToken;
//    private boolean mRequestRender;
//    private boolean mRequestDestroy;
//
//    private int mWidth = 512;
//    private int mHeight = 512;
//    private int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
//    private int mEGLContextClientVersion;
//    private int mEGLRenderableType;
//    private EGL10 mEGL;
//    private EGLDisplay mEGLDisplay;
//    private EGLConfig mEGLConfig;
//    private EGLSurface mEGLSurface;
//    private EGLContext mEGLContext;
//    private GL10 mGL10;
//
//    public GlRenderThread()
//    {
////    public GlRenderThread(KuPlayRendererCallback renderer) {
////        mRenderer = renderer;
//        mSyncToken = new Object();
//        mRequestRender = false;
//        mRequestDestroy = false;
//    }
//
//    @Override
//    public void run() {
//        setName("GlRenderThread " + getId());
//        Log.i(TAG, "start tid = " + getId());
//        try {
//            guardedRun();
//        } catch (InterruptedException e) {
//            Log.e(TAG, "InterruptedException " + e.getMessage());
//            e.printStackTrace();
//        }
//        Log.i(TAG, "end tid = " + getId());
//    }
//
//    private void guardedRun() throws InterruptedException {
//        // 版本
//        int currentapiVersion = android.os.Build.VERSION.SDK_INT;
//        if (currentapiVersion >= 24){
//            mEGLContextClientVersion = 3;
//            mEGLRenderableType = EGLExt.EGL_OPENGL_ES3_BIT_KHR;
//        } else {
//            mEGLContextClientVersion = 2;
//            mEGLRenderableType = EGL14.EGL_OPENGL_ES2_BIT;
//        }
//
//        // 初始化
//        mEGL = (EGL10) EGLContext.getEGL();
//        mEGLDisplay = mEGL.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
//        if (mEGLDisplay == EGL10.EGL_NO_DISPLAY) {
//            throw new RuntimeException("eglGetDisplay failed");
//        }
//
//        int[] version = new int[2];
//        if (!mEGL.eglInitialize(mEGLDisplay, version)) {
//            throw new RuntimeException("eglInitialize failed");
//        }
//
//        // 打印egl信息
//        Log.i(TAG, "egl vendor: " + mEGL.eglQueryString(mEGLDisplay, EGL10.EGL_VENDOR));
//        Log.i(TAG, "egl version: " + mEGL.eglQueryString(mEGLDisplay, EGL10.EGL_VERSION));
//        Log.i(TAG, "egl extension: " + mEGL.eglQueryString(mEGLDisplay, EGL10.EGL_EXTENSIONS));
//
//        int[] configAttributes = new int[] {
//                EGL10.EGL_RED_SIZE, 8,
//                EGL10.EGL_GREEN_SIZE, 8,
//                EGL10.EGL_BLUE_SIZE, 8,
//                EGL10.EGL_ALPHA_SIZE, 8,
//                EGL10.EGL_DEPTH_SIZE, 16,
//                EGL10.EGL_RENDERABLE_TYPE, mEGLRenderableType,
//                EGL10.EGL_NONE };
//
//        int[] num_config = new int[1];
//        if (!mEGL.eglChooseConfig(mEGLDisplay, configAttributes, null, 0, num_config)) {
//            throw new IllegalArgumentException("eglChooseConfig failed");
//        }
//        int numConfigs = num_config[0];
//        if (numConfigs <= 0) {
//            throw new IllegalArgumentException("No configs match configSpec");
//        }
//
//        EGLConfig[] configs = new EGLConfig[numConfigs];
//        if (!mEGL.eglChooseConfig(mEGLDisplay, configAttributes, configs, numConfigs, num_config)) {
//            throw new IllegalArgumentException("eglChooseConfig#2 failed");
//        }
//        mEGLConfig = configs[0];
//
//        int[] pBufAttributes = new int[] {
//                EGL10.EGL_WIDTH, mWidth,
//                EGL10.EGL_HEIGHT, mHeight,
//                EGL10.EGL_NONE
//        };
//        mEGLSurface = mEGL.eglCreatePbufferSurface(mEGLDisplay, mEGLConfig, pBufAttributes);
//        if (mEGLSurface == EGL10.EGL_NO_SURFACE) {
//            throw new IllegalArgumentException("eglCreatePbufferSurface failed");
//        }
//
//        int[] contextAttributes = new int[] {
//                EGL_CONTEXT_CLIENT_VERSION, mEGLContextClientVersion,
//                EGL10.EGL_NONE
//        };
//        mEGLContext = mEGL.eglCreateContext(mEGLDisplay, mEGLConfig, EGL10.EGL_NO_CONTEXT, contextAttributes);
//        if (mEGLContext == EGL10.EGL_NO_CONTEXT) {
//            throw new IllegalArgumentException("eglCreateContext failed");
//        }
//
//        if (!mEGL.eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext)) {
//            throw new IllegalArgumentException("eglMakeCurrent failed");
//        }
//        mGL10 = (GL10)mEGLContext.getGL();
//
//        mRenderer.KuPlayRendererOnSurfaceCreated(mGL10, mEGLConfig);
//        mRenderer.KuPlayRendererOnSurfaceChanged(mGL10, mWidth, mHeight);
//
//        // RENDERMODE_WHEN_DIRTY的方式
//        while (true) {
//            synchronized (mSyncToken) {
//                if (mRequestDestroy) {
//                    break;
//                }
//
//                if (mRequestRender == false) {
//                    try {
////                        Log.i(TAG, "wait");
//                        mSyncToken.wait();
//                    } catch (InterruptedException e) {
//                        e.printStackTrace();
//                    }
//                }
//                mRequestRender = false;
//                if (mRequestDestroy) {
//                    break;
//                }
//            }
////                Log.i(TAG, "onDrawFrame ##");
//            mRenderer.KuPlayRendererOnDrawFrame(mGL10);
//        }
//        mRenderer.KuPlayRendererOnSurfaceWillDestroy(mGL10, mEGLConfig);
//
//        mEGL.eglDestroyContext(mEGLDisplay, mEGLContext);
//        mEGL.eglDestroySurface(mEGLDisplay, mEGLSurface);
//    }
//
//    public void requestRender() {
////            Log.i(TAG, "requestRender");
//        synchronized (mSyncToken) {
//            mRequestRender = true;
//            mSyncToken.notifyAll();
//        }
//    }
//
//    public void requestDestroy() {
//        Log.i(TAG, "requestDestroy");
//        synchronized (mSyncToken) {
//            mRequestDestroy = true;
//            mSyncToken.notifyAll();
//        }
//    }
//}
public class GlRenderThread extends Thread {
    static final  String TAG = "GlRenderThread";
    private AtomicBoolean mShouldRender;
    private Surface mSurface;
    private GLRenderer mRenderer;
    private Object mSyncToken;
    boolean mRequestRender = false;
    boolean mRequestDestroy = false;
    private EGL10 mEgl;
    private EGLDisplay mEglDisplay = EGL10.EGL_NO_DISPLAY;
    private EGLContext mEglContext = EGL10.EGL_NO_CONTEXT;
    private EGLSurface mEglSurface = EGL10.EGL_NO_SURFACE;
    private GL mGL;
    private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
    private static final int EGL_OPENGL_ES2_BIT = 4;

    int mViewWidth = 0;
    int mViewHeight = 0;
    private boolean m_needResize = false;
    public interface GLRenderer {
        void init();
        void resize(int width, int height);
        void drawFrame();
        void deinit();
    }

    public GlRenderThread(Surface surface, GLRenderer renderer) {
        mSurface = surface;
        mRenderer = renderer;
        mSyncToken = new Object();
        Log.d(TAG, "new GlRenderThread");
//        mShouldRender = shouldRender;
    }

    private void initGL() {
        mEgl = (EGL10) EGLContext.getEGL();

        mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
        if (mEglDisplay == EGL10.EGL_NO_DISPLAY) {
            throw new RuntimeException("eglGetdisplay failed : " +
                    GLUtils.getEGLErrorString(mEgl.eglGetError()));
        }

        int[] version = new int[2];
        if (!mEgl.eglInitialize(mEglDisplay, version)) {
            throw new RuntimeException("eglInitialize failed : " +
                    GLUtils.getEGLErrorString(mEgl.eglGetError()));
        }

//            int[] configAttribs = {
//                    EGL10.EGL_BUFFER_SIZE, 32,
//                    EGL10.EGL_ALPHA_SIZE, 8,
//                    EGL10.EGL_BLUE_SIZE, 8,
//                    EGL10.EGL_GREEN_SIZE, 8,
//                    EGL10.EGL_RED_SIZE, 8,
//                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//                    EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
//                    EGL10.EGL_NONE
//            };
        int[] configAttribs = {
                EGL10.EGL_BUFFER_SIZE, 32,
                EGL10.EGL_ALPHA_SIZE, 8,
                EGL10.EGL_BLUE_SIZE, 5,
                EGL10.EGL_GREEN_SIZE, 6,
                EGL10.EGL_RED_SIZE, 5,
                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT,
                EGL10.EGL_NONE
        };

        int[] numConfigs = new int[1];
        EGLConfig[] configs = new EGLConfig[1];
        if (!mEgl.eglChooseConfig(mEglDisplay, configAttribs, configs, 1, numConfigs)) {
            throw new RuntimeException("eglChooseConfig failed : " +
                    GLUtils.getEGLErrorString(mEgl.eglGetError()));
        }

        int[] contextAttribs = {
                EGL_CONTEXT_CLIENT_VERSION, 2,
                EGL10.EGL_NONE
        };
        mEglContext = mEgl.eglCreateContext(mEglDisplay, configs[0], EGL10.EGL_NO_CONTEXT, contextAttribs);
        mEglSurface = mEgl.eglCreateWindowSurface(mEglDisplay, configs[0], mSurface, null);
        if (mEglSurface == EGL10.EGL_NO_SURFACE || mEglContext == EGL10.EGL_NO_CONTEXT) {
            int error = mEgl.eglGetError();
            if (error == EGL10.EGL_BAD_NATIVE_WINDOW) {
                throw new RuntimeException("eglCreateWindowSurface returned  EGL_BAD_NATIVE_WINDOW. ");
            }
            throw new RuntimeException("eglCreateWindowSurface failed : " +
                    GLUtils.getEGLErrorString(mEgl.eglGetError()));
        }

        if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
            throw new RuntimeException("eglMakeCurrent failed : " +
                    GLUtils.getEGLErrorString(mEgl.eglGetError()));
        }

        mGL = mEglContext.getGL();
        Log.d(TAG, "initGL over");
    }

    public void stopRender()
    {
        Log.i(TAG, "requestDestroy");
        synchronized (mSyncToken) {
            mRequestDestroy = true;
            mSyncToken.notifyAll();
        }
    }
    private void destoryGL() {
        mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE,
                EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
        mEgl.eglDestroyContext(mEglDisplay, mEglContext);
        mEgl.eglDestroySurface(mEglDisplay, mEglSurface);
        mEgl.eglTerminate(mEglDisplay);

        mEglContext = EGL10.EGL_NO_CONTEXT;
        mEglSurface = EGL10.EGL_NO_SURFACE;
    }
    public void requestRender() {
            Log.i(TAG, "requestRender");
        synchronized (mSyncToken) {
            mRequestRender = true;
            mSyncToken.notifyAll();
        }
    }
    public void requestResize(int width, int height)
    {
        mViewHeight = height;
        mViewWidth = width;
        m_needResize = true;
        requestRender();
    }
    public void run() {
        setName("GlRenderThread " + getId());
        initGL();

        Log.d(TAG, "gl thread start run ");
        if (mRenderer != null) {
            mRenderer.init();
        }
        while (true) {
            synchronized (mSyncToken) {
                if (mRequestDestroy) {
                    break;
                }
                if (m_needResize)
                {
                    if (mRenderer != null)
                    {
                        Log.d(TAG, "resize callback");
                        mRenderer.resize(mViewWidth, mViewHeight);
                    }
                    m_needResize = false;
                }
                if (mRequestRender == false) {
                    try {
                        mSyncToken.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                mRequestRender = false;
                if (mRequestDestroy) {
                    break;
                }
            }
//                Log.i(TAG, "onDrawFrame ##");
            if (mRenderer != null)
                mRenderer.drawFrame();
            mEgl.eglSwapBuffers(mEglDisplay, mEglSurface);

        }
        if (mRenderer != null)
        {
            mRenderer.deinit();
        }
        destoryGL();
    }
}
