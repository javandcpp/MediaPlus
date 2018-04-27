package app.mobile.nativeapp.com.libmedia.core.camera;

import android.content.Context;

import java.io.File;

import app.mobile.nativeapp.com.libmedia.core.config.CameraConfig;
import app.mobile.nativeapp.com.libmedia.core.inter.CameraCloseListener;
import app.mobile.nativeapp.com.libmedia.core.inter.CameraConfigProvider;
import app.mobile.nativeapp.com.libmedia.core.inter.CameraOpenListener;
import app.mobile.nativeapp.com.libmedia.core.inter.CameraPictureListener;
import app.mobile.nativeapp.com.libmedia.core.inter.CameraVideoListener;
import app.mobile.nativeapp.com.libmedia.core.inter.OnCameraResultListener;
import app.mobile.nativeapp.com.libmedia.core.utils.Size;

/**
 *
 */
public interface ICameraManager<CameraId, SurfaceListener> {

    void initializeCameraManager(CameraConfigProvider cameraConfigProvider, Context context);

    void releaseCameraManager();

    void openCamera(CameraId cameraId, CameraOpenListener<CameraId, SurfaceListener> cameraOpenListener);

    void closeCamera(CameraCloseListener<CameraId> cameraCloseListener);

    void takePicture(File photoFile, CameraPictureListener cameraPictureListener, OnCameraResultListener callback);


    boolean isVideoRecording();

    void setCameraId(CameraId cameraId);

    void setFlashMode(@CameraConfig.FlashMode int flashMode);

    CameraId getCameraId();

    CameraId getFaceFrontCameraId();

    CameraId getFaceBackCameraId();

    int getNumberOfCameras();

    int getFaceFrontCameraOrientation();

    int getFaceBackCameraOrientation();

    Size getPictureSizeForQuality(@CameraConfig.MediaQuality int mediaQuality);

    CharSequence[] getPictureQualityOptions();

    CharSequence[] getVideoQualityOptions();

    void handlerFocus(float x, float y);

    void startPreview();

}
