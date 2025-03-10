cmake_minimum_required(VERSION 3.22.1)
project("finalproject")

# OpenCL 라이브러리와 헤더 파일 경로 설정
set(OpenCL_INCLUDE_DIR "C:/Users/Ssong/Desktop/OpenCL_lib_and_include/OpenCL_lib_and_include/include")
set(OpenCL_LIBRARY "C:/Users/Ssong/Desktop/OpenCL_lib_and_include/OpenCL_lib_and_include/lib/libOpenCL.so")

# 헤더 파일 디렉토리 추가
include_directories(${OpenCL_INCLUDE_DIR})
include_directories(src/main/cpp)

# OpenCLDriver 라이브러리 추가
add_library(
        OpenCLDriver
        SHARED
        OpenCLDriver.c
)

# AndroidBitmap 라이브러리 찾기
find_library(jnigraphics-lib jnigraphics)

# OpenCLDriver 링크
target_link_libraries(OpenCLDriver
        ${OpenCL_LIBRARY}
        log
        ${jnigraphics-lib}
)

# 추가 include 디렉토리 설정
target_include_directories(OpenCLDriver PRIVATE src/main/cpp)
