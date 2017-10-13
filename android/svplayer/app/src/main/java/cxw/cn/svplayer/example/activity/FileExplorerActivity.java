package cxw.cn.svplayer.example.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.PersistableBundle;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AppCompatActivity;

import cxw.cn.svplayer.example.BaseActivity;
import cxw.cn.svplayer.example.Fragments.FileExplorerFragment;
import cxw.cn.svplayer.example.R;

/**
 * Created by cxw on 2017/10/2.
 */

public class FileExplorerActivity extends BaseActivity {

    public static void active(Context context)
    {
        Intent intent = new Intent(context, FileExplorerActivity.class);
        context.startActivity(intent);
    }
    FileExplorerFragment filelistfragment = null;
    private void showFragment(Fragment targetFragment) {
        FragmentTransaction beginTransaction = getSupportFragmentManager().beginTransaction();
        if (!targetFragment.isAdded()) {    // 先判断是否被add过
            beginTransaction.add(R.id.fl_content, targetFragment).commit(); // 隐藏当前的fragment，add下一个到Activity中
        } else {
            beginTransaction.show(targetFragment).commit(); // 隐藏当前的fragment，显示下一个
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    public void onBackPressed() {
//        super.onBackPressed();
        filelistfragment.onBackPressed();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        filelistfragment = new FileExplorerFragment();
        showFragment(filelistfragment);
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

}
