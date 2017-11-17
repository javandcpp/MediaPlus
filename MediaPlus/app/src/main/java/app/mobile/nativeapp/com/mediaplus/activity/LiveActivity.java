/*
 * MIT License
 *
 * Copyright (c) 2017 Huawque
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @author:weichang
 * https://github.com/javandoc/MediaPlus
 */

package app.mobile.nativeapp.com.mediaplus.activity;

import android.app.Activity;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.DisplayMetrics;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import app.mobile.nativeapp.com.libmedia.core.streamer.PushStreamCall;
import app.mobile.nativeapp.com.libmedia.core.streamer.RtmpPushStreamer;
import app.mobile.nativeapp.com.mediaplus.R;

public class LiveActivity extends AppCompatActivity implements View.OnClickListener {
    private SurfaceView surfaceView;
    private TextView tvVideo;
    private TextView tvAudio;
    private RtmpPushStreamer mRtmpPushStreamer;
    private RelativeLayout videoParent;
    private Button btnStart;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // 防止锁屏
        setContentView(R.layout.activity_recorder);
        final Switch swflash = (Switch) findViewById(R.id.swFlash);
        swflash.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if (!mRtmpPushStreamer.Light()) {
                    swflash.setChecked(false);
                    Toast.makeText(getApplicationContext(), "前置摄像头,不支持闪光灯!", Toast.LENGTH_SHORT).show();
                }
            }
        });
        ((Switch) findViewById(R.id.swBeauty)).setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {

            }
        });
        btnStart = (Button) findViewById(R.id.btnStart);
        btnStart.setOnClickListener(this);
        videoParent = (RelativeLayout) findViewById(R.id.rlLayout);
        findViewById(R.id.btnSwitchCamera).setOnClickListener(this);
        tvAudio = ((TextView) findViewById(R.id.tvAudioInfo));
        tvVideo = ((TextView) findViewById(R.id.tvVideoInfo));

        SurfaceView surfaceView = new SurfaceView(this);
        mRtmpPushStreamer = new RtmpPushStreamer(this, surfaceView, new PushStreamCall() {
            @Override
            public void PushSucess() {
                btnStart.setText("停止直播");
                Toast.makeText(getApplicationContext(),"推流成功",Toast.LENGTH_LONG).show();
            }

            @Override
            public void PushFailed() {
                btnStart.setText("开始直播");
                Toast.makeText(getApplicationContext(),"推流失败",Toast.LENGTH_LONG).show();
            }
        });

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            DisplayMetrics displayMetrics = new DisplayMetrics();
            getWindowManager().getDefaultDisplay().getRealMetrics(displayMetrics);
            surfaceView.setLayoutParams(new RelativeLayout.LayoutParams(displayMetrics.widthPixels, displayMetrics.heightPixels));
        } else {
            int width = getWindowManager().getDefaultDisplay().getWidth();
            int height = getWindowManager().getDefaultDisplay().getHeight();
            surfaceView.setLayoutParams(new RelativeLayout.LayoutParams(width, height));
        }
        videoParent.addView(surfaceView);

    }

    @Override
    protected void onResume() {
        super.onResume();
        mRtmpPushStreamer.onActivityResume();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mRtmpPushStreamer.onActivityPause();
    }

    @Override
    protected void onStop() {
        super.onStop();
        mRtmpPushStreamer.destroy();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btnStart:
                if (mRtmpPushStreamer.isSpeak()) {
                    mRtmpPushStreamer.stopSpeak();

                } else {
                    mRtmpPushStreamer.startSpeak();

                }
                break;
            case R.id.btnSwitchCamera:
                mRtmpPushStreamer.SwitchCamera();
                break;

        }

    }

}
