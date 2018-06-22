//package app.mobile.nativeapp.com.libmedia.core.avcapture.impl;
//
//import android.media.AudioFormat;
//import android.media.AudioRecord;
//import android.media.MediaRecorder;
//import android.media.audiofx.AcousticEchoCanceler;
//import android.media.audiofx.NoiseSuppressor;
//import android.os.Build;
//
//import java.util.LinkedList;
//import java.util.Queue;
//
//import app.mobile.nativeapp.com.libmedia.core.avcapture.AudioCaptureInterface;
//
//public class AudioCapture implements AudioCaptureInterface {
//
//    class AudioData {
//        byte[] baAudioData;
//        int iLength;
//    }
//
//    ;
//
//    private static final int BUFFER_LENGTH_MONO = 4096;
//    private static final int BUFFER_LENGTH_STEREO = 8192;
//
//    private int m_iSessionID = -1;
//    private int m_iBufferSize = 0;
//    private AudioData m_data = null;
//    private byte[] m_baAudioDataBuffer = null;
//    private AudioRecord m_oAudioRecord = null;
//    private boolean m_bIsOpenAudioDevice = false;
//    private boolean m_bIsStartAudioCapture = false;
//    private Object m_syncLockObject = new Object();
//    private AudioRecordThread m_oAudioThread = null;
//    private NoiseSuppressor m_oNoiseSuppressor = null;
//    private AcousticEchoCanceler mAcousticEchoCanceler = null;
//
//    private int m_iAudioSource = MediaRecorder.AudioSource.MIC;
//    private Queue<AudioData> m_qAudioData = new LinkedList<AudioData>();
//
//    @Override
//    public OpenAudioDeviceReturn OpenAudioDevice(int aiSamplesPerSec,
//                                                 int aiChannels, int aiBitsPerSample, int[] aiBufferLen) {
//        // TODO Auto-generated method stub
//        int iEncodingPCM, iChannelInfo;
//        if (aiBitsPerSample == 16) {
//            iEncodingPCM = AudioFormat.ENCODING_PCM_16BIT;
//        } else if (aiBitsPerSample == 8) {
//            iEncodingPCM = AudioFormat.ENCODING_PCM_8BIT;
//        } else {
//            return OpenAudioDeviceReturn.OPEN_ERROR_PARAM;
//        }
//
//        m_qAudioData.clear();
//        if (aiChannels == 1) {
//            iChannelInfo = AudioFormat.CHANNEL_IN_MONO;
//            aiBufferLen[0] = BUFFER_LENGTH_MONO;
//        } else if (aiChannels == 2) {
//            iChannelInfo = AudioFormat.CHANNEL_IN_STEREO;
//            aiBufferLen[0] = BUFFER_LENGTH_STEREO;
//        } else {
//            return OpenAudioDeviceReturn.OPEN_ERROR_PARAM;
//        }
//
//        if (m_bIsOpenAudioDevice) {
//            return OpenAudioDeviceReturn.OPEN_ERROR_OCCUPIED;
//        }
//
//        m_iBufferSize = AudioRecord.getMinBufferSize(aiSamplesPerSec,
//                iChannelInfo, iEncodingPCM);
//        if (aiBufferLen[0] < m_iBufferSize) {
//            aiBufferLen[0] = m_iBufferSize;
//        } else {
//            m_iBufferSize = aiBufferLen[0];
//        }
//        m_baAudioDataBuffer = new byte[m_iBufferSize];
//        if (m_syncLockObject == null || m_baAudioDataBuffer == null
//                || m_qAudioData == null) {
//            return OpenAudioDeviceReturn.OPEN_ERROR_BUFFER;
//        }
//        m_oAudioRecord = new AudioRecord(m_iAudioSource, aiSamplesPerSec,
//                iChannelInfo, iEncodingPCM, m_iBufferSize);
//        if (m_oAudioRecord == null) {
//            m_baAudioDataBuffer = null;
//            return OpenAudioDeviceReturn.OPEN_ERROR_PARAM;
//        }
//
//        if (Build.VERSION.SDK_INT >= 16) {
//            m_iSessionID = m_oAudioRecord.getAudioSessionId();
//            m_oNoiseSuppressor = NoiseSuppressor.create(m_iSessionID);
//            if (m_oNoiseSuppressor != null) {
//                m_oNoiseSuppressor.setEnabled(true);
//            }
//            mAcousticEchoCanceler = AcousticEchoCanceler.create(m_iSessionID);
//            if (mAcousticEchoCanceler != null) {
//                mAcousticEchoCanceler.setEnabled(true);
//            }
//        }
//        m_bIsOpenAudioDevice = true;
//        return OpenAudioDeviceReturn.OPEN_DEVICE_SUCCESS;
//    }
//
//    @Override
//    public boolean IsOpenAudioDevice() {
//        // TODO Auto-generated method stub
//        return m_bIsOpenAudioDevice;
//    }
//
//    @Override
//    public void CloseAudioDevice() {
//        // TODO Auto-generated method stub
//        if (m_bIsOpenAudioDevice) {
//            StopAudioCapture();
//            if (Build.VERSION.SDK_INT >= 16 && m_oNoiseSuppressor != null) {
//                m_oNoiseSuppressor.setEnabled(false);
//                m_oNoiseSuppressor.release();
//            }
//
//            if (Build.VERSION.SDK_INT >= 16 && mAcousticEchoCanceler != null) {
//                mAcousticEchoCanceler.setEnabled(false);
//                mAcousticEchoCanceler.release();
//            }
//
//            m_oAudioRecord.release();
//            m_baAudioDataBuffer = null;
//            m_oAudioRecord = null;
//            m_bIsOpenAudioDevice = false;
//        }
//    }
//
//    @Override
//    public void StartAudioCapture() {
//        // TODO Auto-generated method stub
//        if (m_bIsOpenAudioDevice && !m_bIsStartAudioCapture) {
//            m_bIsStartAudioCapture = true;
//            m_oAudioThread = new AudioRecordThread();
//            m_oAudioRecord.startRecording();
//            m_oAudioThread.start();
//        }
//    }
//
//    @Override
//    public boolean IsStartAudioCapture() {
//        // TODO Auto-generated method stub
//        return m_bIsStartAudioCapture;
//    }
//
//    @Override
//    public void StopAudioCapture() {
//        // TODO Auto-generated method stub
//        if (m_bIsStartAudioCapture) {
//            m_oAudioThread.exit();
//            m_oAudioRecord.stop();
//        }
//    }
//
//    @Override
//    public GetAudioDataReturn GetAudioData(byte[] abyBuffer,
//                                           int aiBufferLength, int[] aiDataLength) {
//        // TODO Auto-generated method stub
//        if (!m_bIsStartAudioCapture) {
//            return GetAudioDataReturn.RET_CAPTURE_NO_START;
//        } else if (abyBuffer == null || aiBufferLength < m_iBufferSize) {
//            return GetAudioDataReturn.RET_ERROR_PARAM;
//        }
//
//        synchronized (m_syncLockObject) {
//            m_data = m_qAudioData.poll();
//            if (m_data == null) {
//                return GetAudioDataReturn.RET_NO_AUDIO_DATA;
//            }
//            aiDataLength[0] = m_data.iLength;
//            System.arraycopy(m_data.baAudioData, 0, abyBuffer, 0,
//                    m_data.iLength);
//        }
//        return GetAudioDataReturn.RET_SUCCESS;
//    }
//
//
//    class AudioRecordThread extends Thread {
//
//
//        @Override
//        public void run() {
//            while (m_bIsStartAudioCapture) {
//                int iReadSize = m_oAudioRecord.read(m_baAudioDataBuffer, 0,
//                        m_iBufferSize);
//                if (iReadSize <= 0) {
//                    //��������Ϊ�����Ļ�����ʾ���ɹ���
//                    continue;
//                }
//                synchronized (m_syncLockObject) {
//                    if (m_qAudioData.size() == 2) {
//                        m_qAudioData.poll();
//                    }
//                    m_data = new AudioData();
//                    m_data.baAudioData = m_baAudioDataBuffer;
//                    m_data.iLength = iReadSize;
//                    m_qAudioData.offer(m_data);
//                }
//            }
//        }
//
//        public void exit() {
//            m_bIsStartAudioCapture = false;
//        }
//    }
//}
