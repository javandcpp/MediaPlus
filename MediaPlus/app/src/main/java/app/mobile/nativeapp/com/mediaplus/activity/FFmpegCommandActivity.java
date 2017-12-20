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

import android.Manifest;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Toast;

import app.mobile.nativeapp.com.applicationmanagement.inter.PermissionCheckResult;
import app.mobile.nativeapp.com.applicationmanagement.permission.PermissionManager;
import app.mobile.nativeapp.com.libmedia.core.jni.FFmpegCommandLine;
import app.mobile.nativeapp.com.mediaplus.R;


public class FFmpegCommandActivity extends AppCompatActivity implements View.OnClickListener {

    private boolean ReadPermissonGranted;
    private String sdAbsolutePath;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ffmpeg_command);
        findViewById(R.id.btnCommand1).setOnClickListener(this);


        PermissionManager.getInstance(this).permissonCheck(new PermissionCheckResult() {
            @Override
            public void granted() {
                ReadPermissonGranted = true;
                sdAbsolutePath = Environment.getExternalStorageDirectory().getAbsolutePath().toString();
            }

            @Override
            public void beDenied() {

            }
        }, Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.btnCommand1:
                if (ReadPermissonGranted) {
//                    ffmpeg -i test.flv -i mark.png -filter_complex "overlay=5:5" out.flv
//                    ffmpeg -i test.flv -ss 10 -t 5 out.flv
                    String cmd="ffmpeg -i "+sdAbsolutePath+"/testcase.wav -f mp3 -acodec libmp3lame -y "+sdAbsolutePath+"/output_test.mp3";
//                    String result=String.format("ffmpeg -i %s %s",inputPath+"/test.flv",outputPath+"/outtest.flv");
//                    String cmd="ffmpeg -re -i /mnt/sdcard/ffmpeg.flv -vcodec copy -f flv rtmp://192.168.24.153:1935/test/live";
//                    String cmd = "ffmpeg -i " + inputPath +  "/test.flv" + " -y -ab 32 -ar 22050 -qscale 10 -s 640*480 -r 15 " + outputPath + "/test_out.flv";
//                    String cmd = "ffmpeg -i " + inputPath + "/test.flv "  + outputPath + "/test_out1.flv";
                    String regulation=" ";
                    final String[] splitStr = cmd.split(regulation);
                    FFmpegCommandLine.runCommand(1,splitStr);
                } else {
                    Toast.makeText(getApplicationContext(), "请检查存储读写权限!", Toast.LENGTH_SHORT).show();
                }
                break;
        }
    }
}
