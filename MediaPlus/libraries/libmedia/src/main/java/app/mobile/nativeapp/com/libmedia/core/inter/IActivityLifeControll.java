package app.mobile.nativeapp.com.libmedia.core.inter;

/**
 * Created by android on 3/24/18.
 */

public interface IActivityLifeControll {

    void onActivityCreate();

    void onActivityDestroy();

    void onActivityStop();

    void onActivityResume();

    void onActivityPause();
}
