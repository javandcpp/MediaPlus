package app.mobile.nativeapp.com.libmedia.core.inter;

/**
 * Created by android on 4/16/18.
 */


public interface OnCameraResultListener {

    void onVideoRecorded(String filePath, boolean tooShort);

    void onPhotoTaken(byte[] bytes, String filePath);
}
