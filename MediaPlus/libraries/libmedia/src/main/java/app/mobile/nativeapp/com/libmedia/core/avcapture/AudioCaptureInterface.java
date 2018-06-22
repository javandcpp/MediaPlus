//package app.mobile.nativeapp.com.libmedia.core.avcapture;
//
//public interface AudioCaptureInterface {
//
//    enum OpenAudioDeviceReturn {
//        OPEN_DEVICE_SUCCESS,
//        OPEN_ERROR_OCCUPIED,
//        OPEN_ERROR_PARAM,
//        OPEN_ERROR_BUFFER
//    }
//
//    enum GetAudioDataReturn {
//        RET_SUCCESS,
//        RET_CAPTURE_NO_START,
//        RET_ERROR_PARAM,
//        RET_NO_AUDIO_DATA
//    }
//
//    public OpenAudioDeviceReturn OpenAudioDevice(int aiSamplesPerSec, int aiChannels, int aiBitsPerSample, int[] aiBufferLen);
//
//    public boolean IsOpenAudioDevice();
//
//    public void CloseAudioDevice();
//
//
//    public void StopAudioCapture();
//
//    public GetAudioDataReturn GetAudioData(byte[] abyBuffer, int aiBufferLength, int[] aiDataLength);
//
//    public boolean IsStartAudioCapture();
//
//    public void StartAudioCapture();
//}
