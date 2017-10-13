package cxw.cn.svplayer.example.Fragments;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.support.v4.os.EnvironmentCompat;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ListView;

import java.io.File;

import cxw.cn.svplayer.example.R;
import cxw.cn.svplayer.example.activity.VideoPlayerActivity;
import cxw.cn.svplayer.example.adapter.AdapterFilelist;

/**
 * Created by cxw on 2017/10/4.
 */

public class FileExplorerFragment extends Fragment {

    View mRootView = null;
    ListView mlv_filelist;
    AdapterFilelist mAdapter = null;
    String mRootPath = Environment.getExternalStorageDirectory().getAbsolutePath();
    String mCurrentPath = mRootPath;

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        mRootView =  (ViewGroup) inflater.inflate(R.layout.fragment_fileexplorer, container, false);
        initView();
        return mRootView;
    }

    void initView()
    {
        File currentfile = new File(mCurrentPath);
        mlv_filelist = (ListView) mRootView.findViewById(R.id.lv_fileexplorer);
        mAdapter = new AdapterFilelist(this.getContext(),currentfile.listFiles());
        mlv_filelist.setAdapter(mAdapter);
        mlv_filelist.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                File fileitem = mAdapter.getItem(i);
                if (fileitem.isDirectory())
                {
                    mCurrentPath = fileitem.getAbsolutePath();
                    mAdapter.setData(fileitem.listFiles());
                }
                else
                {
                    if (fileitem.isFile())
                    {
                        String extname;
                        String filename = fileitem.getName();
                        int extPos = filename.lastIndexOf('.');
                        if (extPos >= 0)
                        {
                            extname = filename.substring(extPos + 1);
                            if (!extname.isEmpty() && (extname.contains("mp4") || extname.contains("flv")))
                            {
                                VideoPlayerActivity.active(FileExplorerFragment.this.getContext(), fileitem.getAbsolutePath());
                            }
                        }
                    }
                }
            }
        });
//        mAdapter.setData(currentfile.listFiles());
    }
    public  void onBackPressed()
    {
        if (mRootPath.compareTo(mCurrentPath) == 0)
        {
            getActivity().finish();
        }
        else
        {
            mCurrentPath = new File(mCurrentPath).getParent();
            mAdapter.setData(new File(mCurrentPath).listFiles());
        }
    }
}
