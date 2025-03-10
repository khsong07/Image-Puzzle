#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <CL/opencl.h>
#include <stdlib.h>
#include <math.h>

#define LOG_TAG "OpenCLDriver"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

const char* kernelSource =
        "__kernel void rotate90(__global uint* input, __global uint* output, int width, int height) {\n"
        "    int x = get_global_id(0);\n"
        "    int y = get_global_id(1);\n"
        "    if (x < width && y < height) {\n"
        "        output[x * height + (height - 1 - y)] = input[y * width + x];\n"
        "    }\n"
        "}\n"
        "\n"
        "__kernel void rotate180(__global uint* input, __global uint* output, int width, int height) {\n"
        "    int x = get_global_id(0);\n"
        "    int y = get_global_id(1);\n"
        "    if (x < width && y < height) {\n"
        "        output[(height - 1 - y) * width + (width - 1 - x)] = input[y * width + x];\n"
        "    }\n"
        "}\n"
        "\n"
        "__kernel void invertColors(__global uint* input, __global uint* output, int width, int height) {\n"
        "    int x = get_global_id(0);\n"
        "    int y = get_global_id(1);\n"
        "    if (x < width && y < height) {\n"
        "        uint pixel = input[y * width + x];\n"
        "        output[y * width + x] = ((pixel & 0xFF000000) |\n"
        "                                 ((~pixel) & 0x00FFFFFF));\n"
        "    }\n"
        "}\n"
        "\n"
        "__kernel void grayscale(__global uint* input, __global uint* output, int width, int height) {\n"
        "    int x = get_global_id(0);\n"
        "    int y = get_global_id(1);\n"
        "    if (x < width && y < height) {\n"
        "        uint pixel = input[y * width + x];\n"
        "        uchar4 color = (uchar4)(pixel & 0xFF, (pixel >> 8) & 0xFF, (pixel >> 16) & 0xFF, (pixel >> 24) & 0xFF);\n"
        "        uchar gray = (uchar)(0.299f * color.x + 0.587f * color.y + 0.114f * color.z);\n"
        "        output[y * width + x] = (gray) | (gray << 8) | (gray << 16) | (color.w << 24);\n"
        "    }\n"
        "}\n"
        "\n"
        "__kernel void gaussianBlur(__global uint* input, __global uint* output, int width, int height, int radius) {\n"
        "    int x = get_global_id(0);\n"
        "    int y = get_global_id(1);\n"
        "    \n"
        "    if (x >= width || y >= height) return;\n"
        "    \n"
        "    float4 sum = (float4)(0.0f);\n"
        "    float weight_sum = 0.0f;\n"
        "    \n"
        "    for(int ky = -radius; ky <= radius; ky++) {\n"
        "        for(int kx = -radius; kx <= radius; kx++) {\n"
        "            int ix = clamp(x + kx, 0, width - 1);\n"
        "            int iy = clamp(y + ky, 0, height - 1);\n"
        "            \n"
        "            uint pixel = input[iy * width + ix];\n"
        "            float4 color = (float4)(\n"
        "                (pixel & 0xFF),\n"
        "                ((pixel >> 8) & 0xFF),\n"
        "                ((pixel >> 16) & 0xFF),\n"
        "                ((pixel >> 24) & 0xFF)\n"
        "            );\n"
        "            \n"
        "            float weight = exp(-(kx * kx + ky * ky) / (2.0f * radius * radius));\n"
        "            sum += color * weight;\n"
        "            weight_sum += weight;\n"
        "        }\n"
        "    }\n"
        "    \n"
        "    sum /= weight_sum;\n"
        "    output[y * width + x] = \n"
        "        ((uint)sum.x) |\n"
        "        ((uint)sum.y << 8) |\n"
        "        ((uint)sum.z << 16) |\n"
        "        ((uint)sum.w << 24);\n"
        "}\n";

JNIEXPORT jobject JNICALL
Java_com_example_finalproject_MainActivity_processImageGPU(
        JNIEnv* env, jobject thiz, jobject bitmap, jint operation) {
    AndroidBitmapInfo info;
    void* pixels;
    int ret;
    cl_int err;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed! error=%d", ret);
        return NULL;
    }

    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888!");
        return NULL;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed! error=%d", ret);
        return NULL;
    }

    // OpenCL 설정
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    // 플랫폼과 디바이스 선택
    if(clGetPlatformIDs(1, &platform, NULL) != CL_SUCCESS) {
        LOGE("Failed to get OpenCL platform");
        goto cleanup;
    }

    if(clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL) != CL_SUCCESS) {
        LOGE("Failed to get OpenCL device");
        goto cleanup;
    }

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    queue = clCreateCommandQueue(context, device, 0, &err);

    program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, &err);
    if(clBuildProgram(program, 1, &device, NULL, NULL, NULL) != CL_SUCCESS) {
        LOGE("Failed to build OpenCL program");
        goto cleanup;
    }

    // 커널 선택
    const char* kernelName;
    switch(operation) {
        case 0: kernelName = "rotate90"; break;
        case 1: kernelName = "rotate180"; break;
        case 2: kernelName = "invertColors"; break;
        case 3: kernelName = "grayscale"; break;
        case 4: kernelName = "gaussianBlur"; break;
        default: return NULL;
    }

    kernel = clCreateKernel(program, kernelName, &err);

    size_t dataSize = info.height * info.width * sizeof(uint32_t);
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, dataSize, NULL, &err);
    cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, dataSize, NULL, &err);

    err = clEnqueueWriteBuffer(queue, inputBuffer, CL_TRUE, 0, dataSize, pixels, 0, NULL, NULL);
    if(err != CL_SUCCESS) {
        LOGE("Failed to write to input buffer");
        goto cleanup;
    }

    // 커널 인자 설정
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 2, sizeof(int), &info.width);
    clSetKernelArg(kernel, 3, sizeof(int), &info.height);
    if (operation == 4) {
        int radius = 2;
        clSetKernelArg(kernel, 4, sizeof(int), &radius);
    }

    // 커널 실행
    size_t globalWorkSize[2] = {info.width, info.height};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, NULL);
    if(err != CL_SUCCESS) {
        LOGE("Failed to execute kernel");
        goto cleanup;
    }

    err = clEnqueueReadBuffer(queue, outputBuffer, CL_TRUE, 0, dataSize, pixels, 0, NULL, NULL);
    if(err != CL_SUCCESS) {
        LOGE("Failed to read output buffer");
        goto cleanup;
    }

    cleanup:
    if(inputBuffer) clReleaseMemObject(inputBuffer);
    if(outputBuffer) clReleaseMemObject(outputBuffer);
    if(kernel) clReleaseKernel(kernel);
    if(program) clReleaseProgram(program);
    if(queue) clReleaseCommandQueue(queue);
    if(context) clReleaseContext(context);

    AndroidBitmap_unlockPixels(env, bitmap);
    return bitmap;
}
