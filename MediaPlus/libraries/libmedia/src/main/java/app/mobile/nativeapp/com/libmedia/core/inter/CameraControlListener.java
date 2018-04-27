package app.mobile.nativeapp.com.libmedia.core.inter;

public interface CameraControlListener {
    void lockControls();

    void unLockControls();

    void allowCameraSwitching(boolean allow);

    void allowRecord(boolean allow);

    void setMediaActionSwitchVisible(boolean visible);
}
