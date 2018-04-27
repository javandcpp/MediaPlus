package app.mobile.nativeapp.com.libmedia.core.inter;

import java.io.File;

/**
 * Created by android on 3/21/18.
 */

public interface CameraVideoCallBack {

    /**
     * 录制时间回调
     *
     * @param millseconds
     */
    void recordTime(long millseconds);


    void recordFinish(File file);
}
