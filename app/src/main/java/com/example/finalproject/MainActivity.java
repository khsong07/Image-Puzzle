package com.example.finalproject;

import androidx.appcompat.app.AppCompatActivity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.os.Handler;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import java.util.Random;

public class MainActivity extends AppCompatActivity {
    private ImageView imgV;
    private Bitmap originalBitmap;
    private TextView scoreTextView;
    private TextView timerTextView;
    private TextView resultTextView;
    private TextView highScoreTextView;
    private Button[] transformButtons;
    private Button startStopButton;

    private int score = 0;
    private int highScore = 0;
    private int timeLeft = 20;
    private boolean isGameRunning = false;
    private Handler handler = new Handler();
    private Random random = new Random();

    private static final int ROTATE_90 = 0;
    private static final int ROTATE_180 = 1;
    private static final int INVERT = 2;
    private static final int GRAYSCALE = 3;
    private static final int BLUR = 4;

    private int currentTransformation = -1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imgV = findViewById(R.id.imageView);
        scoreTextView = findViewById(R.id.scoreTextView);
        timerTextView = findViewById(R.id.timerTextView);
        resultTextView = findViewById(R.id.resultTextView);
        highScoreTextView = findViewById(R.id.highScoreTextView);
        startStopButton = findViewById(R.id.startStopButton);

        transformButtons = new Button[5];
        transformButtons[0] = findViewById(R.id.rotate90Button);
        transformButtons[1] = findViewById(R.id.rotate180Button);
        transformButtons[2] = findViewById(R.id.invertButton);
        transformButtons[3] = findViewById(R.id.grayscaleButton);
        transformButtons[4] = findViewById(R.id.blurButton);

        for (int i = 0; i < transformButtons.length; i++) {
            final int transformation = i;
            transformButtons[i].setOnClickListener(v -> checkAnswer(transformation));
        }

        startStopButton.setOnClickListener(v -> {
            if (isGameRunning) {
                stopGame();
            } else {
                startGame();
            }
        });

        byte[] byteArray = getIntent().getByteArrayExtra("capturedImage");
        if (byteArray != null) {
            originalBitmap = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.length);
            imgV.setImageBitmap(originalBitmap);
        }
    }

    private void startGame() {
        score = 0;
        timeLeft = 20;
        isGameRunning = true;
        updateUI();
        applyTransformation();
        startTimer();
        startStopButton.setText("Stop");
    }

    private void stopGame() {
        isGameRunning = false;
        handler.removeCallbacksAndMessages(null);
        if (score > highScore) {
            highScore = score;
            highScoreTextView.setText("High Score: " + highScore);
        }
        resultTextView.setText("Game Over! Your score: " + score);
        startStopButton.setText("Start");
    }

    private void applyTransformation() {
        currentTransformation = random.nextInt(5);
        Bitmap transformedBitmap = processImageGPU(originalBitmap, currentTransformation);
        imgV.setImageBitmap(transformedBitmap);
    }

    private void checkAnswer(int userAnswer) {
        if (!isGameRunning) return;

        if (userAnswer == currentTransformation) {
            score += 5;
            resultTextView.setText("Correct!");
        } else {
            resultTextView.setText("Wrong!");
        }
        updateUI();
        applyTransformation();
    }

    private void updateUI() {
        scoreTextView.setText("Score: " + score);
        timerTextView.setText("Time: " + timeLeft);
    }

    private void startTimer() {
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                timeLeft--;
                if (timeLeft > 0) {
                    updateUI();
                    handler.postDelayed(this, 1000);
                } else {
                    stopGame();
                }
            }
        }, 1000);
    }

    private native Bitmap processImageGPU(Bitmap input, int transformationType);

    static {
        System.loadLibrary("native-lib");
    }
}
