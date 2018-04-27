package app.mobile.nativeapp.com.libmedia.core.utils.timer;

import android.os.Handler;
import android.os.Looper;

public abstract class TimerTaskBase {

    public interface Callback {
        void setText(String text);
        void setTextVisible(boolean visible);
        void countDownOver();
        void recordTimeTooShort();
    }

    protected Handler handler = new Handler(Looper.getMainLooper());
    protected boolean alive = false;
    protected long recordingTimeSeconds = 0;
    protected long recordingTimeMinutes = 0;

    protected Callback callback;

    protected TimerTaskBase(Callback callback) {
        this.callback = callback;
    }

    public abstract boolean stop();

    public abstract void start();

    public abstract long getRecordTotalSeconds();
}
