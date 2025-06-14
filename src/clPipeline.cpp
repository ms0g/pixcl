#include "clPipeline.h"
#include <iostream>
#include <fstream>
#include <format>
#include <stdexcept>
#include "io.hpp"
#include "clError.hpp"

CLPipeline::CLPipeline() {
    // Get Platform and Device Info
    err = clGetPlatformIDs(1, &platform, &platformCount);
    checkError(err, "Failed to get platform IDs");

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &deviceCount);
    checkError(err, "Failed to get device IDs");

    // Create OpenCL context
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    checkError(err, "Failed to create the context");

    // Create Command Queue
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    checkError(err, "Failed to create the command queue");
}

CLPipeline::~CLPipeline() {
    clReleaseEvent(readEvent);
    clReleaseEvent(writeEvent);
    clReleaseEvent(kernelEvent);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseMemObject(inputBuffer);
    clReleaseMemObject(outputBuffer);
    clReleaseMemObject(kernelBuffer);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);
}

void CLPipeline::execute(const int width, const int height) {
    // Set the work item size
    size_t maxGroupSize;
    clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxGroupSize, nullptr);

    const auto side = static_cast<size_t>(sqrt(maxGroupSize));
    const size_t localWorkSize[2] = {side, side};

    const size_t globalWorkSize[2] = {
        ((width + localWorkSize[0] - 1) / localWorkSize[0]) * localWorkSize[0],
        ((height + localWorkSize[1] - 1) / localWorkSize[1]) * localWorkSize[1]
    };
    // Execute Kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 2, nullptr, globalWorkSize, localWorkSize, 0, nullptr,
                                 &kernelEvent);
    checkError(err, "Failed to execute the kernel");
}

cl_mem CLPipeline::createBuffer(const BufferType type, const int width, const int height, const cl_mem_flags flags,
                                void* ptr) {
    switch (type) {
        case BufferType::INPUT:
            inputBuffer = clCreateBuffer(context, flags, width * height * sizeof(cl_uchar4), ptr, &err);
            return inputBuffer;
        case BufferType::OUTPUT:
            outputBuffer = clCreateBuffer(context, flags, width * height * sizeof(cl_uchar4), ptr, &err);
            return outputBuffer;
        case BufferType::KERNEL:
            kernelBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(gaussianKernel),
                                          (void*) gaussianKernel, &err);
            return kernelBuffer;
    }

    return nullptr;
}

void CLPipeline::writeBuffer(cl_mem buffer, const void* data, const int width, const int height, const int channels,
                             const size_t offset) {
    // Transfer data to GPU
    err = clEnqueueWriteBuffer(queue, buffer, CL_FALSE, offset, width * height * channels * sizeof(cl_uchar), data,
                               0, nullptr, &writeEvent);
    checkError(err, "Failed to write data to the buffer");
}

void CLPipeline::readBuffer(cl_mem buffer, void* data, const int width, const int height, const size_t offset) {
    err = clEnqueueReadBuffer(queue, buffer, CL_FALSE, offset, width * height * sizeof(cl_uchar4), data,
                              1, &kernelEvent, &readEvent);
    checkError(err, "Failed to read data from the buffer");
    // Wait for the reading buffer to finish
    clWaitForEvents(1, &readEvent);
}

void CLPipeline::createProgram(const char* kernelName) {
    const std::string source = loadKernelSource(fs::path(std::string("kernels/") + kernelName + ".cl").c_str());
    const char* source_str = source.c_str();
    const size_t source_size = source.size();

    program = clCreateProgramWithSource(context, 1, &source_str, &source_size, &err);

    err = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
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
}

void CLPipeline::createKernel(const char* kernelName) {
    kernel = clCreateKernel(program, kernelName, &err);
    checkError(err, "Failed to create the kernel");
}

void CLPipeline::printProfilingInfo() const {
    // Get profiling information
    cl_ulong start, end;
    clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_START, sizeof(start), &start, nullptr);
    clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_END, sizeof(end), &end, nullptr);
    std::cout << "Kernel Execution Time: " << static_cast<double>(end - start) / 1000.0 << " ms" << std::endl;

    clGetEventProfilingInfo(readEvent, CL_PROFILING_COMMAND_START, sizeof(start), &start, nullptr);
    clGetEventProfilingInfo(readEvent, CL_PROFILING_COMMAND_END, sizeof(end), &end, nullptr);
    std::cout << "Data Read Time: " << static_cast<double>(end - start) / 1000.0 << " ms" << std::endl;
}

std::string CLPipeline::loadKernelSource(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open kernel source file");
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

void CLPipeline::checkError(const cl_int err, const char* msg) const {
    if (err != CL_SUCCESS) {
        throw std::runtime_error(std::format("{}: {}\n", msg, clErrorString(err)));
    }
}
