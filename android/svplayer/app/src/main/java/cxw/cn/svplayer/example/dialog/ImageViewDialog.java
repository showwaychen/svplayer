package cxw.cn.svplayer.example.dialog;

import android.app.AlertDialog;
import android.content.Context;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.widget.ImageView;

import cxw.cn.svplayer.example.R;

/**
 * Created by user on 2017/10/26.
 */

public class ImageViewDialog extends AlertDialog {
    String mImagePath;
    public ImageViewDialog(Context context) {
        super(context);
    }
    public ImageViewDialog(Context context, int theme) {
        super(context, theme);
    }
    public void setImagePath(String filename)
    {
        mImagePath = filename;
    }
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.imageview);
        InitView();
    }
    void InitView()
    {
        ImageView imageview = (ImageView)findViewById(R.id.iv_display);
        imageview.setImageBitmap(BitmapFactory.decodeFile(mImagePath));
    }
}
