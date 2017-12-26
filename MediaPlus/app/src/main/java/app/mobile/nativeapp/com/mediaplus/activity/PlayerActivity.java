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

package app.mobile.nativeapp.com.mediaplus.activity;

import android.os.Bundle;
import android.view.SurfaceView;

import app.mobile.nativeapp.com.libmedia.core.player.MediaPlusPlayer;
import app.mobile.nativeapp.com.mediaplus.R;
import butterknife.ButterKnife;

/**
 * SDL_android.c 是底层与SDLActivity的回调等相关调用
 */

public class PlayerActivity extends BaseActivity {


    private SurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);
        ButterKnife.bind(this);

        mSurfaceView = (SurfaceView) findViewById(R.id.surfaceView);

        MediaPlusPlayer mediaPlusPlayer = new MediaPlusPlayer();
        mediaPlusPlayer.setDisplay(mSurfaceView.getHolder());
        mediaPlusPlayer.setScreenOnWhilePlaying(true);
    }
}
