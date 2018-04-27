package app.mobile.nativeapp.com.libmedia.core.inter;

import android.support.annotation.Nullable;
import android.view.View;

import app.mobile.nativeapp.com.libmedia.core.config.CameraConfig;
import app.mobile.nativeapp.com.libmedia.core.utils.Size;


public interface CameraView {
    void updateCameraPreview(Size size, View cameraPreview);

    void updateUiForMediaAction(@CameraConfig.MediaAction int mediaAction);

    void updateCameraSwitcher(int numberOfCameras);

    void onPictureTaken(byte[] bytes, @Nullable OnCameraResultListener callback);

    void onVideoRecordStart(int width, int height);

    void onVideoRecordStop(@Nullable OnCameraResultListener callback);

    void releaseCameraPreview();
}
