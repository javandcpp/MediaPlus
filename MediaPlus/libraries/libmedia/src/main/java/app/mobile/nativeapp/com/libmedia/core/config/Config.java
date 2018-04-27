package app.mobile.nativeapp.com.libmedia.core.config;

import android.content.Context;

import app.mobile.nativeapp.com.libmedia.core.utils.FileUtils;


/**
 * Created by android on 3/29/18.
 */

public class Config {

    public static final String TAG = "libMedia";
    public static final boolean DEBUG = true;
    public static final int MAX_PHOTO_SIZE = 9;
    public static final String VIDEO_COMPRESSOR_APPLICATION_DIR_NAME = "VideoCompressor";
    public static final String VIDEO_COMPRESSOR_COMPRESSED_VIDEOS_DIR = "/CompressedVideos/";
    public static final String VIDEO_COMPRESSOR_TEMP_DIR = "/Temp/";
    public static final int MIN_VIDEO_DURATION = 1000;
    public static final int MAX_VIDEO_SECONDS = 180;
    public static final int MAX_AUDIO_SECONDS = 180;
    public static final int MIN_AUDIO_WIDTH = 78;

    public static final int RESOLUTION_1080_1920 = 1080 * 1920;
    public static final int RESOLUTION_720_1280 = 720 * 1280;
    public static final int RESOLUTION_480_640 = 480 * 640;


    /**
     * 视频压缩
     *
     * @return MB
     */
    public static boolean getMinCompressValue(long size) {
        return (size / 1000 / 1000) <= 5;
    }

    /**
     * 获取图片视频保存目录
     *
     * @return
     */
    public static String getUploadSavePath(Context context) {

        return FileUtils.getUploadCachePath(context);
    }
}
