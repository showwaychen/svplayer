package cxw.cn.svplayer.example.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.zip.Inflater;

import cxw.cn.svplayer.example.R;

/**
 * Created by cxw on 2017/10/4.
 */

public class AdapterFilelist extends BaseAdapter {
    ArrayList<File> mfilelist = null;
    Context mContext = null;
    final class ViewHolder {
        public ImageView iconImageView;
        public TextView nameTextView;
    }
    protected static Comparator<File> sComparator = new Comparator<File>() {
        @Override
        public int compare(File file, File t1) {
            if (file.isDirectory() && !t1.isDirectory())
            {
                return -1;
            }
            else if (!file.isDirectory() && t1.isDirectory())
            {
                return 1;
            }
            return file.getName().compareTo(t1.getName());
        }
    };
    protected void sortFiles()
    {
        Collections.sort(mfilelist, sComparator);
    }
    public AdapterFilelist(Context context, File[] filelist)
    {
        mfilelist = new ArrayList<File>(Arrays.asList(filelist));
        sortFiles();
        mContext = context;
    }
    @Override
    public int getCount() {
        if (mfilelist == null)
        {
            return 0;
        }
        return mfilelist.size();
    }

    @Override
    public File getItem(int i) {
        if (mfilelist == null)
        {
            return null;
        }
        return mfilelist.get(i);
    }

    @Override
    public long getItemId(int i) {
        return 0;
    }

    public void setData(File[] filelist)
    {
        mfilelist = new ArrayList<File>(Arrays.asList(filelist));
        sortFiles();
        notifyDataSetChanged();
    }
    @Override
    public View getView(int i, View view, ViewGroup viewGroup) {
//        if (mfilelist == null )
//        {
//            return view;
//        }
        File fileitem = mfilelist.get(i);
        View itemview = view;
        if (view == null)
        {
            itemview = LayoutInflater.from(mContext).inflate(R.layout.filelist_item, viewGroup,false);
        }
        TextView tvname = (TextView)itemview.findViewById(R.id.tv_name);
        ImageView iv_icon = (ImageView)itemview.findViewById(R.id.iv_icon);
        tvname.setText(fileitem.getName());
        if (fileitem.isDirectory())
        {
            iv_icon.setImageResource(R.drawable.ic_folder);
        }
        else
        {
            iv_icon.setImageResource(R.drawable.ic_description);
        }
        return itemview;
    }
}
