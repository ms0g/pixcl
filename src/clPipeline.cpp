#include "clPipeline.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include "io.hpp"

CLPipeline::CLPipeline() {
    // Get Platform and Device Info
    err = clGetPlatformIDs(1, &platform, &platformCount);
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, &deviceCount);

    // Create OpenCL context
    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create a context.");
    }

    // Create Command Queue
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    if (err != CL_SUCCESS) {
        throw std::runtime_error("Failed to create a command queue.");
    }
}

CLPipeline::~CLPipeline() {
    clReleaseEvent(readEvent);
    clReleaseEvent(writeEvent);
    clReleaseEvent(kernelEvent);
    err = clReleaseKernel(kernel);
    err = clReleaseProgram(program);
    err = clReleaseMemObject(inputBuffer);
    err = clReleaseMemObject(outputBuffer);
    err = clReleaseCommandQueue(queue);
    err = clReleaseContext(context);
}

void CLPipeline::execute() {
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
}

void CLPipeline::createBuffer(const BufferType type, const cl_mem_flags flags) {
    switch (type) {
        case BufferType::INPUT:
            inputBuffer = clCreateBuffer(context, flags, size * sizeof(cl_uchar), nullptr, nullptr);
            break;
        case BufferType::OUTPUT:
            outputBuffer = clCreateBuffer(context, flags, size * sizeof(cl_uchar), nullptr, nullptr);
            break;
        case BufferType::KERNEL:
            kernelBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(gaussianKernel),
                                          (void*) gaussianKernel, nullptr);
            break;
    }
}

void CLPipeline::writeBuffer(const void* data, const size_t offset) {
    // Transfer data to GPU
    clEnqueueWriteBuffer(queue, inputBuffer, CL_FALSE, offset, size * sizeof(cl_uchar), data, 0, nullptr,
                         &writeEvent);
}

void CLPipeline::readBuffer(void* data, const size_t offset) {
    err = clEnqueueReadBuffer(queue, outputBuffer,CL_FALSE, offset, size * sizeof(cl_uchar), data, 1, &kernelEvent,
                              &readEvent);

    // Wait for the reading buffer to finish
    clWaitForEvents(1, &readEvent);
}

void CLPipeline::createProgram(const char* kernelName) {
    const std::string source = loadKernelSource(fs::path(std::string("kernels/") + kernelName + ".cl").c_str());
    const char* source_str = source.c_str();
    const size_t source_size = source.size();

    program = clCreateProgramWithSource(context, 1, &source_str, &source_size, &err);

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
}

void CLPipeline::createKernel(const char* kernelName) {
    kernel = clCreateKernel(program, kernelName, &err);
}

void CLPipeline::printProfilingInfo() const {
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
}

std::string CLPipeline::loadKernelSource(const char* filename) {
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
