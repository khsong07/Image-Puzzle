# Image-Puzzle
<img src="./assets/sample img.png" width="400">

# 📂 Project Structure
```
Image-Puzzle
├── app
│   ├── 📂 src
│   │   ├── 📂 main
│   │   │   ├── 📂 java/com/example/finalproject
│   │   │   ├── 📂 cpp
│   │   │   ├── 📂 res
│   │   │   ├── AndroidManifest.xml
│   │   │   └── CMakeLists.txt
│   │   ├── 📂 test
│   │   ├── 📂 androidTest
│   ├── build.gradle.kts
│   ├── proguard-rules.pro
│   └── .gitignore
├── config
│   ├── build.gradle.kts
│   ├── gradle.properties
│   ├── gradlew
│   ├── gradlew.bat
│   ├── settings.gradle.kts
├── .gitignore
├── README.md
```
## Motivation🚀
The goal of this project was to develop an Android application that showcases real-time image processing performance differences between the CPU and GPU. Additionally, it introduces users to fundamental image processing concepts through an engaging game. Various sensors available on the board were utilized to enhance the gaming experience.
```
Tech Stack ⚙️
- Language: Java, C
- Framework: Android SDK
- Parallel Computing: OpenCL
- Device: HANBACK HBE-SM9-Smart 
```

## Overview🔬
This repository hosts the code for Image-Puzzle, a game that utilizes CPU and GPU (OpenCL) to process user-captured images in four different ways. The goal of the game is to quickly analyze and identify which transformation has been applied to the displayed image for users. By leveraging parallel computing with OpenCL, the game processes images efficiently and in real time, providing both an engaging challenge and a demonstration of CPU vs. GPU performance differences.

## Layout📖
The game consists of two main pages:

1. Camera Page
   - Displays a camera preview at the top of the screen.
   - When the user clicks the "Start" button, the captured image is saved, and the game automatically navigates to the next page.  

2. Game Page
   - The game uses the image taken from the previous screen.
   - Clicking the "Start" button begins the game, which has a 20-second time limit.
   - The highest score is displayed in the top-right corner, while the current score updates in the bottom-left corner.
   - The "Stop" button allows the user to exit the game at any time.
   <br>   
   <img src="./assets/layoutpreview.png" width="600">

## About Game🎰
### 1. How to play
   - Randomly displays one of four image processing results at 1-second intervals.
   - Click the button at the bottom to make a selection.
   - Correct answers earn +5 points, while incorrect answers earn 0 points.
   - If the answer is correct, "ANSWER!!" is displayed.
   - If the answer is incorrect, "NO!!" is displayed.
   
### 2. Image Processing
   - Grayscale
   - Gaussian Blur (**Implemented with OpenCL for parallel processing**) 
   - 180 degree roatate
   - Inversion
     <br>
     <br>
      <img src="./assets/image processing.png" width="600">

### 3. 7-segments
   - The highest score is displayed using a 7-segment display attached to the board.
   - A custom device driver was developed to interface with the 7-segment display.
     <br><br>
      <img src="./assets/7-segments.png" width="600">

## Demo 
![KakaoTalk_20250312_010545288](./assets/demo.gif)


