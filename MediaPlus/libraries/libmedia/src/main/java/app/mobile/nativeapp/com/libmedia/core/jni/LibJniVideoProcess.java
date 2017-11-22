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
 * 色彩空间处理
 * Created by android on 11/16/17.
 */

public class LibJniVideoProcess {

    /**
     * NV21转换I420
     *
     * @param in_width  输入宽度
     * @param in_height 输入高度
     * @param srcData   源数据
     * @param dstData   目标数据
     * @return
     */
    public static native int NV21TOI420(int in_width, int in_height,
                                        byte[] srcData,
                                        byte[] dstData);

    /**
     * 镜像I420
     *
     * @param in_width  输入宽度
     * @param in_height 输入高度
     * @param srcData   源数据
     * @param dstData   目标数据
     * @return
     */
    public static native int MirrorI420(int in_width, int in_height,
                                        byte[] srcData,
                                        byte[] dstData);

    /**
     * 指定角度旋转I420
     *
     * @param in_width  输入宽度
     * @param in_height 输入高度
     * @param srcData   源数据
     * @param dstData   目标数据
     */
    public static native int RotateI420(int in_width, int in_height,
                                        byte[] srcData,
                                        byte[] dstData, int rotationValue);

    /**
     * opencv 双边滤波
     * @param in_width
     * @param in_height
     * @param srcData
     * @param dstData
     * @return
     */
    public static native int Beauty(int in_width, int in_height,
                                    byte[] srcData,
                                    byte[] dstData);


}
