package com.example.finalproject;

import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.hardware.Camera;
import android.os.Bundle;
import android.widget.*;
import androidx.appcompat.app.AppCompatActivity;
import java.io.ByteArrayOutputStream;

public class CameraActivity extends AppCompatActivity {
    private static final String TAG = "CanTestActivity";
    private Camera mCamera;
    private CameraPreview mPreview;
    private ImageView capturedImageHolder;
    private Button captureButton;
    private Button startButton;
    private Bitmap capturedBitmap;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        captureButton = findViewById(R.id.button_capture);
        startButton = findViewById(R.id.button_start);
        capturedImageHolder = findViewById(R.id.captured_image);

        mCamera = getCameraInstance();
        mCamera.setDisplayOrientation(180);
        mPreview = new CameraPreview(this, mCamera);
        FrameLayout preview = findViewById(R.id.camera_preview);
        preview.addView(mPreview);

        captureButton.setOnClickListener(v -> mCamera.takePicture(null, null, pictureCallback));

        startButton.setOnClickListener(v -> {
            if (capturedBitmap != null) {
                startGame(capturedBitmap);
            } else {
                Toast.makeText(CameraActivity.this, "이미지를 먼저 캡처해주세요", Toast.LENGTH_SHORT).show();
            }
        });
    }

    public static Camera getCameraInstance() {
        Camera c = null;
        try {
            c = Camera.open();
        } catch (Exception e) {
            // Camera is not available (in use or does not exist)
        }
        return c;
    }

    private Camera.PictureCallback pictureCallback = (data, camera) -> {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inSampleSize = 4;
        Bitmap bitmap = BitmapFactory.decodeByteArray(data, 0, data.length, options);
        if (bitmap == null) {
            Toast.makeText(CameraActivity.this, "Capture image is empty", Toast.LENGTH_LONG).show();
            return;
        }
        Matrix mMatrix = new Matrix();
        mMatrix.postRotate(180);
        capturedBitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), mMatrix, true);
        capturedImageHolder.setImageBitmap(scaleDownBitmapImage(capturedBitmap, 450, 300));
        mCamera.startPreview();
    };

    private Bitmap scaleDownBitmapImage(Bitmap bitmap, int newWidth, int newHeight) {
        return Bitmap.createScaledBitmap(bitmap, newWidth, newHeight, true);
    }

    private void startGame(Bitmap bitmap) {
        Intent intent = new Intent(CameraActivity.this, MainActivity.class);
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, stream);
        byte[] byteArray = stream.toByteArray();
        intent.putExtra("capturedImage", byteArray);
        startActivity(intent);
        finish(); // CameraActivity를 종료합니다.
    }

    @Override
    protected void onPause() {
        super.onPause();
        releaseCamera();
    }

    private void releaseCamera() {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
    }
}
