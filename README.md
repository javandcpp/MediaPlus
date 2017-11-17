# MediaPlus


- libMedia Multimedia components: acquisition, coding, filters, rtmp push flow.


<div align=center>

<table>
<tr>
<img width="220" height="380" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/screen_one.png"/>
</tr>

</table>

</div>



#### Use example:

* android camera capture NV21 by the bottom of the conversion I420 (yuv420p), processing rotation, the front camera has been processed image; audio capture to PCM 16 bit sampling rate of 48000, no other processing; audio and video recording the initial time stamp of each frame, encoding The current timestamp calculates the difference as the AVFrame PTS, AVPacket data that the audio and video coding completed is sent in two threads.
* The current version only supports RTMP protocol streaming, and subsequent  extensions will extend other protocols.
* This version is only for android mobile audio and video capture and streaming.
* Has not yet added a video filter, behind the use of opencv filter, look forward to!
* At present the camera preview also needs opengl to draw, to be perfect.


```

* Initialize audio and video capture
LiveJniMediaManager.InitAudioCapture();
LiveJniMediaManager.InitVideoCapture();
	
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


#### UML:

<div align=center>
<table>
<tr>
<img width="800" height="880" src="https://github.com/javandoc/MediaPlus/blob/master/Resource/MediaUML.png">
</tr>
</table>
</div>
<div align=left>
The above UML for the current class diagram, the current function is not too full, hope that through this project more and more rich, and permanently open source, detailed use please see the source code.
RTMP server is not yet available, you need to build a local service.
</div>
