package app.mobile.nativeapp.com.libmedia.core.utils;

import android.util.ArrayMap;
import android.view.View;

import java.util.HashMap;
import java.util.Map;


/**
 * 快速点击
 */
public class FastClickChecker {

    private Map<View, Long> viewTimeHashMap;
    public Map<Integer, Long> idTimeHashMap;

    public FastClickChecker() {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.KITKAT) {
            viewTimeHashMap = new ArrayMap<>();
            idTimeHashMap = new ArrayMap<Integer, Long>();
        } else {
            viewTimeHashMap = new HashMap<>();
            idTimeHashMap = new HashMap<Integer, Long>();
        }
    }

    private int timeSpan = 1000;

    //设置需要监听的id
    public void setView(int... viewIds) {
        for (int id : viewIds) {
            idTimeHashMap.put(id, 0L);
        }
    }

    //设置需要监听的view
    public void setView(View... views) {
        for (View v : views) {
            viewTimeHashMap.put(v, 0L);
        }
    }

    //检查是否是快速点击
    public boolean isFastClick(View view) {
        if (viewTimeHashMap.containsKey(view)) {
            Long lastClickTime = viewTimeHashMap.get(view);
            long time = System.currentTimeMillis();
            long span = time - lastClickTime;
            if (0 < span && span < timeSpan) {
                return true;
            }
            viewTimeHashMap.put(view, time);
        }
        return false;
    }

    /**
     * 检查是否是快速点击
     */
    public boolean isFastClick(int viewId) {
        if (idTimeHashMap.containsKey(viewId)) {
            Long lastClickTime = idTimeHashMap.get(viewId);
            long time = System.currentTimeMillis();
            long span = time - lastClickTime;
            if (0 < span && span < timeSpan) {
                return true;
            }
            idTimeHashMap.put(viewId, time);
        }
        return false;
    }

    /**
     * 设置认定为快速点击的时间间隔
     */
    public void setTimeSpan(int span) {
        this.timeSpan = span;
    }
}
