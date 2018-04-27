package app.mobile.nativeapp.com.libmedia.core.camera;

import android.content.Context;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Camera;
import android.media.CamcorderProfile;
import android.media.MediaRecorder;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Process;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;
import java.util.List;

import app.mobile.nativeapp.com.libmedia.core.config.CameraConfig;
import app.mobile.nativeapp.com.libmedia.core.inter.CameraConfigProvider;
import app.mobile.nativeapp.com.libmedia.core.utils.LogUtils;
import app.mobile.nativeapp.com.libmedia.core.utils.Size;


/**
 * The base camera manager
 */
abstract class BaseCameraManager<CameraId, SurfaceListener> implements ICameraManager<CameraId, SurfaceListener>
         {

    private static final String TAG = "BaseCameraManager";

    private final int MIN_PREVIEW_PIXELS = 480 * 320;
    private final double MAX_ASPECT_DISTORTION = 0.15;
    Context mContext;
    CameraConfigProvider cameraConfigProvider;

    MediaRecorder mMediaRecorder;
    boolean mIsVideoRecording = false;
    CameraId mCameraId = null;
    CameraId mFaceFrontCameraId = null;
    CameraId mFaceBackCameraId = null;
    int mNumberOfCameras = 0;
    int mFaceFrontCameraOrientation;
    int mFaceBackCameraOrientation;

    Size mPhotoSize;
    Size mVideoSize;
    Size mPreviewSize;
    Size mWindowSize;
    CamcorderProfile mCamcorderProfile;

    HandlerThread mBackgroundThread;
    Handler mBackgroundHandler;
    Handler mUiiHandler = new Handler(Looper.getMainLooper());

    @Override
    public void initializeCameraManager(CameraConfigProvider cameraConfigProvider, Context context) {
        this.mContext = context;
        this.cameraConfigProvider = cameraConfigProvider;
        startBackgroundThread();
    }

    @Override
    public void releaseCameraManager() {
        this.mContext = null;
        stopBackgroundThread();
    }

    protected abstract void prepareCameraOutputs();

    protected abstract int getPhotoOrientation(@CameraConfig.SensorPosition int sensorPosition);

    protected abstract int getVideoOrientation(@CameraConfig.SensorPosition int sensorPosition);

    protected void releaseVideoRecorder() {
        try {
            if (mMediaRecorder != null) {
                mMediaRecorder.reset();
                mMediaRecorder.release();
            }
        } catch (Exception ignore) {

        } finally {
            mMediaRecorder = null;
        }
    }

    private void startBackgroundThread() {
        mBackgroundThread = new HandlerThread(TAG, Process.THREAD_PRIORITY_BACKGROUND);
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    private void stopBackgroundThread() {
        if (Build.VERSION.SDK_INT > 17) {
            mBackgroundThread.quitSafely();
        } else {
            mBackgroundThread.quit();
        }

        try {
            mBackgroundThread.join();
        } catch (InterruptedException e) {
            LogUtils.e(TAG, "stopBackgroundThread: ");
        } finally {
            mBackgroundThread = null;
            mBackgroundHandler = null;
        }
    }



    protected Rect calculateTapArea(float x, float y, float coefficient, Camera.Size previewSize) {
        float focusAreaSize = 300;
        int areaSize = Float.valueOf(focusAreaSize * coefficient).intValue();
        int centerX = (int) (x / previewSize.width - 1000);
        int centerY = (int) (y / previewSize.height - 1000);
        int left = clamp(centerX - areaSize / 2, -1000, 1000);
        int top = clamp(centerY - areaSize / 2, -1000, 1000);
        RectF rectF = new RectF(left, top, left + areaSize, top + areaSize);
        return new Rect(Math.round(rectF.left), Math.round(rectF.top), Math.round(rectF.right), Math.round(rectF.bottom));
    }

    private int clamp(int x, int min, int max) {
        if (x > max) {
            return max;
        }
        if (x < min) {
            return min;
        }
        return x;
    }

    @Override
    public boolean isVideoRecording() {
        return mIsVideoRecording;
    }

    @Override
    public CameraId getCameraId() {
        return mCameraId;
    }

    @Override
    public CameraId getFaceFrontCameraId() {
        return mFaceFrontCameraId;
    }

    @Override
    public CameraId getFaceBackCameraId() {
        return mFaceBackCameraId;
    }

    @Override
    public int getNumberOfCameras() {
        return mNumberOfCameras;
    }

    @Override
    public int getFaceFrontCameraOrientation() {
        return mFaceFrontCameraOrientation;
    }

    @Override
    public int getFaceBackCameraOrientation() {
        return mFaceBackCameraOrientation;
    }

    @Override
    public void setCameraId(CameraId currentCameraId) {
        this.mCameraId = currentCameraId;
    }

    /**
     * 找出最适合的预览界面分辨率
     *
     * @param cameraParameters
     * @param screenResolution
     * @param screenOrientation
     * @param cameraOrientation
     * @return
     */
    public Point findBestPreviewResolution(Camera.Parameters cameraParameters, Point screenResolution, int screenOrientation, int cameraOrientation) {
        Camera.Size defaultPreviewResolution = cameraParameters.getPreviewSize(); //默认的预览尺寸
        List<Camera.Size> rawSupportedSizes = cameraParameters.getSupportedPreviewSizes();
        if (rawSupportedSizes == null) {
            return new Point(defaultPreviewResolution.width, defaultPreviewResolution.height);
        }
        // 按照分辨率从大到小排序
        List<Camera.Size> supportedPreviewResolutions = new ArrayList<Camera.Size>(rawSupportedSizes);
        Collections.sort(supportedPreviewResolutions, new Comparator<Camera.Size>() {
            @Override
            public int compare(Camera.Size a, Camera.Size b) {
                int aPixels = a.height * a.width;
                int bPixels = b.height * b.width;
                if (bPixels < aPixels) {
                    return -1;
                }
                if (bPixels > aPixels) {
                    return 1;
                }
                return 0;
            }
        });
        // 在camera分辨率与屏幕分辨率宽高比不相等的情况下，找出差距最小的一组分辨率
        // 由于camera的分辨率是width>height，这里先判断我们的屏幕和相机的角度是不是相同的方向(横屏 or 竖屏),然后决定比较的时候要不要先交换宽高值
        boolean isCandidatePortrait = screenOrientation % 180 != cameraOrientation % 180;
        double screenAspectRatio = (double) screenResolution.x / (double) screenResolution.y;
        // 移除不符合条件的分辨率
        Iterator<Camera.Size> it = supportedPreviewResolutions.iterator();
        while (it.hasNext()) {
            Camera.Size supportedPreviewResolution = it.next();
            int width = supportedPreviewResolution.width;
            int height = supportedPreviewResolution.height;
            // 移除低于下限的分辨率，尽可能取高分辨率
            if (width * height < MIN_PREVIEW_PIXELS) {
                it.remove();
                continue;
            }
            //移除宽高比差异较大的
            int maybeFlippedWidth = isCandidatePortrait ? height : width;
            int maybeFlippedHeight = isCandidatePortrait ? width : height;
            double aspectRatio = (double) maybeFlippedWidth / (double) maybeFlippedHeight;
            double distortion = Math.abs(aspectRatio - screenAspectRatio);
            if (distortion > MAX_ASPECT_DISTORTION) {
                it.remove();
                continue;
            }
            // 找到与屏幕分辨率完全匹配的预览界面分辨率直接返回
            if (maybeFlippedWidth == screenResolution.x && maybeFlippedHeight == screenResolution.y) {
                Point exactPoint = new Point(width, height);
                return exactPoint;
            }
        }
        // 如果没有找到合适的，并且还有候选的像素，则设置其中最大比例的，对于配置比较低的机器不太合适
        if (!supportedPreviewResolutions.isEmpty()) {
            Camera.Size largestPreview = supportedPreviewResolutions.get(0);
            Point largestSize = new Point(largestPreview.width, largestPreview.height);
            return largestSize;
        }
        // 没有找到合适的，就返回默认的
        Point defaultResolution = new Point(defaultPreviewResolution.width, defaultPreviewResolution.height);
        return defaultResolution;
    }

}
