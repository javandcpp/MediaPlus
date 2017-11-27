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

package app.mobile.nativeapp.com.libmedia.core.streamer;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.os.Environment;
import android.os.Handler;
import android.support.annotation.RequiresPermission;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.Toast;

import com.guagua.avcapture.AudioCaptureInterface;
import com.guagua.avcapture.VideoCaptureInterface;
import com.guagua.avcapture.impl.AudioCapture;
import com.guagua.avcapture.impl.VideoCapture;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.List;

import app.mobile.nativeapp.com.applicationmanagement.permission.PermissionManager;
import app.mobile.nativeapp.com.libmedia.core.config.VideoSizeConfig;
import app.mobile.nativeapp.com.libmedia.core.jni.LibJniVideoProcess;
import app.mobile.nativeapp.com.libmedia.core.jni.LiveJniMediaManager;
import app.mobile.nativeapp.com.libmedia.core.nativehandler.NativeCrashHandler;

import static android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK;

/**
 * Rtmp推流功能实现
 */

public class RtmpPushStreamer extends
        PushBase implements SurfaceHolder.Callback {

    public final SurfaceHolder mSurfaceHolder;
    private final PushStreamCall mPushStreamCallBack;
    private final Activity mContext;
    private String mPushurl;
    public int[] m_aiBufferLength;
    public AudioCaptureInterface mAudioCapture = new AudioCapture();
    public VideoCaptureInterface mVideoCapture = new VideoCapture();
    public int frameCount;
    public List<VideoCaptureInterface.CameraDeviceInfo> s_lCameraInfoList;
    public AudioThread m_oAudioThread = null;
    public VideoThread m_oVideoThread = null;
    public int maxZoomLevel;
    public int currentZoomLevel;
    public boolean isLight;
    public short m_sCameraID = 0;
    public boolean destroy = false;
    public int curCameraType = VideoCaptureInterface.CameraDeviceType.CAMERA_FACING_FRONT;
    public short currentMicIndex;
    public FileOutputStream stream;
    public boolean singleCamera;
    public VideoSizeConfig mVideoSizeConfig = new VideoSizeConfig();
    private Handler handler = new Handler();
    private final File videoFile;
    private final File audioFile;
    private FileOutputStream videoOps;
    private FileOutputStream audioOps;
    private RandomAccessFile file;
    private boolean nativeInt;
    private boolean speak;
    private boolean beautyEffect;


    public RtmpPushStreamer(Activity context, SurfaceView surfaceView, PushStreamCall pushStreamCallBack) {
        mSurfaceHolder = surfaceView.getHolder();
        this.mContext=context;
        mPushStreamCallBack = pushStreamCallBack;
        File externalStorageDirectory = Environment.getExternalStorageDirectory();
        surfaceView.getHolder().addCallback(this);
        NativeCrashHandler nativeCrashHandler = new NativeCrashHandler();
        nativeCrashHandler.registerForNativeCrash(context.getApplicationContext());

        videoFile = new File(externalStorageDirectory, "video.yuv");
        audioFile = new File(externalStorageDirectory, "audio.pcm");
        try {
            if (videoFile.exists()) {
                videoFile.delete();
            }
            if (audioFile.exists()) {
                audioFile.delete();
            }
            videoFile.createNewFile();
            audioFile.createNewFile();
            videoOps = new FileOutputStream(videoFile);
            audioOps = new FileOutputStream(audioFile);
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (!InitNative()) {
            Log.e("init native", "init failed!");
        }
    }

    /**
     * 初始化Native采集
     *
     * @return
     */
    private boolean initCapture() {
        int ret = 0;
        ret = LiveJniMediaManager.InitAudioCapture(2, 48000, 16);
        if (ret < 0) {
            Log.e("initNative", "init audio capture failed!");
            return false;
        }
        ret = LiveJniMediaManager.InitVideoCapture(mVideoSizeConfig.srcFrameWidth, mVideoSizeConfig.srcFrameHeight, 640, 480, 25, true);
        if (ret < 0) {
            Log.e("initNative", "init video capture failed!");
            return false;
        }
        return true;
    }

    /**
     * 初始化native编码器
     *
     * @return
     */
    private boolean initEncoder() {
        int ret = 0;
        ret = LiveJniMediaManager.InitAudioEncoder();
        if (ret < 0) {
            Log.e("initNative", "init AudioEncoder failed!");
            return false;
        }
        ret = LiveJniMediaManager.InitVideoEncoder();
        if (ret < 0) {
            Log.e("initNative", "init VideoEncoder failed!");
            return false;
        }
        return true;
    }

    /**
     * 初始化底层采集与编码器
     */
    private boolean InitNative() {
        if (!initCapture()) {
            return false;
        }
        if (!initEncoder()) {
            return false;
        }
        Log.d("initNative", "native init success!");
        nativeInt = true;
        return nativeInt;
    }

    /**
     * 开启推流
     *
     * @param pushUrl
     * @return
     */
    private boolean startPushStream(String pushUrl) {
        if (nativeInt) {
            int ret = 0;
            ret = LiveJniMediaManager.StartPush(pushUrl);
            if (ret < 0) {
                Log.d("initNative", "native push failed!");
                return false;
            }
            return true;
        }
        return false;
    }

    /**
     * 开始直播
     *
     * @param Pushurl
     */
    @RequiresPermission(allOf = {Manifest.permission.CAMERA, Manifest.permission.RECORD_AUDIO})
    public void startSpeak(String Pushurl) {
        mPushurl = Pushurl;
        if (!speak) {
            if (startPushStream(mPushurl)) {
                speak = true;
                if (mPushStreamCallBack != null) {
                    mPushStreamCallBack.PushSucess();
                }
            } else {
                speak = false;
                if (mPushStreamCallBack != null) {
                    mPushStreamCallBack.PushFailed();
                }
            }
        }
    }

    public boolean isSpeak() {
        return speak;
    }

    /**
     * 直播停止
     */
    public void stopSpeak() {
        if (speak) {
            speak = false;
            LiveJniMediaManager.Close();
        }
    }

    /**
     * 锁毁推流器
     *
     * @return
     */
    public boolean destroy() {
        if (!destroy) {
            destroy = true;
            closeCapture();
            LiveJniMediaManager.Release();
            try {
                if (videoOps != null) {
                    videoOps.close();
                }
                if (audioOps != null) {
                    audioOps.close();
                }
            } catch (Exception e) {

            }
        }
        return false;
    }

    /**
     * 关闭音视频采集
     */
    public void closeAudioVideoCapture() {
        if (null != mAudioCapture && mAudioCapture.IsStartAudioCapture()) {
            if (m_oVideoThread != null) {

                m_oAudioThread.stopThread();
                try {
                    m_oAudioThread.join();
                } catch (Exception e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
        }
        if (null != mVideoCapture && mVideoCapture.IsStartVideoCapture()) {
            if (m_oVideoThread != null) {
                m_oVideoThread.stopThread();
                try {
                    m_oVideoThread.join();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
        if (null != mAudioCapture) {
            mAudioCapture.CloseAudioDevice();
        }
        if (null != mVideoCapture) {
            mVideoCapture.CloseVideoDevice();
        }
    }

    /**
     * 关闭采集
     *
     * @return
     */
    public int closeCapture() {
        if (nativeInt) {
            LiveJniMediaManager.Close();
        }
        closeAudioVideoCapture();
        return 0;
    }


    public void onActivityResume() {
        if (!destroy) {

//            createCapture(currentMicIndex);
//            if (m_oVideoThread == null
//                    || !m_oVideoThread.isAlive()) {
//                m_oVideoThread = new VideoThread();
//                m_oVideoThread.start();
//            }
//
//            if (null == m_oAudioThread || !m_oAudioThread
//                    .isAlive()) {
//                m_oAudioThread = new AudioThread();
//                m_oAudioThread.start();
//            }
        }
    }

    public void onActivityPause() {
        if (!destroy) {
//            closeAudioVideoCapture();
        }
    }


    /**
     * 开始采信
     *
     * @return
     */
    public int createCapture(short micIndex) {
        m_aiBufferLength = new int[1];
        try {
            AudioCaptureInterface.OpenAudioDeviceReturn retAudio = mAudioCapture.OpenAudioDevice(
                    48000,
                    2,
                    16, m_aiBufferLength);

            if (retAudio == AudioCaptureInterface.OpenAudioDeviceReturn.OPEN_DEVICE_SUCCESS) {
                if (null == m_oAudioThread || m_oAudioThread.isAlive() == false) {
                    m_oAudioThread = new AudioThread();
                    mAudioCapture.StartAudioCapture();
                    m_oAudioThread.start();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
            return 0;
        }
        switchCamera(curCameraType, micIndex);
        return 0;
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {
        createCapture((short) 0);
    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }


    /**
     * 视频采集线程
     */
    class VideoThread extends Thread {

        public volatile boolean m_bExit = false;
        byte[] m_nv21Data = new byte[mVideoSizeConfig.srcFrameWidth
                * mVideoSizeConfig.srcFrameHeight * 3 / 2];
        byte[] m_I420Data = new byte[mVideoSizeConfig.srcFrameWidth
                * mVideoSizeConfig.srcFrameHeight * 3 / 2];
        byte[] m_RotateData = new byte[mVideoSizeConfig.srcFrameWidth
                * mVideoSizeConfig.srcFrameHeight * 3 / 2];
        byte[] m_MirrorData = new byte[mVideoSizeConfig.srcFrameWidth
                * mVideoSizeConfig.srcFrameHeight * 3 / 2];
        byte[] m_BeautyData = new byte[mVideoSizeConfig.srcFrameWidth
                * mVideoSizeConfig.srcFrameHeight * 3 / 2];

        @Override
        public void run() {
            // TODO Auto-generated method stub
            super.run();

            VideoCaptureInterface.GetFrameDataReturn ret;
            while (!m_bExit) {
                try {
                    Thread.sleep(1, 10);
                    if (m_bExit) {
                        break;
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                ret = mVideoCapture.GetFrameData(m_nv21Data,
                        m_nv21Data.length);
                if (ret == VideoCaptureInterface.GetFrameDataReturn.RET_SUCCESS) {
                    frameCount++;
                    LibJniVideoProcess.NV21TOI420(mVideoSizeConfig.srcFrameWidth, mVideoSizeConfig.srcFrameHeight, m_nv21Data, m_I420Data);
                    if (curCameraType == VideoCaptureInterface.CameraDeviceType.CAMERA_FACING_FRONT) {
                        LibJniVideoProcess.MirrorI420(mVideoSizeConfig.srcFrameWidth, mVideoSizeConfig.srcFrameHeight, m_I420Data, m_MirrorData);
                        LibJniVideoProcess.RotateI420(mVideoSizeConfig.srcFrameWidth, mVideoSizeConfig.srcFrameHeight, m_MirrorData, m_RotateData, 90);
                    } else if (curCameraType == VideoCaptureInterface.CameraDeviceType.CAMERA_FACING_BACK) {
                        LibJniVideoProcess.RotateI420(mVideoSizeConfig.srcFrameWidth, mVideoSizeConfig.srcFrameHeight, m_I420Data, m_RotateData, 90);
                    }
                    encodeVideo(m_RotateData, m_RotateData.length);
                }
            }
        }


        public void stopThread() {
            m_bExit = true;
        }
    }


    /**
     * 音频采集线程
     */
    class AudioThread extends Thread {

        public volatile boolean m_bExit = false;

        @Override
        public void run() {
            // TODO Auto-generated method stub
            super.run();

            int[] dataLength;
            byte[] audioBuffer;
            AudioCaptureInterface.GetAudioDataReturn ret;

            dataLength = new int[1];
            audioBuffer = new byte[m_aiBufferLength[0]];


            while (!m_bExit) {
                try {
                    Thread.sleep(1, 10);
                    if (m_bExit) {
                        break;
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                try {
                    ret = mAudioCapture.GetAudioData(audioBuffer,
                            m_aiBufferLength[0], dataLength);
                    if (ret == AudioCaptureInterface.GetAudioDataReturn.RET_SUCCESS) {
                        encodeAudio(audioBuffer, dataLength[0]);

                    }

                } catch (Exception e) {
                    e.printStackTrace();
                    stopThread();
                }
            }
        }

        public void stopThread() {
            m_bExit = true;
            if (null != stream) {
                try {
                    stream.flush();
                    stream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * 采集的YUV视频数据
     *
     * @param videoBuffer
     * @param length
     */
    public void encodeVideo(byte[] videoBuffer, int length) {
        try {
            LiveJniMediaManager.EncodeH264(videoBuffer, length);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 采集的PCM音频数据
     *
     * @param audioBuffer
     * @param length
     */
    public void encodeAudio(byte[] audioBuffer, int length) {
        try {
            LiveJniMediaManager.EncodeAAC(audioBuffer, length);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * 相机切换
     *
     * @param type
     * @param micIndex
     */
    public void switchCamera(int type, short micIndex) {
        try {
            if (s_lCameraInfoList == null) {
                s_lCameraInfoList = mVideoCapture.EnumerateCameraDevice();
            }
            int[] iBufferLen = new int[1];
            int[] iOpenResult = new int[1];
            if (s_lCameraInfoList.size() == 1) {
                singleCamera = true;
                if (s_lCameraInfoList.get(0).m_nType != type) {
                    curCameraType = CAMERA_FACING_BACK;
                }
            } else {
                for (int i = 0; i < s_lCameraInfoList.size(); i++) {
                    if (s_lCameraInfoList.get(i).m_nType == type) {
                        break;
                    }
                }
            }
            openCamera(iBufferLen, curCameraType);
        } catch (Exception e) {
            e.printStackTrace();
        } finally {

        }
    }


    /**
     * 打开相机
     *
     * @param iBufferLen
     * @param i
     * @return
     */
    public boolean openCamera(int[] iBufferLen, int i) {
        mVideoCapture.CloseVideoDevice();
        VideoCaptureInterface.OpenVideoDeviceReturn retVideo = null;
        retVideo = mVideoCapture
                .OpenVideoDevice(
                        i,
                        mVideoSizeConfig.srcFrameWidth,
                        mVideoSizeConfig.srcFrameHeight,
                        25,
                        iBufferLen);

        if (retVideo == VideoCaptureInterface.OpenVideoDeviceReturn.OPEN_DEVICE_SUCCESS) {
            maxZoomLevel = s_lCameraInfoList.get(i).m_nMaxZoom;
            currentZoomLevel = 0;
            isLight = false;
            m_sCameraID = (short) i;
            mVideoCapture.SetSurfaceHolder(mSurfaceHolder);
            mVideoCapture.StartVideoCapture();
            if (m_oVideoThread == null
                    || m_oVideoThread.isAlive() == false) {
                m_oVideoThread = new VideoThread();
                m_oVideoThread.start();
            }
            return true;
        }
        return false;
    }

    /**
     * 闪光灯
     *
     * @param
     */
    public boolean Light() {
        if (curCameraType == VideoCaptureInterface.CameraDeviceType.CAMERA_FACING_FRONT) {
            return false;
        }
        if (null != mVideoCapture) {
            isLight = !isLight;
            mVideoCapture.TurnFlash(isLight);
        }
        return true;

    }

    /**
     * 切换摄像头
     */
    public void SwitchCamera() {
        if (singleCamera) {
            return;
        }
        if (curCameraType == VideoCaptureInterface.CameraDeviceType.CAMERA_FACING_FRONT) {
            curCameraType = CAMERA_FACING_BACK;
            LiveJniMediaManager.SetCameraID(LiveJniMediaManager.CameraID.BACK.ordinal());
        } else {
            curCameraType = VideoCaptureInterface.CameraDeviceType.CAMERA_FACING_FRONT;
            LiveJniMediaManager.SetCameraID(LiveJniMediaManager.CameraID.FRONT.ordinal());
        }
        switchCamera(curCameraType, currentMicIndex);
    }

    public void switchBeauty() {
        beautyEffect = !beautyEffect;
        if(beautyEffect) {
            Toast.makeText(mContext, "开启美颜", Toast.LENGTH_LONG).show();
        }else{
            Toast.makeText(mContext, "关闭美颜", Toast.LENGTH_LONG).show();
        }
    }


}
