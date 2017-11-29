/*
 * MIT License
 *
 * Copyright (c) 2017 Huawque
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @author:weichang
 * https://github.com/javandoc/MediaPlus
 */

package app.mobile.nativeapp.com.applicationmanagement.permission;

import android.app.Activity;
import android.util.Log;

import com.tbruyelle.rxpermissions2.Permission;
import com.tbruyelle.rxpermissions2.RxPermissions;

import java.lang.ref.SoftReference;
import java.lang.ref.WeakReference;

import app.mobile.nativeapp.com.applicationmanagement.inter.PermissionCheckResult;
import io.reactivex.functions.Consumer;
import rx.functions.Action1;

import static okhttp3.internal.Internal.instance;

/**
 * 权限管理
 * Created by android on 10/20/17.
 */

public class PermissionManager {


    private static Activity mActivity;
    private static SoftReference<Activity> activitySoftReference;

    private PermissionManager() {

    }

    private static PermissionManager instance = null;

    public static PermissionManager getInstance(Activity activity) {
        activitySoftReference = new SoftReference<>(activity);
        if (null == instance) {
            instance = new PermissionManager();
        }
        return instance;
    }

    private RxPermissions rxPermissions;

    public void permissonCheck(final PermissionCheckResult permissionCheckResult, String... permissions) {
        final boolean[] results = new boolean[permissions.length];
        final int[] index = {0};
        new RxPermissions(activitySoftReference.get()).request(permissions).subscribe(new Consumer<Boolean>() {
            @Override
            public void accept(Boolean aBoolean) throws Exception {
                Log.d("permission",aBoolean+"");
            }
        });
    }
}
