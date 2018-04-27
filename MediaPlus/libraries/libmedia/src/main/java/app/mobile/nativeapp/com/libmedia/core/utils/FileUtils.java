package app.mobile.nativeapp.com.libmedia.core.utils;

import android.content.ContentResolver;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.os.StatFs;
import android.text.TextUtils;

import java.io.ByteArrayOutputStream;
import java.io.Closeable;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.channels.FileChannel;
import java.text.DecimalFormat;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Random;

import app.mobile.nativeapp.com.libmedia.core.config.Config;


public class FileUtils {
    /**
     * 随机命名
     */
    private static final String RANDOM_STRING = "abcdefghijklmnopqrstuvwxyz0123456789";
    /**
     * 时间命名
     */
    private static final String TIME_STRING = "yyyyMMdd_HHmmss";
    /**
     * 限制图片最大宽度进行压缩
     */
    private static final int MAX_WIDTH = 720;
    /**
     * 限制图片最大高度进行压缩
     */
    private static final int MAX_HEIGHT = 1280;
    /**
     * 上传最大图片限制
     */
    private static final int MAX_UPLOAD_PHOTO_SIZE = 300 * 1024;

    private FileUtils() {
    }

    /**
     * 缓存文件根目录名
     */
    private static final String FILE_DIR = "mediaplus";
    /**
     * 上传的照片文件路径
     */
    private static final String UPLOAD_FILE = "upload";

    /**
     * SD卡是否存在
     */
    public static boolean isSDCardExist() {
        return Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);
    }

    /**
     * 获取缓存目录路径
     *
     * @return
     */
    public static String getCacheDirPath(Context context) {
        if (isSDCardExist()) {
            String path = Environment.getExternalStorageDirectory() + File.separator + FILE_DIR + File.separator;
            File directory = new File(path);
            if (!directory.exists()) directory.mkdirs();
            return path;
        } else {
            File directory = new File(context.getCacheDir(), FileUtils.FILE_DIR);
            if (!directory.exists()) directory.mkdirs();
            return directory.getAbsolutePath();
        }
    }

    /**
     * 获取缓存目录
     *
     * @return
     */
    public static File getCacheDir(Context context) {
        if (isSDCardExist()) {
            String path = Environment.getExternalStorageDirectory() + File.separator + FILE_DIR + File.separator;
            File directory = new File(path);
            if (!directory.exists()) directory.mkdirs();
            return directory;
        } else {
            File directory = new File(context.getCacheDir(), FileUtils.FILE_DIR);
            if (!directory.exists()) directory.mkdirs();
            return directory;
        }
    }

    /**
     * 获取上传的路径
     *
     * @return
     */
    public static String getUploadCachePath(Context context) {
        if (isSDCardExist()) {
            String path = Environment.getExternalStorageDirectory() + File.separator + FILE_DIR + File.separator + UPLOAD_FILE + File.separator;
            File directory = new File(path);
            if (!directory.exists()) directory.mkdirs();
            return path;
        } else {
            File directory = new File(context.getCacheDir(), FileUtils.FILE_DIR + File.separator + UPLOAD_FILE);
            if (!directory.exists()) directory.mkdirs();
            return directory.getAbsolutePath();
        }
    }

    /**
     * jpg文件名
     *
     * @param context
     * @return
     */
    public static String getUploadPhotoFile(Context context) {
        return getUploadCachePath(context) + getTimeString() + ".jpg";
    }

    /**
     * mp4文件名
     *
     * @param context
     * @return
     */
    public static String getUploadVideoFile(Context context) {
        return getUploadCachePath(context) + getTimeString() + ".mp4";
    }

    /**
     * 保存拍摄图片
     *
     * @param photoPath
     * @param data
     * @param isFrontFacing 是否为前置拍摄
     * @return
     */
    public static boolean savePhoto(String photoPath, byte[] data, boolean isFrontFacing) {
        if (photoPath != null && data != null) {
            FileOutputStream fos = null;
            try {
                Bitmap preBitmap = compressBitmap(data, MAX_WIDTH, MAX_HEIGHT);
                if (isFrontFacing) {
                    Matrix matrix = new Matrix();
                    matrix.postScale(1, -1);
                    Bitmap newBitmap = Bitmap.createBitmap(preBitmap, 0, 0, preBitmap.getWidth(), preBitmap.getHeight(), matrix, true);
                    preBitmap.recycle();
                    preBitmap = newBitmap;
                }
                byte[] newDatas = compressBitmapToBytes(preBitmap, MAX_UPLOAD_PHOTO_SIZE);
                fos = new FileOutputStream(photoPath);
                fos.write(newDatas);
                LogUtils.i("compress over ");
                return true;
            } catch (Exception e) {
                e.printStackTrace();
                LogUtils.i(e);
            } finally {
                closeCloseable(fos);
            }
        }
        return false;
    }

    /**
     * 把字节流按照图片方式大小进行压缩
     *
     * @param datas
     * @param w
     * @param h
     * @return
     */
    public static Bitmap compressBitmap(byte[] datas, int w, int h) {
        if (datas != null) {
            BitmapFactory.Options opts = new BitmapFactory.Options();
            opts.inJustDecodeBounds = true;
            BitmapFactory.decodeByteArray(datas, 0, datas.length, opts);
            if (opts.outWidth != 0 && opts.outHeight != 0) {
                LogUtils.i(opts.outWidth + " " + opts.outHeight);
                int scaleX = opts.outWidth / w;
                int scaleY = opts.outHeight / h;
                int scale = 1;
                if (scaleX >= scaleY && scaleX >= 1) {
                    scale = scaleX;
                }
                if (scaleX < scaleY && scaleY >= 1) {
                    scale = scaleY;
                }
                opts.inJustDecodeBounds = false;
                opts.inSampleSize = scale;
                LogUtils.i("compressBitmap inSampleSize " + datas.length + " " + scale);
                return BitmapFactory.decodeByteArray(datas, 0, datas.length, opts);
            }
        }
        return null;
    }

    /**
     * 质量压缩图片
     *
     * @param bitmap
     * @param maxSize
     * @return
     */
    public static byte[] compressBitmapToBytes(Bitmap bitmap, int maxSize) {
        if (bitmap == null) return null;
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.JPEG, 90, baos);
        byte[] datas = baos.toByteArray();
        int options = 80;
        int longs = datas.length;
        while (longs > maxSize && options > 0) {
            LogUtils.i("compressBitmapToBytes " + longs + "  " + options);
            baos.reset();
            bitmap.compress(Bitmap.CompressFormat.JPEG, options, baos);
            datas = baos.toByteArray();
            longs = datas.length;
            options -= 20;
        }
        return datas;
    }

    /**
     * 获取文件路径下所有文件大小, 适当放到子线程中执行
     *
     * @param file
     * @return
     */
    public static long getFileSize(File file) {
        if (file == null || !file.exists()) return 0;
        long totalSize = 0;
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            for (File f : files) {
                totalSize += getFileSize(f);
            }
            return totalSize;
        } else {
            return file.length();
        }
    }

    /**
     * 删除文件夹下所有文件,适当放到子线程中执行
     *
     * @param file
     */
    public static void delteFiles(File file) {
        if (file == null || !file.exists()) return;
        if (file.isDirectory()) {
            File[] files = file.listFiles();
            for (File f : files) {
                if (!f.isDirectory()) {
                    f.delete();
                }
            }
        } else {
            file.delete();
        }
    }

    /**
     * 关闭资源
     *
     * @param close
     */
    public static void closeCloseable(Closeable close) {
        if (close != null) {
            try {
                close.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    /**
     * 文件大小
     *
     * @param fileS
     * @return
     */
    public static String formetFileSize(long fileS) {
        if (fileS <= 0) return "0B";
        if (fileS < 1024) {
            return new DecimalFormat("#.00").format((double) fileS) + "B";
        } else if (fileS < 1048576) {
            return new DecimalFormat("#.00").format((double) fileS / 1024) + "KB";
        } else if (fileS < 1073741824) {
            return new DecimalFormat("#.00").format((double) fileS / 1048576) + "MB";
        } else {
            return new DecimalFormat("#.00").format((double) fileS / 1073741824) + "GB";
        }
    }

    /**
     * sd卡容量
     *
     * @return
     */
    public static long getAvailableStorage() {
        long availableSize = 0;
        if (isSDCardExist()) {
            File sdFile = Environment.getExternalStorageDirectory();
            StatFs stat = new StatFs(sdFile.getPath());
            if (Build.VERSION.SDK_INT == Build.VERSION_CODES.JELLY_BEAN_MR2) {
                availableSize = stat.getAvailableBytes();
            } else {
                availableSize = ((long) stat.getAvailableBlocks() * stat.getBlockSize());
            }
        }
        return availableSize;
    }

    /**
     * 获取随机文件名称字符串
     *
     * @param length 生成字符串的长度
     * @return
     */
    public static String getRandomString(int length) {
        Random random = new Random();
        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < length; i++) {
            int number = random.nextInt(RANDOM_STRING.length());
            sb.append(RANDOM_STRING.charAt(number));
        }
        return sb.toString();
    }

    public static String getTimeString() {
        return new SimpleDateFormat(TIME_STRING).format(new Date());
    }

    /**
     * 将字符串写入文件中
     *
     * @param str    需要写入的字符串
     * @param file   写入文件的路径
     * @param append true 追加写入 false 覆盖写入
     * @return
     */
    public static boolean writeFile(String str, File file, boolean append) {
        if (TextUtils.isEmpty(str) || file == null) return false;
        FileWriter fw = null;
        try {
            fw = new FileWriter(file, append);
            fw.write(str);
            fw.flush();
            return true;
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            closeCloseable(fw);
        }
        return false;
    }


    public static boolean fileIsExists(String path) {
        if (path == null || path.trim().length() <= 0) {
            return false;
        }
        try {
            File f = new File(path);
            if (!f.exists()) {
                return false;
            }
        } catch (Exception e) {
            return false;
        }
        return true;
    }


    public static void createApplicationFolder() {
        File f = new File(Environment.getExternalStorageDirectory(), File.separator + Config.VIDEO_COMPRESSOR_APPLICATION_DIR_NAME);
        f.mkdirs();
        f = new File(Environment.getExternalStorageDirectory(), File.separator + Config.VIDEO_COMPRESSOR_APPLICATION_DIR_NAME + Config.VIDEO_COMPRESSOR_COMPRESSED_VIDEOS_DIR);
        f.mkdirs();
        f = new File(Environment.getExternalStorageDirectory(), File.separator + Config.VIDEO_COMPRESSOR_APPLICATION_DIR_NAME + Config.VIDEO_COMPRESSOR_TEMP_DIR);
        f.mkdirs();
    }

    public static File saveTempFile(String fileName, Context context, Uri uri) {

        File mFile = null;
        ContentResolver resolver = context.getContentResolver();
        InputStream in = null;
        FileOutputStream out = null;

        try {
            in = resolver.openInputStream(uri);

            mFile = new File(Environment.getExternalStorageDirectory().getPath() + File.separator + Config.VIDEO_COMPRESSOR_APPLICATION_DIR_NAME + Config.VIDEO_COMPRESSOR_TEMP_DIR, fileName);
            out = new FileOutputStream(mFile, false);
            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1) {
                out.write(buffer, 0, read);
            }
            out.flush();
        } catch (IOException e) {
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                }
            }
            if (out != null) {
                try {
                    out.close();
                } catch (IOException e) {
                }
            }
        }
        return mFile;
    }


    public static Bitmap createVideoThumbnail(String filePath) {
        Class<?> clazz = null;
        Object instance = null;
        try {
            clazz = Class.forName("android.media.MediaMetadataRetriever");
            instance = clazz.newInstance();

            Method method = clazz.getMethod("setDataSource", String.class);
            method.invoke(instance, filePath);

            // The method name changes between API Level 9 and 10.
            if (Build.VERSION.SDK_INT <= 9) {
                return (Bitmap) clazz.getMethod("captureFrame").invoke(instance);
            } else {
                byte[] data = (byte[]) clazz.getMethod("getEmbeddedPicture").invoke(instance);
                if (data != null) {
                    Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length);
                    if (bitmap != null) return bitmap;
                }
                return (Bitmap) clazz.getMethod("getFrameAtTime").invoke(instance);
            }
        } catch (IllegalArgumentException ex) {
            // Assume this is a corrupt video file
        } catch (RuntimeException ex) {
            // Assume this is a corrupt video file.
        } catch (InstantiationException e) {
        } catch (InvocationTargetException e) {
        } catch (ClassNotFoundException e) {
        } catch (NoSuchMethodException e) {
        } catch (IllegalAccessException e) {
        } finally {
            try {
                if (instance != null) {
                    clazz.getMethod("release").invoke(instance);
                }
            } catch (Exception ignored) {
            }
        }
        return null;
    }

    public static void saveBitmap(Bitmap bitmap, String path) throws IOException {
        File file = new File(path);
        if (file.exists()) {
            file.delete();
        }
        FileOutputStream out;
        try {
            out = new FileOutputStream(file);
            if (bitmap.compress(Bitmap.CompressFormat.PNG, 90, out)) {
                out.flush();
                out.close();
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * filechannel copy
     *
     * @param fromFile
     * @param toFile
     */
    public static boolean fileCopyWithFileChannel(File fromFile, File toFile) {
        FileInputStream fileInputStream = null;
        FileOutputStream fileOutputStream = null;
        FileChannel fileChannelInput = null;
        FileChannel fileChannelOutput = null;
        try {
            fileInputStream = new FileInputStream(fromFile);
            fileOutputStream = new FileOutputStream(toFile);
            //得到fileInputStream的文件通道
            fileChannelInput = fileInputStream.getChannel();
            //得到fileOutputStream的文件通道
            fileChannelOutput = fileOutputStream.getChannel();
            //将fileChannelInput通道的数据，写入到fileChannelOutput通道
            fileChannelInput.transferTo(0, fileChannelInput.size(), fileChannelOutput);
            return true;
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            try {
                if (fileInputStream != null) {
                    fileInputStream.close();
                }
                if (fileChannelInput != null) {
                    fileChannelInput.close();
                }
                if (fileOutputStream != null) {
                    fileOutputStream.close();
                }
                if (fileChannelOutput != null) {
                    fileChannelOutput.close();
                }

            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return false;
    }
}
