package app.mobile.nativeapp.com.libmedia.core.utils.timer;


import java.util.concurrent.TimeUnit;

import app.mobile.nativeapp.com.libmedia.core.utils.DateUtils;

public class CountdownTask extends TimerTaskBase implements Runnable {

    private int maxDurationMilliseconds = 0;

    public CountdownTask(Callback callback, int maxDurationMilliseconds) {
        super(callback);
        this.maxDurationMilliseconds = maxDurationMilliseconds;
    }

    @Override
    public void run() {

        recordingTimeSeconds--;

        int millis = (int) recordingTimeSeconds * 1000;

        if (callback != null) {
            callback.setText(
                    String.format("%02d:%02d",
                            TimeUnit.MILLISECONDS.toMinutes(millis),
                            TimeUnit.MILLISECONDS.toSeconds(millis) -
                                    TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(millis))
                    ));
        }

        if (alive && recordingTimeSeconds > 0) {
            handler.postDelayed(this, DateUtils.SECOND);
        } else {
            stop();
            handler.removeCallbacks(this);
        }

    }

    @Override
    public boolean stop() {
        if (callback != null) {
            callback.setTextVisible(false);
        }
        alive = false;
        return alive;
    }

    @Override
    public void start() {
        alive = true;
        recordingTimeSeconds = maxDurationMilliseconds / 1000;
        if (callback != null) {
            callback.setText(
                    String.format("%02d:%02d",
                            TimeUnit.MILLISECONDS.toMinutes(maxDurationMilliseconds),
                            TimeUnit.MILLISECONDS.toSeconds(maxDurationMilliseconds) -
                                    TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(maxDurationMilliseconds))
                    ));
            callback.setTextVisible(true);
        }
        handler.postDelayed(this, DateUtils.SECOND);
    }

    @Override
    public long getRecordTotalSeconds() {
        return 0;
    }
}
