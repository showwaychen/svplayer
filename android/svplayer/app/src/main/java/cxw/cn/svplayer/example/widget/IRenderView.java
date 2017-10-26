package cxw.cn.svplayer.example.widget;

import android.support.annotation.NonNull;
import android.view.Surface;
import android.view.View;

/**
 * Created by user on 2017/10/25.
 */

public interface IRenderView {

    View getView();
    void addRenderCallback(@NonNull IRenderCallback callback);

    void removeRenderCallback(@NonNull IRenderCallback callback);
    interface ISurfaceHolder {

        @NonNull
        Surface getSurface();
    }
    interface IRenderCallback {

        void onSurfaceCreated(@NonNull ISurfaceHolder holder, int width, int height);


        void onSurfaceChanged(@NonNull ISurfaceHolder holder,int width, int height);

        void onSurfaceDestroyed(@NonNull ISurfaceHolder holder);
    }
}
