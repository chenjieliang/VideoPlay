package com.example.videoplay;

import android.Manifest;
import android.content.res.Configuration;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.PermissionChecker;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.SeekBar;

/**
 * @author chenjieliang
 */
public class TestActivity extends AppCompatActivity implements SeekBar.OnSeekBarChangeListener, View.OnClickListener {

    static {
        System.loadLibrary("native-lib");
    }

    private SurfaceView mSurfaceView;
    private long mLoginId;
    private SeekBar mSeekBar;
    private Thread mThread;
    private Runnable mRunnable;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //去掉标题栏
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
        //全屏，隐藏状态
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN
        );
        setContentView(R.layout.activity_test);

        initView();
        initData();
        initPermission();

    }

    private void initView() {
        mSurfaceView = findViewById(R.id.surfaceView);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });
        findViewById(R.id.btn_play).setOnClickListener(this);

        mSeekBar = findViewById(R.id.seek_progress);
        mSeekBar.setOnSeekBarChangeListener(this);
    }


    private void initData() {
        mRunnable = new Runnable() {
            @Override
            public void run() {
                while (true) {
                    int progress = (int) (getPlayPos() * 100);

                    //Log.i("===TAG===", "progress = " + progress * 100);
                    mSeekBar.setProgress(progress);
                    try {
                        Thread.currentThread().sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        };
        //mThread = new Thread(mRunnable);
        //mThread.start();
    }

    //在onPause中暂停会有个问题，直接卡主了
    /*@Override
    protected void onResume() {
        super.onResume();

        mVideoView.playOnPause();
        Log.i("===TAG===", "onResume");
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.i("===TAG===", "onPause");
        mVideoView.playOnPause();
    }*/

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        Log.i("===TAG===", "onConfigurationChanged");
    }

    @Override
    public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {

    }

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {
        double pos = (double) seekBar.getProgress() / (double) seekBar.getMax();
        Log.i("===TAG===", "Seek pos = " + pos);
        //seekTo(pos);
    }

    //获取播放位置
    public native double getPlayPos();

    //滑动到
    public native double seekTo(double pos);

    //播放
    public native void play();


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_play:

                break;
        }
    }

    //发送视频帧
    private native void receiverVideoFrame(
            int type,              //I、P、A
            int camType,
            int width,
            int height,
            int fps,              //帧率
            long pts,             //时间戳
            byte[] dataArray,     //unint_8 、unsigned char
            long size);

    //发送视频帧
    private native void receiverAudioFrame(int type, int channels, int sps, int bps, long pts, byte[] dataArray, long size);


    /*权限相关*/
    private void initPermission() {
        int result = PermissionChecker.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        if (result == -1) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }
}
