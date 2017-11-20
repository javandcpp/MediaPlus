# MediaPlus  

<div align=left>
<img width="100" height="100" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/github_qr.png"/>
</div>

#### android 音视频采集与直播SDK：
* [v1.0 采集音视频并处理数据转换与编码，Rtmp推流](https://github.com/javandoc/MediaPlus/releases/tag/v1.0)

* [文档链接](http://www.jianshu.com/u/f5072293c4b3)


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



#### Use example:

* android camera capture NV21 by the bottom of the conversion I420 (yuv420p), processing rotation, the front camera has been processed image; audio capture to PCM 16 bit sampling rate of 48000, no other processing; audio and video recording the initial time stamp of each frame, encoding The current timestamp calculates the difference as the AVFrame PTS, AVPacket data that the audio and video coding completed is sent in two threads.
* The current version only supports RTMP protocol streaming, and subsequent  extensions will extend other protocols.
* This version is only for android mobile audio and video capture and streaming.
* Has not yet added a video filter, behind the use of opencv filter, look forward to!
* At present the camera preview also needs opengl to draw, to be perfect.
* Without RTMP server, you need to set up local RTMP service,If there is no RtmpServer, you can also write a local test, just push the stream address to “/mnt/sdcard/test.flv”.
* The following is a JNI API call.

```

* Initialize audio and video capture
LiveJniMediaManager.InitAudioCapture(params...);
LiveJniMediaManager.InitVideoCapture(params...);
	
* Initialize audio and video encoder
LiveJniMediaManager.InitAudioEncoder();
LiveJniMediaManager.InitVideoEncoder();
    
* Start Rtmp stream
LiveJniMediaManager.StartPush(pushUrl);
        
* Camera switch
LiveJniMediaManager.SetCameraID(int cameraID);
        
* Pass the video data and audio data to the H264 encoder or AAC encoder
LiveJniMediaManager.EncodeH264(videoBuffer, length);
LiveJniMediaManager.EncodeAAC(audioBuffer, length);
 
* Stop Rtmp stream and native release
LiveJniMediaManager.Close();
LiveJniMediaManager.Release();
    
   
```


#### -- [download APK](https://github.com/javandoc/MediaPlus/blob/master/Resource/app-debug1.0.apk) --

#### -- [版本记录](https://github.com/javandoc/MediaPlus/releases/) --

#### -- [API文档](https://javandoc.github.io/javadoc/) --

#### C++ class diagram：

<div align=center>
<table>
<tr>
<img width="790" height="768" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/MediaUML.png">
</tr>
</table>
</div>
<div align=left>
The above UML for the current class diagram, the current function is not too full, hope that through this project more and more rich, and permanently open source, detailed use please see the source code.
</div>
