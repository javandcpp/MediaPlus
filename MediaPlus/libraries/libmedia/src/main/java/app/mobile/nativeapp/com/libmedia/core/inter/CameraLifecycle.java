package app.mobile.nativeapp.com.libmedia.core.inter;

import android.os.Bundle;
import android.support.annotation.Nullable;

import java.io.File;

import app.mobile.nativeapp.com.libmedia.core.camera.ICameraManager;
import app.mobile.nativeapp.com.libmedia.core.config.CameraConfig;


public interface CameraLifecycle<CameraId> {

    void onCreate(Bundle savedInstanceState);

    void onResume();

    void onPause();

    void onDestroy();

    void takePhoto(OnCameraResultListener resultListener);

    void takePhoto(OnCameraResultListener callback, @Nullable String direcoryPath, @Nullable String fileName);

    void startVideoRecord();

    void startVideoRecord(@Nullable String direcoryPath, @Nullable String fileName);

    void stopVideoRecord(OnCameraResultListener callback);

    boolean isVideoRecording();

    void switchCamera(@CameraConfig.CameraFace int cameraFace);

    void switchQuality();

    void setFlashMode(@CameraConfig.FlashMode int flashMode);

    int getNumberOfCameras();

    @CameraConfig.MediaAction
    int getMediaAction();

    CameraId getCameraId();

    CameraId getFrontCameraId();

    File getOutputFile();

    ICameraManager getCameraManager();

    CharSequence[] getVideoQualityOptions();

    CharSequence[] getPhotoQualityOptions();

    void handlerFocus(float x, float y);
}
