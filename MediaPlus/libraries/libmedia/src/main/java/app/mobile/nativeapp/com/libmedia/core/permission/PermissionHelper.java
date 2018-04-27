package app.mobile.nativeapp.com.libmedia.core.permission;

import android.annotation.TargetApi;
import android.app.AppOpsManager;
import android.content.Context;
import android.hardware.Camera;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Binder;
import android.os.Build;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import app.mobile.nativeapp.com.libmedia.core.utils.LogUtils;

/**
 * Created by android on 4/5/18.
 */

public final class PermissionHelper {
    public static final int STATE_NO_PERMISSION = -1;
    public static final int STATE_RECORDING = 0;
    public static final int STATE_SUCCESS = 1;
    public static final int OP_POST_NOTIFICATION = 11;
//
//    public static final int OP_NONE = -1;
//    public static final int OP_COARSE_LOCATION = 0;
//    public static final int OP_FINE_LOCATION = 1;
//    public static final int OP_GPS = 2;
//    public static final int OP_VIBRATE = 3;
//    public static final int OP_READ_CONTACTS = 4;
//    public static final int OP_WRITE_CONTACTS = 5;
//    public static final int OP_READ_CALL_LOG = 6;
//    public static final int OP_WRITE_CALL_LOG = 7;
//    public static final int OP_READ_CALENDAR = 8;
//    public static final int OP_WRITE_CALENDAR = 9;
//    public static final int OP_WIFI_SCAN = 10;
//    public static final int OP_NEIGHBORING_CELLS = 12;
//    public static final int OP_CALL_PHONE = 13;
//    public static final int OP_READ_SMS = 14;
//    public static final int OP_WRITE_SMS = 15;
//    public static final int OP_RECEIVE_SMS = 16;
//    public static final int OP_RECEIVE_EMERGECY_SMS = 17;
//    public static final int OP_RECEIVE_MMS = 18;
//    public static final int OP_RECEIVE_WAP_PUSH = 19;
//    public static final int OP_SEND_SMS = 20;
//    public static final int OP_READ_ICC_SMS = 21;
//    public static final int OP_WRITE_ICC_SMS = 22;
//    public static final int OP_WRITE_SETTINGS = 23;
//    public static final int OP_SYSTEM_ALERT_WINDOW = 24;
//    public static final int OP_ACCESS_NOTIFICATIONS = 25;
//    public static final int OP_CAMERA = 26;
//    public static final int OP_RECORD_AUDIO = 27;
//    public static final int OP_PLAY_AUDIO = 28;
//    public static final int OP_READ_CLIPBOARD = 29;
//    public static final int OP_WRITE_CLIPBOARD = 30;
//    public static final int OP_TAKE_MEDIA_BUTTONS = 31;
//    public static final int OP_TAKE_AUDIO_FOCUS = 32;
//    public static final int OP_AUDIO_MASTER_VOLUME = 33;
//    public static final int OP_AUDIO_VOICE_VOLUME = 34;
//    public static final int OP_AUDIO_RING_VOLUME = 35;
//    public static final int OP_AUDIO_MEDIA_VOLUME = 36;
//    public static final int OP_AUDIO_ALARM_VOLUME = 37;
//    public static final int OP_AUDIO_NOTIFICATION_VOLUME = 38;
//    public static final int OP_AUDIO_BLUETOOTH_VOLUME = 39;
//    public static final int OP_WAKE_LOCK = 40;
//    public static final int OP_MONITOR_LOCATION = 41;
//    public static final int OP_MONITOR_HIGH_POWER_LOCATION = 42;
//    public static final int OP_GET_USAGE_STATS = 43;
//    public static final int OP_MUTE_MICROPHONE = 44;
//    public static final int OP_TOAST_WINDOW = 45;
//    public static final int OP_PROJECT_MEDIA = 46;
//    public static final int OP_ACTIVATE_VPN = 47;
//    public static final int OP_WRITE_WALLPAPER = 48;
//    public static final int OP_ASSIST_STRUCTURE = 49;
//    public static final int OP_ASSIST_SCREENSHOT = 50;
//    public static final int OP_READ_PHONE_STATE = 51;
//    public static final int OP_ADD_VOICEMAIL = 52;
//    public static final int OP_USE_SIP = 53;
//    public static final int OP_PROCESS_OUTGOING_CALLS = 54;
//    public static final int OP_USE_FINGERPRINT = 55;
//    public static final int OP_BODY_SENSORS = 56;
//    public static final int OP_READ_CELL_BROADCASTS = 57;
//    public static final int OP_MOCK_LOCATION = 58;
//    public static final int OP_READ_EXTERNAL_STORAGE = 59;Di
//    public static final int OP_WRITE_EXTERNAL_STORAGE = 60;
//    public static final int OP_TURN_SCREEN_ON = 61;

    private static final String TAG = PermissionHelper.class.getSimpleName();

    /**
     * 判断是否有录音权限
     *
     * @return
     */
    public static boolean hasAudioRecordPermission(Context context) {
        if (hasPermission(context, opRecordAudioFlag)) {
            if (getRecordState() == STATE_SUCCESS) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }


    /**
     * 获取录音状态
     *
     * @return
     */
    private static int getRecordState() {
        int minBuffer = AudioRecord.getMinBufferSize(44100, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
        short[] point = new short[minBuffer];
        int readSize = 0;

        AudioRecord audioRecord = null;
        try {
            audioRecord = new AudioRecord(MediaRecorder.AudioSource.DEFAULT, 44100, AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT,
                    (minBuffer * 100));
            // 开始录音 检测是否可以进入初始化状态
            audioRecord.startRecording();

        } catch (Exception e) {
            if (audioRecord != null) {
                audioRecord.release();
                audioRecord = null;
            } else {
            }

            return STATE_NO_PERMISSION;
        }

        // 检测是否在录音中
        if (audioRecord.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
            if (audioRecord != null) {
                audioRecord.stop();
                audioRecord.release();
                audioRecord = null;
            }
            return STATE_RECORDING;

        } else {
            readSize = audioRecord.read(point, 0, point.length);
            // 检测是否可以获取录音结果
            if (readSize <= 0) {
                if (audioRecord != null) {
                    audioRecord.stop();
                    audioRecord.release();
                    audioRecord = null;
                }
                return STATE_NO_PERMISSION;

            } else {
                if (audioRecord != null) {
                    audioRecord.stop();
                    audioRecord.release();
                    audioRecord = null;
                }
                return STATE_SUCCESS;
            }
        }
    }


    /**
     * 相机权限检查(API19以下)
     *
     * @return
     */
    public static boolean hasCameraPermission(Context context) {
        if(hasPermission(context,opCameraFlag)) {
            boolean canUse = true;
            Camera mCamera = null;
            try {
                mCamera = Camera.open();
                mCamera.setParameters(mCamera.getParameters());
            } catch (Exception e) {
                canUse = false;
                if (mCamera != null) {// 返回对象非空(魅族)
                    mCamera.release();
                    mCamera = null;
                } else if (mCamera == null) {// 2, 返回对象为空
                }

            } finally {
                if (mCamera != null) {
                    canUse = true;
                    mCamera.release();
                    mCamera = null;
                } else if (mCamera == null) {// 2, 返回对象为空
                    canUse = false;
                }
            }
            return canUse;
        }else{
            return false;
        }
    }


    public static final int opCameraFlag = 26;
    public static final int opRecordAudioFlag = 27;

    /**
     * 是否有指定权限，在api19以上有效
     *
     * @param mContext 上下文
     * @param opFlag   指定操作码，see AppOpsManager define
     * @return
     */
    @TargetApi(Build.VERSION_CODES.KITKAT)
    public static boolean hasPermission(Context mContext, int opFlag) {
        boolean hasPermission = false;

        if (Build.VERSION.SDK_INT >= 19) {
            if (checkOp(mContext, opFlag) == 0) {
                hasPermission = true;

            } else if (checkOp(mContext, opFlag) == 1) {
                hasPermission = false;

            } else {
                hasPermission = false;
            }

            return hasPermission;

        } else {
            LogUtils.e(TAG, "API below 19 cannot invoke!");
        }
        return true;
    }


    /**
     * Api19以上有效
     * 返回 0 代表有权限，1代表没有权限，-1函数出错啦
     *
     * @param context
     * @param op
     * @return
     */
    @TargetApi(Build.VERSION_CODES.KITKAT)
    @SuppressWarnings({"unchecked", "rawtypes"})
    private static int checkOp(Context context, int op) {
        if (Build.VERSION.SDK_INT >= 19) {
            try {
                AppOpsManager manager = (AppOpsManager) context.getSystemService(Context.APP_OPS_SERVICE);
                Class clazz = AppOpsManager.class;
                Class[] cArg = new Class[3];
                cArg[0] = int.class;
                cArg[1] = int.class;
                cArg[2] = String.class;
                Method checkOpMethod = clazz.getDeclaredMethod("checkOp", cArg);
                return (Integer) checkOpMethod.invoke(manager, op, Binder.getCallingUid(), context.getPackageName());
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            }
        }
        return -1;
    }
}


