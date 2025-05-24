#include <iostream>
#include <fstream>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include "image.h"
#include "io.hpp"

#ifndef PROFILE
#define PROFILE 0
#endif

constexpr float gaussian_kernel[25] = {
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
    0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
    0.003765, 0.015019, 0.023792, 0.015019, 0.003765
};

std::string loadKernelSource(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open kernel source file." << std::endl;
        return {};
    }

    file.seekg(0, std::ios::end);
    const std::size_t length = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string source;
    source.resize(length);
    file.read(source.data(), length);
    file.close();

    return source;
}

int main(int argc, char** argv) {
    Image inImage{}, outImage{};

    inImage.load("/Users/msamigurpinar/workspace/pixcl/pris.jpg");
    outImage.create(inImage.width(), inImage.height(), inImage.channels(), ImageFormat::JPG);

    cl_device_id device;
    cl_platform_id platform;
    cl_event readEvent, writeEvent, kernelEvent;
    cl_uint platformCount, deviceCount;

    // Get Platform and Device Info
    cl_int err = clGetPlatformIDs(1, &platform, &platformCount);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &deviceCount);

    // Create OpenCL context
    const cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cerr << "Error: Failed to create a context." << std::endl;
        return -1;
    }
    // Create Command Queue
    const cl_command_queue queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    //
    const cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY, inImage.size() * sizeof(cl_uchar), nullptr,
                                              nullptr);
    const cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, inImage.size() * sizeof(cl_uchar), nullptr,
                                               nullptr);
    // Transfer data to GPU
    clEnqueueWriteBuffer(queue, inputBuffer,CL_FALSE, 0, inImage.size() * sizeof(cl_uchar), inImage.raw(), 0, nullptr,
                         &writeEvent);

    // Create buffer for Gaussian kernel
    const cl_mem kernelBuffer = clCreateBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(gaussian_kernel),
                                               (void*) gaussian_kernel, nullptr);

    // Create Program
    // Load Kernel Source
    const std::string source = loadKernelSource(fs::path("kernels/gaussian_blur.cl").c_str());
    const char* source_str = source.c_str();
    const size_t source_size = source.size();
    const cl_program program = clCreateProgramWithSource(context, 1, &source_str, &source_size, &err);

    // Build Program
    err = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    if (err != 0) {
        // Determine the size of the log
        size_t log_size;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
        // Allocate memory for the log
        std::string log;
        log.resize(log_size);
        // Get the log
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, (void*) log.c_str(), nullptr);
        // Print the log
        std::cout << log;
    }
    // Create Kernel
    cl_kernel kernel = clCreateKernel(program, "gaussian_blur", &err);
    constexpr int kernelRadius = 2;
    const int width = inImage.width();
    const int height = inImage.height();
    //Set Kernel arguments
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);
    clSetKernelArg(kernel, 2, sizeof(int), &width);
    clSetKernelArg(kernel, 3, sizeof(int), &height);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), &kernelBuffer);
    clSetKernelArg(kernel, 5, sizeof(int), &kernelRadius);

    // Set the work item size
    size_t maxGroupSize;
    clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxGroupSize, nullptr);
    const auto side = static_cast<size_t>(sqrt(maxGroupSize));
    const size_t localWorkSize[2] = {side, side};

    const size_t globalWorkSize[2] = {
        (width + localWorkSize[0] - 1) / localWorkSize[0] * localWorkSize[0],
        (height + localWorkSize[1] - 1) / localWorkSize[1] * localWorkSize[1]
    };

    // Execute Kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalWorkSize, localWorkSize, 1, &writeEvent,
                                 &kernelEvent);

    // We will use CL_FALSE for operations are nonblocking and dependent on completion of events
    err = clEnqueueReadBuffer(queue, outputBuffer,CL_FALSE, 0, width * height * inImage.channels() * sizeof(cl_uchar),
                              outImage.raw(), 1, &kernelEvent, &readEvent);

    // Wait for the reading buffer to finish
    clWaitForEvents(1, &readEvent);

    outImage.write("out.jpg");

    // Get profiling information
    cl_ulong start, end;
    clGetEventProfilingInfo(writeEvent, CL_PROFILING_COMMAND_START, sizeof(start), &start, nullptr);
    clGetEventProfilingInfo(writeEvent, CL_PROFILING_COMMAND_END, sizeof(end), &end, nullptr);
    std::cout << "Data Write Time: " << (end - start) / 1000.0 << " ms" << std::endl;

    clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_START, sizeof(start), &start, nullptr);
    clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_END, sizeof(end), &end, nullptr);
    std::cout << "Kernel Execution Time: " << (end - start) / 1000.0 << " ms" << std::endl;

    clGetEventProfilingInfo(readEvent, CL_PROFILING_COMMAND_START, sizeof(start), &start, nullptr);
    clGetEventProfilingInfo(readEvent, CL_PROFILING_COMMAND_END, sizeof(end), &end, nullptr);
    std::cout << "Data Read Time: " << (end - start) / 1000.0 << " ms" << std::endl;

    clReleaseEvent(readEvent);
    clReleaseEvent(writeEvent);
    clReleaseEvent(kernelEvent);

    // Clean Up
    err = clReleaseKernel(kernel);
    err = clReleaseProgram(program);
    err = clReleaseMemObject(inputBuffer);
    err = clReleaseMemObject(outputBuffer);
    err = clReleaseCommandQueue(queue);
    err = clReleaseContext(context);

    return 0;
}
