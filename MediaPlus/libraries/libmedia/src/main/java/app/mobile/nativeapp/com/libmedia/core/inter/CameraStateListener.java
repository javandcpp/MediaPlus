package app.mobile.nativeapp.com.libmedia.core.inter;

import java.io.File;

/**
 * Created by android on 3/21/18.
 */
public interface CameraStateListener {

    void onCurrentCameraBack();

    void onCurrentCameraFront();

    void onFlashAuto();

    void onFlashOn();

    void onFlashOff();

    void onCameraSetupForPhoto();

    void onCameraSetupForVideo();

    void onRecordStateVideoReadyForRecord();

    void onRecordStateVideoInProgress();

    void onRecordStatePhoto();

    void shouldRotateControls(int degrees);

    void onStartVideoRecord(File outputFile);

    void onStopVideoRecord();

    void onCountDownOver();

    void onRecordTooShort();
}
