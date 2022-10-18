package com.jj.whitenoise;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.com.whitenoise.R;

/**
 * 类描述：
 * 创建人：jjj
 * 创建日期：2022/8/29 15:51
 */

public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_activity);
        native_onInit(getAssets());
        initMusic("打字.mp3", R.id.play_button1, R.id.seek_bar1);
        initMusic("下雨-有雷.mp3", R.id.play_button2, R.id.seek_bar2);
        initMusic("test.mp3", R.id.play_button3, R.id.seek_bar3);
    }

    private void initMusic(String music, int playButtonId, int seekBarId) {
        Button playButton = findViewById(playButtonId);
        playButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String state = playButton.getText().toString();
                if (TextUtils.equals(state, "播放")) {
                    native_onStart(music);
                    playButton.setText("暂停");
                } else if (TextUtils.equals(state, "暂停")) {
                    native_onPause(music);
                    playButton.setText("播放");
                }
            }
        });
        SeekBar seekBar = findViewById(seekBarId);
        seekBar.setProgress(100);
        seekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                native_setVolume(music, seekBar.getProgress() / 100.0f);
            }
        });
    }

    private native boolean native_onInit(AssetManager assetManager);

    private native void native_onStart(String filename);

    private native void native_onPause(String filename);

    private native void native_setVolume(String filename, float volume);
}
