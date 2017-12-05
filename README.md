# MediaPlus  

<div align=left>
<img width="100" height="100" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/MediaPlus_2.png"/>
</div>

#### android 音视频采集与推流：
* [releases](https://github.com/javandoc/MediaPlus/releases/)

* [文档链接](https://juejin.im/user/5809cf500bd1d00057ddc475)

<div align=center>
<img src="https://github.com/javandoc/MediaPlus/blob/master/Resource/MediaPlus_xmind.png"/>
</div>


* Rtmp推流截图
<div align=center>
<table border=0 cellspacing="10" style="border-collapse:separate; border-spacing:0px 50px;">
<td>
<img width="220" height="380" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/screen_one.png"/>
</td>
<td>
<img width="220" height="380" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/screen_live.gif"/>
</td>
</table>

</div>

* 水印截图
<div align=center>
<table border=0 cellspacing="10" style="border-collapse:separate; border-spacing:0px 50px;">
<td>
<img width="220" height="380" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/watermark.gif"/>
</td>
</table>
</div>



* Android相机采集NV21格式数据，并使用libyuv转换I420（yuv420p）、处理I420旋转、前置摄像头镜像; 音频采集PCM数据：2通道、16位、采样率48000KHz,音视频软编。
* V1.0版本只支持RTMP协议，后续扩展会扩展其他协议。
* 仅适用于Android移动音频和视频的采集和推流。
* V1.0没有添加任何视频滤镜等其他特效，后续版本会新增视频滤镜等功能。
* 需要搭建RTMP服务(如:nginx+rtmp,crtmpserver等);也可以写入本地文件，只需更改推流地址，如：“/mnt/sdcard/test.flv”。
* 以下为相关API调用：

#### JNI API说明：
```


* 初始化音视频采集
LiveJniMediaManager.InitAudioCapture(int channles, int SampleRate, int SampleBitRate);
LiveJniMediaManager.InitVideoCapture(int inWidth, int inHeight, int outWidth, int outHeight, int fps, boolean mirror);
	
* 初始化音视频编码器
LiveJniMediaManager.InitAudioEncoder();
LiveJniMediaManager.InitVideoEncoder();
    
* 开始推流
LiveJniMediaManager.StartPush(pushUrl);
        
* 发送音视频数据至底层
LiveJniMediaManager.EncodeH264(videoBuffer, length);
LiveJniMediaManager.EncodeAAC(audioBuffer, length);
 
* 停止推流与资源回收
LiveJniMediaManager.Close();
LiveJniMediaManager.Release();

   
```

#### Use example:
```
 mRtmpPushStreamer = new RtmpPushStreamer.Builder()
                     .withActivity(LiveActivity.this)
                     .withSurfaceView(surfaceView)
                     .withWaterMark(true, ivWaterMark, 90, 30)  //参数:true 水印开关 水印图片 宽(90px)\高(30px)
                     .withPushStreamCall(new PushStreamCall() {
                            @Override
                            public void PushSucess() {                     
                               #-------"推流成功"--------#
                            }

                            @Override
                            public void PushFailed() {
                               #-------"推流失败"--------#
                            }
                        }).build();
```

#### -- [download APK](https://github-production-release-asset-2e65be.s3.amazonaws.com/107510291/ac471784-cdeb-11e7-9551-a8294928925d?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20171120%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20171120T043140Z&X-Amz-Expires=300&X-Amz-Signature=8e6a595afae260f794f7d44fcbf6bfc4a5c254b90a5fde24cdf46290c5ca242d&X-Amz-SignedHeaders=host&actor_id=9412054&response-content-disposition=attachment%3B%20filename%3Dapp-debug1.0.apk&response-content-type=application%2Fvnd.android.package-archive) --

#### -- [API文档](https://javandoc.github.io/javadoc/) --
