package app.mobile.nativeapp.com.libmedia.core.utils;

import android.annotation.SuppressLint;
import android.annotation.TargetApi;
import android.content.Context;
import android.content.pm.FeatureInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.os.Build;
import android.os.Environment;
import android.text.TextUtils;
import android.util.TypedValue;
import android.view.Display;
import android.view.Surface;
import android.view.WindowManager;
import android.webkit.MimeTypeMap;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.Locale;

/**
 * 信息获取
 */
public class DeviceUtils {

    private static final String TAG = "DeviceUtils";


    /**
     * >=2.3
     */
    public static boolean hasGingerbread() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.GINGERBREAD;
    }

    /**
     * >=3.0 LEVEL:11
     */
    public static boolean hasHoneycomb() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB;
    }

    /**
     * >=3.1
     */
    public static boolean hasHoneycombMR1() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB_MR1;
    }

    /**
     * >=4.0 14
     */
    public static boolean hasICS() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.ICE_CREAM_SANDWICH;
    }

    /**
     * >= 4.1 16
     *
     * @return
     */
    public static boolean hasJellyBean() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN;
    }

    /**
     * >= 4.2 17
     */
    public static boolean hasJellyBeanMr1() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1;
    }

    /**
     * >= 4.3 18
     */
    public static boolean hasJellyBeanMr2() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2;
    }

    /**
     * >=4.4 19
     */
    public static boolean hasKitkat() {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT;
    }

    public static int getSDKVersionInt() {
        return Build.VERSION.SDK_INT;
    }

    @SuppressWarnings("deprecation")
    public static String getSDKVersion() {
        return Build.VERSION.SDK;
    }


    /**
     * 是否有sd卡
     *
     * @return
     */
    public static boolean hasSdCard() {
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {
            return true;
        }
        return false;
    }


    /**
     * 判断是否是平板电脑
     *
     * @param context
     * @return
     */
    public static boolean isTablet(Context context) {
        return (context.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) >= Configuration.SCREENLAYOUT_SIZE_LARGE;
    }

    /**
     * 检测是否是平板电脑
     *
     * @param context
     * @return
     */
    public static boolean isHoneycombTablet(Context context) {
        return hasHoneycomb() && isTablet(context);
    }

    public static int dipToPX(final Context ctx, float dip) {
        return (int) TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dip, ctx.getResources().getDisplayMetrics());
    }

    /**
     * 获取CPU的信息
     *
     * @return
     */
    public static String getCpuInfo() {
        String cpuInfo = "";
        try {
            if (new File("/proc/cpuinfo").exists()) {
                FileReader fr = new FileReader("/proc/cpuinfo");
                BufferedReader localBufferedReader = new BufferedReader(fr, 8192);
                cpuInfo = localBufferedReader.readLine();
                localBufferedReader.close();

                if (cpuInfo != null) {
                    cpuInfo = cpuInfo.split(":")[1].trim().split(" ")[0];
                }
            }
        } catch (Exception e) {
        }
        return cpuInfo;
    }

    /**
     * 是否支持闪光
     * @param context
     * @return
     */
    public static boolean isSupportFlashCamera(Context context) {
        FeatureInfo[] features = context.getPackageManager().getSystemAvailableFeatures();
        for(FeatureInfo info : features) {
            if(PackageManager.FEATURE_CAMERA_FLASH.equals(info.name))
                return true;
        }
        return false;
    }

    /**
     * 判断是否支持闪光灯
     */
    public static boolean isSupportCameraLedFlash(PackageManager pm) {
        if (pm != null) {
            FeatureInfo[] features = pm.getSystemAvailableFeatures();
            if (features != null) {
                for (FeatureInfo f : features) {
                    if (f != null && PackageManager.FEATURE_CAMERA_FLASH.equals(f.name)) //判断设备是否支持闪光灯
                        return true;
                }
            }
        }
        return false;
    }

    /**
     * 检测设备是否支持相机
     */
    public static boolean isSupportCameraHardware(Context context) {
        if (context != null && context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_CAMERA)) {
            // this device has a camera
            return true;
        } else {
            // no camera on this device
            return false;
        }
    }

    /**
     * 获取屏幕宽度
     */
    @SuppressWarnings("deprecation")
    public static int getScreenWidth(Context context) {
        Display display = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        return display.getWidth();
    }

    @SuppressWarnings("deprecation")
    public static int getScreenHeight(Context context) {
        Display display = ((WindowManager) context.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        return display.getHeight();
    }

    /**
     * 是否支持前置摄像头
     */
    @SuppressLint("NewApi")
    @TargetApi(Build.VERSION_CODES.GINGERBREAD)
    public static boolean isSupportFrontCamera() {
        if (!DeviceUtils.hasGingerbread()) {
            return false;
        }
        int numberOfCameras = android.hardware.Camera.getNumberOfCameras();
        if (2 == numberOfCameras) {
            return true;
        }
        return false;
    }


    /**
     * 获取当前手机系统语言。
     *
     * @return 返回当前系统语言。例如：当前设置的是“中文-中国”，则返回“zh-CN”
     */
    public static String getSystemLanguage() {
        return Locale.getDefault().getLanguage();
    }

    /**
     * 获取当前系统上的语言列表(Locale列表)
     *
     * @return 语言列表
     */
    public static Locale[] getSystemLanguageList() {
        return Locale.getAvailableLocales();
    }

    /**
     * 获取当前手机系统版本号
     *
     * @return 系统版本号
     */
    public static String getSystemVersion() {
        return Build.VERSION.RELEASE;
    }

    /**
     * 获取手机型号
     *
     * @return 手机型号
     */
    public static String getSystemModel() {
        return Build.MODEL;
    }

    /**
     * 获取手机厂商
     *
     * @return 手机厂商
     */
    public static String getDeviceBrand() {
        return Build.BRAND;
    }




    /**
     * 检测当前设备是否是特定的设备
     *
     * @param devices
     * @return
     */
    public static boolean isDevice(String... devices) {
        String model = DeviceUtils.getDeviceModel();
        if (devices != null && model != null) {
            for (String device : devices) {
                if (model.indexOf(device) != -1) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * 获得设备型号
     *
     * @return
     */
    public static String getDeviceModel() {
        return StringUtils.trim(Build.MODEL);
    }


    public static int getDeviceDefaultOrientation(Context context) {
        WindowManager windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Configuration config = context.getResources().getConfiguration();

        int rotation = windowManager.getDefaultDisplay().getRotation();

        if (((rotation == Surface.ROTATION_0 || rotation == Surface.ROTATION_180) &&
                config.orientation == Configuration.ORIENTATION_LANDSCAPE)
                || ((rotation == Surface.ROTATION_90 || rotation == Surface.ROTATION_270) &&
                config.orientation == Configuration.ORIENTATION_PORTRAIT)) {
            return Configuration.ORIENTATION_LANDSCAPE;
        } else {
            return Configuration.ORIENTATION_PORTRAIT;
        }
    }

    public static String getMimeType(String url) {
        String type = "";
        String extension = MimeTypeMap.getFileExtensionFromUrl(url);
        if (!TextUtils.isEmpty(extension)) {
            type = MimeTypeMap.getSingleton().getMimeTypeFromExtension(extension);
        } else {
            String reCheckExtension = MimeTypeMap.getFileExtensionFromUrl(url.replaceAll("\\s+", ""));
            if (!TextUtils.isEmpty(reCheckExtension)) {
                type = MimeTypeMap.getSingleton().getMimeTypeFromExtension(reCheckExtension);
            }
        }
        return type;
    }

    public static int convertDipToPixels(Context context, int dip) {
        Resources resources = context.getResources();
        float px = TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dip, resources.getDisplayMetrics());
        return (int) px;
    }

    public static int getRealSize(Context context, float size) {
        return (int) (context.getResources().getDisplayMetrics().density * size + 0.5F);
    }


}
