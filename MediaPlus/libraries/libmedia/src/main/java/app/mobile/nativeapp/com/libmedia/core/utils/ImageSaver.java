package app.mobile.nativeapp.com.libmedia.core.utils;

import android.annotation.TargetApi;
import android.media.Image;
import android.os.Build;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by android on 4/16/18.
 */
public class ImageSaver implements Runnable {

    private final static String TAG = "ImageSaver";

    private final Image image;
    private final File file;
    private ImageSaverCallback imageSaverCallback;

    public interface ImageSaverCallback {
        void onSuccessFinish(byte[] bytes);

        void onError();
    }

    public ImageSaver(Image image, File file, ImageSaverCallback imageSaverCallback) {
        this.image = image;
        this.file = file;
        this.imageSaverCallback = imageSaverCallback;
    }

    @TargetApi(Build.VERSION_CODES.KITKAT)
    @Override
    public void run() {
        FileOutputStream output = null;
        try {
            ByteBuffer buffer = image.getPlanes()[0].getBuffer();
            byte[] bytes = new byte[buffer.remaining()];
            buffer.get(bytes);
            output = new FileOutputStream(file);
            output.write(bytes);
            imageSaverCallback.onSuccessFinish(bytes);
        } catch (IOException ignore) {
            imageSaverCallback.onError();
        } catch (IllegalStateException e) {
            imageSaverCallback.onError();
        } finally {
            image.close();
            if (null != output) {
                try {
                    output.close();
                } catch (IOException e) {
                }
            }
        }
    }

}