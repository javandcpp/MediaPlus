/*
 * MIT License
 *
 * Copyright (c) 2017 Huawque
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @author:weichang
 * https://github.com/javandoc/MediaPlus
 */

package app.mobile.nativeapp.com.libmedia.core.jni;

/**
 * 媒体处理(直播推流)
 * Created by android on 11/12/17.
 */

public class LiveJniMediaManager {

    public enum CameraID {
        FRONT, BACK
    }

    /**
     * 初始化音频编码器
     */
    public static native int InitAudioEncoder();

    /**
     * 初始化视频编码器
     */
    public static native int InitVideoEncoder();

    /**
     * 初始化音频采集
     *
     * @param channles
     * @param SampleRate
     * @param SampleBitRate
     * @return
     */
    public static native int InitAudioCapture(int channles, int SampleRate, int SampleBitRate);

    /**
     * 初始化视频采集
     *
     * @param inWidth
     * @param inHeight
     * @param outWidth
     * @param outHeight
     * @param fps
     * @param mirror
     * @return
     */
    public static native int InitVideoCapture(int inWidth, int inHeight, int outWidth, int outHeight, int fps, boolean mirror);

    /**
     * 开启推流
     *
     * @param url
     * @return
     */
    public static native int StartPush(String url);

    /**
     * 关闭推流
     *
     * @return
     */
    public static native int Close();

    /**
     * 底层资源回收与释放
     *
     * @return
     */
    public static native int Release();

    /**
     * 视频滤镜
     *
     * @param filterType
     * @param filterValue
     * @return
     */
    public static native int Filter(FilterType filterType, int filterValue);

    /**
     * 上传音频数据编码AAC
     *
     * @param audioBuffer
     * @param length
     * @return
     */
    public static native int EncodeAAC(byte[] audioBuffer, int length);

    /**
     * 上传视频数据编码H264
     *
     * @param videoBuffer
     * @param length
     * @return
     */
    public static native int EncodeH264(byte[] videoBuffer, int length);


    /**
     * 设置摄像头
     *
     * @param cameraID
     */
    public static native void SetCameraID(int cameraID);
}
