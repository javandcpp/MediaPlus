package app.mobile.nativeapp.com.libmedia.core.utils.timer;


import app.mobile.nativeapp.com.libmedia.core.utils.DateUtils;

public class TimerTask extends TimerTaskBase implements Runnable {

    private long mMaxDuration;
    private int recordTotalSeconds;

    public TimerTask(TimerTaskBase.Callback callback) {
        super(callback);
    }

    public TimerTask(Callback callback, int videoDuration) {
        super(callback);
        this.mMaxDuration = videoDuration;
    }

    @Override
    public long getRecordTotalSeconds() {
        return recordTotalSeconds;
    }

    @Override
    public void run() {
        recordingTimeSeconds++;
        recordTotalSeconds++;
        if (recordingTimeSeconds == 60) {
            recordingTimeSeconds = 0;
            recordingTimeMinutes++;
        }

        if (recordTotalSeconds == mMaxDuration / 1000) {
            if (null != handler) {
                handler.removeCallbacks(this);
                handler = null;
            }
            if (null != callback) {
                callback.countDownOver();
            }
            return;
        }

        if (callback != null) {
            callback.setText(
                    String.format("%02d:%02d", recordingTimeMinutes, recordingTimeSeconds));
        }
        if (alive) {
            handler.postDelayed(this, DateUtils.SECOND);
        }
    }


    @Override
    public void start() {
        alive = true;
        recordingTimeMinutes = 0;
        recordingTimeSeconds = 0;
        recordTotalSeconds = 0;
        if (callback != null) {
            callback.setText(
                    String.format("%02d:%02d", recordingTimeMinutes, recordingTimeSeconds));
            callback.setTextVisible(true);
        }
        handler.postDelayed(this, DateUtils.SECOND);
    }

    @Override
    public boolean stop() {
        boolean flag = false;
        if (null != callback && recordTotalSeconds < 1) {
            flag = true;
            callback.recordTimeTooShort();
        }
        if (callback != null) {
            callback.setTextVisible(false);
        }
        alive = false;
        if (null != handler) {
            handler.removeCallbacks(this);
        }
        return flag;
    }
}
