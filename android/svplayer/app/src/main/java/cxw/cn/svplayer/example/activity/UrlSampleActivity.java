package cxw.cn.svplayer.example.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ListView;
import android.widget.TextView;

import cxw.cn.svplayer.CommonSetting;
import cxw.cn.svplayer.example.BaseActivity;
import cxw.cn.svplayer.example.R;

/**
 * Created by cxw on 2017/10/4.
 */

public class UrlSampleActivity extends BaseActivity{


    ListView mlv_samples = null;
    UrlSamepleAdapter mAdapter = null;

    int count = 0;
    public static  void active(Context context)
    {
        Intent intent = new Intent(context, UrlSampleActivity.class);
        context.startActivity(intent    );
    }
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initView();

    }

    @Override
    protected void onPause() {
        super.onPause();

    }

    @Override
    protected void onResume() {
        super.onResume();

    }

    void initView()
    {
        FrameLayout rootview = (FrameLayout) findViewById(R.id.fl_content);
        mlv_samples = new ListView(this);
        rootview.addView(mlv_samples);
        mAdapter = new UrlSamepleAdapter(this);
        mlv_samples.setAdapter(mAdapter);
        mlv_samples.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                SampleMediaItem item = mAdapter.getItem(i);
                VideoPlayerActivity.active(UrlSampleActivity.this, item.mUrl);
            }
        });
        mAdapter.addItem("rtmp://live.hkstv.hk.lxdns.com/live/hks", "  ");
        mAdapter.addItem("http://www.w3school.com.cn/i/movie.mp4", "  ");
    }

    final static class SampleMediaItem {
        String mUrl;
        String mName;

        public SampleMediaItem(String url, String name) {
            mUrl = url;
            mName = name;
        }
    }

    static class UrlSamepleAdapter extends ArrayAdapter<SampleMediaItem>
    {
        final class ViewHolder {
            public TextView mNameTextView;
            public TextView mUrlTextView;
        }
        public UrlSamepleAdapter(Context context) {
            super(context, android.R.layout.simple_list_item_2);
        }
        public void addItem(String url, String name) {
            add(new SampleMediaItem(url, name));
        }
        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            View view = convertView;
            if (view == null) {
                LayoutInflater inflater = LayoutInflater.from(parent.getContext());
                view = inflater.inflate(android.R.layout.simple_list_item_2, parent, false);
            }

            ViewHolder viewHolder = (ViewHolder) view.getTag();
            if (viewHolder == null) {
                viewHolder = new ViewHolder();
                viewHolder.mNameTextView = (TextView) view.findViewById(android.R.id.text1);
                viewHolder.mUrlTextView = (TextView) view.findViewById(android.R.id.text2);
            }

            SampleMediaItem item = getItem(position);
            viewHolder.mNameTextView.setText(item.mName);
            viewHolder.mUrlTextView.setText(item.mUrl);

            return view;
        }
        @Override
        public long getItemId(int position) {
            return position;
        }
    }
}
