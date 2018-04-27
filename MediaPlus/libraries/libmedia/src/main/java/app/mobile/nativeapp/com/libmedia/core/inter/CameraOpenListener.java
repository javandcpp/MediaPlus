package app.mobile.nativeapp.com.libmedia.core.inter;

import app.mobile.nativeapp.com.libmedia.core.utils.Size;

/**
 * Created by android on 4/16/18.
 */

public interface CameraOpenListener<CameraId, SurfaceListener> {

    void onCameraOpened(CameraId openedCameraId, Size previewSize, SurfaceListener surfaceListener);

    void onCameraOpenError();
}
