//package app.mobile.nativeapp.com.libmedia.core.avcapture.impl;
//
//import android.annotation.SuppressLint;
//import android.graphics.ImageFormat;
//import android.graphics.Rect;
//import android.graphics.SurfaceTexture;
//import android.hardware.Camera;
//import android.hardware.Camera.Area;
//import android.hardware.Camera.AutoFocusCallback;
//import android.hardware.Camera.CameraInfo;
//import android.hardware.Camera.Parameters;
//import android.hardware.Camera.PreviewCallback;
//import android.hardware.Camera.Size;
//import android.opengl.GLES11Ext;
//import android.util.Log;
//import android.view.SurfaceHolder;
//
//import java.io.IOException;
//import java.util.ArrayList;
//import java.util.Collections;
//import java.util.Comparator;
//import java.util.LinkedList;
//import java.util.List;
//import java.util.Queue;
//import java.util.Vector;
//
//import app.mobile.nativeapp.com.libmedia.core.avcapture.VideoCaptureInterface;
//
//
//public class VideoCapture implements Comparator<Size>, PreviewCallback, VideoCaptureInterface {
//
//    private static final int VERSION = 2017204;
//    private static final int SIDE_LEN = 100;
//    private static final int MAX_LEN = 1000;
//    private static int m_nNum = 0;
//    private static List<CameraDeviceInfo> listInfo = null;
//
//    private int m_nLength;
//    private int m_nMaxZoom;
//    private int m_nDuration;
//    private long m_nSumTime;
//    private long m_nPrevTime, m_nCurrentTime;
//    private Object m_syncLockObject = new Object();
//    private volatile boolean m_bIsOpenCamera = false;
//    private volatile boolean m_bIsStartVideoCapture = false;
//    private Queue<byte[]> m_qVideoData = new LinkedList<byte[]>();
//
//    private byte[] m_nv21 = null;
//    private Camera m_camera = null;
//    private SurfaceHolder m_holder = null;
//    private SurfaceTexture m_texture = null;
//
//    @Override
//    public int compare(Size lhs, Size rhs) {
//        if (lhs.width == rhs.width) {
//            if (lhs.height < rhs.height) {
//                return -1;
//            } else if (lhs.height == rhs.height) {
//                return 0;
//            } else {
//                return 1;
//            }
//        } else if (lhs.width < rhs.width) {
//            return -1;
//        } else {
//            return 1;
//        }
//    }
//
//    @Override
//    public int GetAVCaptureVersion() {
//        // TODO Auto-generated method stub
//        return VERSION;
//    }
//
//    @Override
//    public List<CameraDeviceInfo> EnumerateCameraDevice() {
//        Camera camera = null;
//        Parameters param = null;
//        List<String> flashModes = null;
//        m_nNum = Camera.getNumberOfCameras();
//        CameraDeviceInfo cameraDeviceInfo = null;
//        CameraInfo cameraInfo = new CameraInfo();
//        List<CameraDeviceInfo> listCameraDeviceInfo = new Vector<CameraDeviceInfo>();
//        for (int i = 0; i < m_nNum; i++) {
//            camera = Camera.open(i);
//            Camera.getCameraInfo(i, cameraInfo);
//            param = camera.getParameters();
//            cameraDeviceInfo = new CameraDeviceInfo();
//            cameraDeviceInfo.m_nID = i;
//            cameraDeviceInfo.m_nType = cameraInfo.facing;
//            cameraDeviceInfo.m_nRotation = cameraInfo.orientation;
//            cameraDeviceInfo.m_lsPreviewSize = param.getSupportedPreviewSizes();
//            cameraDeviceInfo.m_liFPSRange = param.getSupportedPreviewFpsRange().get(0);
//            cameraDeviceInfo.m_nMaxZoom = 0;
//            if (param.isZoomSupported()) {
//                cameraDeviceInfo.m_nMaxZoom = param.getMaxZoom();
//            }
//            flashModes = param.getSupportedFlashModes();
//            if (flashModes == null || cameraInfo.facing == CameraInfo.CAMERA_FACING_FRONT) {
//                cameraDeviceInfo.m_bFlash = false;
//            } else if (flashModes.contains(Parameters.FLASH_MODE_TORCH)
//                    && flashModes.contains(Parameters.FLASH_MODE_OFF)) {
//                cameraDeviceInfo.m_bFlash = true;
//            }
//            Collections.sort(cameraDeviceInfo.m_lsPreviewSize, this);
//            listCameraDeviceInfo.add(cameraDeviceInfo);
//            camera.release();
//        }
//        listInfo = listCameraDeviceInfo;
//
//        return listCameraDeviceInfo;
//    }
//
//    @SuppressWarnings("deprecation")
//    @SuppressLint("InlinedApi")
//    @Override
//    public OpenVideoDeviceReturn OpenVideoDevice(int aiID, int aiWidth, int aiHeight, int aiFPS, int[] aiBufferLen) {
//        // TODO Auto-generated method stub
//        m_qVideoData.clear();
//        if (m_bIsOpenCamera) {
//            return OpenVideoDeviceReturn.OPEN_ERROR_OCCUPIED;
//        } else if (aiID >= m_nNum) {
//            return OpenVideoDeviceReturn.OPEN_ERROR_TYPE;
//        } else if (m_syncLockObject == null || m_qVideoData == null) {
//            return OpenVideoDeviceReturn.OPEN_ERROR_BUFFER;
//        }
//
//        m_camera = Camera.open(aiID);
//        if (m_camera == null) {
//            return OpenVideoDeviceReturn.OPEN_ERROR_UNKNOWN;
//        }
//
//        m_bIsOpenCamera = true;
//        Parameters params = m_camera.getParameters();
//        int[] range = listInfo.get(aiID).m_liFPSRange;
//        List<Size> supportedPreviewSizeList = params.getSupportedPreviewSizes();
//        for (int j = 0; j < supportedPreviewSizeList.size(); j++) {
//            Size size = supportedPreviewSizeList.get(j);
//            if (size.width == aiWidth && size.height == aiHeight) {
//                params.setPreviewFormat(ImageFormat.NV21);
//                int nFormat = params.getPreviewFormat();
//                aiBufferLen[0] = m_nLength = aiWidth * aiHeight * ImageFormat.getBitsPerPixel(nFormat) / 8;
//                m_nMaxZoom = params.getMaxZoom();
//                m_camera.setDisplayOrientation(90);
//                params.setPreviewSize(aiWidth, aiHeight);
//                if (aiFPS <= 0) {
//                    aiFPS = 1;
//                }
//                m_nSumTime = 0;
//                m_nDuration = 1000 / aiFPS;
//                if (aiFPS * 1000 < range[0]) {
//                    params.setPreviewFrameRate((int) (range[0] / 1000.0 + 0.9));
//                } else if (aiFPS * 1000 < range[1]) {
//                    params.setPreviewFrameRate(aiFPS);
//                } else {
//                    params.setPreviewFrameRate(range[0] / 1000);
//                }
//                List<String> focusModes = params.getSupportedFocusModes();
//                if (focusModes.contains(Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
//                    params.setFocusMode(Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
//                } else if (focusModes.contains(Parameters.FOCUS_MODE_AUTO)) {
//                    params.setFocusMode(Parameters.FOCUS_MODE_AUTO);
//                }
//                /*
//                 * if (Build.VERSION.SDK_INT >= 17) { List<String> sceneModes =
//				 * params.getSupportedSceneModes(); if
//				 * (sceneModes.contains(Parameters.SCENE_MODE_HDR))
//				 * params.setSceneMode(Parameters.SCENE_MODE_HDR); }
//				 */
//                m_camera.setParameters(params);
//                m_nv21 = new byte[m_nLength];
//                return OpenVideoDeviceReturn.OPEN_DEVICE_SUCCESS;
//            }
//        }
//
//        CloseVideoDevice();
//        return OpenVideoDeviceReturn.OPEN_ERROR_SIZE;
//    }
//
//    @Override
//    public boolean IsOpenVideoDevice() {
//        // TODO Auto-generated method stub
//        return m_bIsOpenCamera;
//    }
//
//    @Override
//    public void CloseVideoDevice() {
//        // TODO Auto-generated method stub
//        if (m_bIsOpenCamera) {
//            StopVideoCapture();
//            m_camera.release();
//            m_nv21 = null;
//            m_camera = null;
//            m_holder = null;
//            m_texture = null;
//            m_bIsOpenCamera = false;
//        }
//    }
//
//    @Override
//    public void SetSurfaceHolder(SurfaceHolder aoHolder) {
//        // TODO Auto-generated method stub
//        m_holder = aoHolder;
//    }
//
//    @SuppressLint("NewApi")
//    @Override
//    public StartVideoCaptureReturn StartVideoCapture() {
//        // TODO Auto-generated method stub
//        if (!m_bIsOpenCamera) {
//            return StartVideoCaptureReturn.STATUS_NO_DEVICE;
//        } else if (m_bIsStartVideoCapture) {
//            return StartVideoCaptureReturn.STATUS_SUCCESS;
//        }
//
//        if (m_holder == null) {
//            if (m_texture == null) {
//                m_texture = new SurfaceTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES);
//            }
//            try {
//                m_camera.setPreviewTexture(m_texture);
//            } catch (IOException e) {
//                // TODO Auto-generated catch block
//                e.printStackTrace();
//            }
//        } else {
//            try {
//                m_camera.setPreviewDisplay(m_holder);
//            } catch (IOException e) {
//                // TODO Auto-generated catch block
//                e.printStackTrace();
//            }
//        }
//
//        m_camera.addCallbackBuffer(m_nv21);
//        m_camera.setPreviewCallbackWithBuffer(this);
//        m_camera.startPreview();
//        m_bIsStartVideoCapture = true;
//        m_nPrevTime = System.currentTimeMillis();
//        return StartVideoCaptureReturn.STATUS_SUCCESS;
//    }
//
//    @Override
//    public boolean IsStartVideoCapture() {
//        // TODO Auto-generated method stub
//        return m_bIsStartVideoCapture;
//    }
//
//    @Override
//    public void StopVideoCapture() {
//        // TODO Auto-generated method stub
//        if (m_bIsStartVideoCapture) {
//            m_camera.setPreviewCallback(null);
//            m_camera.stopPreview();
//            m_bIsStartVideoCapture = false;
//        }
//    }
//
//    @Override
//    public GetFrameDataReturn GetFrameData(byte[] abyBuffer, int aiLength) {
//        // TODO Auto-generated method stub
//        if (!m_bIsStartVideoCapture) {
//            return GetFrameDataReturn.RET_CAPTURE_NO_START;
//        } else if (abyBuffer == null || aiLength < m_nLength) {
//            return GetFrameDataReturn.RET_ERROR_PARAM;
//        } else {
//            synchronized (m_syncLockObject) {
//                byte[] frameBuffer = m_qVideoData.poll();
//                if (frameBuffer == null) {
//                    return GetFrameDataReturn.RET_NO_VIDEO_DATA;
//                }
//                System.arraycopy(frameBuffer, 0, abyBuffer, 0, m_nLength);
//            }
//        }
//        return GetFrameDataReturn.RET_SUCCESS;
//    }
//
//    @Override
//    public void onPreviewFrame(byte[] data, Camera camera) {
//        // TODO Auto-generated method stub
//        synchronized (m_syncLockObject) {
//            if (m_qVideoData.size() == 2) {
//                m_qVideoData.poll();
//            }
//            m_nCurrentTime = System.currentTimeMillis();
//            m_nSumTime = m_nSumTime + m_nCurrentTime - m_nPrevTime;
//            if (m_nSumTime > m_nDuration) {
//                m_qVideoData.offer(data);
//                m_nSumTime = m_nSumTime % m_nDuration;
//            }
//            m_nPrevTime = m_nCurrentTime;
//        }
//        m_camera.addCallbackBuffer(m_nv21);
//    }
//
//    AutoFocusCallback m_oFocusCallback = new AutoFocusCallback() {
//        @Override
//        public void onAutoFocus(boolean paramBoolean, Camera paramCamera) {
//        }
//    };
//
//    private void limitRect(Rect rect) {
//        if (rect.left < -MAX_LEN) {
//            rect.left = -MAX_LEN;
//        }
//        if (rect.top < -MAX_LEN) {
//            rect.top = -MAX_LEN;
//        }
//        if (MAX_LEN < rect.right) {
//            rect.right = MAX_LEN;
//        }
//        if (MAX_LEN < rect.bottom) {
//            rect.bottom = MAX_LEN;
//        }
//    }
//
//    private void dumpRect(Rect rect) {
//        Log.i("VideoCapture", "rect left: " + rect.left);
//        Log.i("VideoCapture", "rect top: " + rect.top);
//        Log.i("VideoCapture", "rect right: " + rect.right);
//        Log.i("VideoCapture", "rect bottom: " + rect.bottom);
//    }
//
//    @SuppressLint({"NewApi"})
//    @Override
//    public StatusReturn FocusByCameraPoint(int aiPointX, int aiPointY) {
//        if (m_camera == null) {
//            return StatusReturn.RET_NO_CAMERA;
//        }
//
//        Parameters param = m_camera.getParameters();
//        if (param.getMaxNumFocusAreas() <= 0) {
//            return StatusReturn.RET_NOT_SUPPORTED;
//        }
//
//        m_camera.cancelAutoFocus();
//        ArrayList<Area> list = new ArrayList<Area>();
//        Rect localRect = new Rect(aiPointX - SIDE_LEN, aiPointY - SIDE_LEN, aiPointX + SIDE_LEN, aiPointY + SIDE_LEN);
//        limitRect(localRect);
//        list.add(new Area(localRect, MAX_LEN));
//        param.setFocusMode(Parameters.FOCUS_MODE_AUTO);
//        param.setFocusAreas(list);
//        try {
//            m_camera.setParameters(param);
//            m_camera.autoFocus(m_oFocusCallback);
//            dumpRect(localRect);
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//
//        return StatusReturn.RET_SUCCESS;
//    }
//
//    @Override
//    public StatusReturn SetZoomLevel(int aiZoomLevel) {
//        // TODO Auto-generated method stub
//        if (m_camera == null) {
//            return StatusReturn.RET_NO_CAMERA;
//        }
//
//        Parameters params = m_camera.getParameters();
//        if (!params.isZoomSupported()) {
//            return StatusReturn.RET_NOT_SUPPORTED;
//        }
//
//        if (aiZoomLevel > m_nMaxZoom) {
//            aiZoomLevel = m_nMaxZoom;
//        }
//        params.setZoom(aiZoomLevel);
//        m_camera.setParameters(params);
//        return StatusReturn.RET_SUCCESS;
//    }
//
//    @Override
//    public StatusReturn TurnFlash(boolean abStatus) {
//        // TODO Auto-generated method stub
//        if (m_camera == null) {
//            return StatusReturn.RET_NO_CAMERA;
//        }
//
//        String flashMode = null;
//        Parameters param = m_camera.getParameters();
//        List<String> flashModes = param.getSupportedFlashModes();
//        if (flashModes == null) {
//            return StatusReturn.RET_NOT_SUPPORTED;
//        }
//
//        if (abStatus) {
//            flashMode = Parameters.FLASH_MODE_TORCH;
//        } else {
//            flashMode = Parameters.FLASH_MODE_OFF;
//        }
//        if (!flashModes.contains(flashMode)) {
//            return StatusReturn.RET_NOT_SUPPORTED;
//        } else if (flashMode != param.getFlashMode()) {
//            param.setFlashMode(flashMode);
//            m_camera.setParameters(param);
//        }
//
//        return StatusReturn.RET_SUCCESS;
//    }
//
//    @Override
//    public Size getSuggestCapSize(CameraDeviceInfo captureInfo, boolean isPortrait,
//                                  int resultWidth, int resultHeight) {
//        if (captureInfo == null) {
//            return null;
//        }
//
//        int rotation = captureInfo.m_nRotation;
//        int capWidth;
//        int capHeight;
//
//        if (isPortrait) {
//            capWidth = resultHeight;
//            capHeight = resultWidth;
//        } else {
//            capWidth = resultWidth;
//            capHeight = resultHeight;
//        }
//
//        for (Size size : captureInfo.m_lsPreviewSize) {
//            if (size.width >= capWidth && size.height >= capHeight) {
//                return size;
//            }
//        }
//
//        return null;
//
//    }
//
//	/*
//     * @Override public StatusReturn GetExposureCompensation(int[] value) { //
//	 * TODO Auto-generated method stub if (m_camera == null) return
//	 * StatusReturn.RET_NO_CAMERA;
//	 *
//	 * Parameters param = m_camera.getParameters(); value[0] =
//	 * param.getExposureCompensation(); return StatusReturn.RET_SUCCESS; }
//	 *
//	 * @Override public StatusReturn SetExposureCompensation(int value) { //
//	 * TODO Auto-generated method stub if (m_camera == null) return
//	 * StatusReturn.RET_NO_CAMERA;
//	 *
//	 * Parameters param = m_camera.getParameters();
//	 * param.setExposureCompensation(value); return StatusReturn.RET_SUCCESS; }
//	 */
//}
