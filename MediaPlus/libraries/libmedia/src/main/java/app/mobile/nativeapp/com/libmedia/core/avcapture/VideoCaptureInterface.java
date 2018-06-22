//package app.mobile.nativeapp.com.libmedia.core.avcapture;
//
//import java.util.List;
//
//import android.hardware.Camera.Size;
//import android.view.SurfaceHolder;
//
//public interface VideoCaptureInterface {
//
//    class CameraDeviceInfo {
//        public int m_nID;
//        public int m_nType;
//        public int m_nRotation;
//        public List<Size> m_lsPreviewSize;
//        public int[] m_liFPSRange;
//        public int m_nMaxZoom;
//        public boolean m_bFlash;
//    }
//
//    ;
//
//
//    class CameraDeviceType {
//        public static final int CAMERA_FACING_BACK = 0;
//        public static final int CAMERA_FACING_FRONT = 1;
//    }
//
//    ;
//
//
//    enum OpenVideoDeviceReturn {
//        OPEN_DEVICE_SUCCESS,
//        OPEN_ERROR_OCCUPIED,
//        OPEN_ERROR_TYPE,
//        OPEN_ERROR_SIZE,
//        OPEN_ERROR_BUFFER,
//        OPEN_ERROR_UNKNOWN
//    }
//
//
//    enum StartVideoCaptureReturn {
//        STATUS_SUCCESS,
//        STATUS_NO_DEVICE
//    }
//
//
//    enum GetFrameDataReturn {
//        RET_SUCCESS,
//        RET_CAPTURE_NO_START,
//        RET_ERROR_PARAM,
//        RET_NO_VIDEO_DATA
//    }
//
//
//    enum StatusReturn {
//        RET_SUCCESS,
//        RET_NO_CAMERA,
//        RET_NOT_SUPPORTED
//    }
//
//
//    public int GetAVCaptureVersion();
//
//    public List<CameraDeviceInfo> EnumerateCameraDevice();
//
//    public OpenVideoDeviceReturn OpenVideoDevice(int aiID, int aiWidth, int aiHeight, int aiFPS, int[] aiBufferLen);
//
//    public boolean IsOpenVideoDevice();
//
//    public void CloseVideoDevice();
//
//    public void SetSurfaceHolder(SurfaceHolder aoHolder);
//
//    public StartVideoCaptureReturn StartVideoCapture();
//
//    public boolean IsStartVideoCapture();
//
//    public void StopVideoCapture();
//
//    public GetFrameDataReturn GetFrameData(byte[] abyBuffer, int aiLength);
//
//    public StatusReturn FocusByCameraPoint(int aiPointX, int aiPointY);
//
//    public StatusReturn SetZoomLevel(int aiZoomLevel);
//
//    public StatusReturn TurnFlash(boolean abStatus);
//
//    public Size getSuggestCapSize(CameraDeviceInfo captureInfo, boolean isPortrait, int resultWidth, int resultHeight);
//}
