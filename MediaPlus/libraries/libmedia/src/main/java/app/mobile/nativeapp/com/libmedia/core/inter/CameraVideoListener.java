package app.mobile.nativeapp.com.libmedia.core.inter;


import java.io.File;

import app.mobile.nativeapp.com.libmedia.core.utils.Size;

/**
 * Created by android on 3/21/18.
 */
public interface CameraVideoListener {

    void onVideoRecordStarted(Size videoSize);

    void onVideoRecordStopped(File videoFile, OnCameraResultListener callback);

    void onVideoRecordError();
}
