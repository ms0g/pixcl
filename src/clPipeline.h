#ifndef CLPIPELINE_H
#define CLPIPELINE_H

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <string>

enum class BufferType {
    INPUT, OUTPUT, KERNEL
};

class CLPipeline {
public:
    CLPipeline();

    ~CLPipeline();

    void execute();

    cl_mem getBuffer(const BufferType type) const {
        return type == BufferType::INPUT ? inputBuffer : type == BufferType::OUTPUT ? outputBuffer : kernelBuffer;
    }

    void createBuffer(BufferType type, int channels = 0, cl_mem_flags flags = 0);

    void writeBuffer(const void* data, int channels, size_t offset = 0);

    void readBuffer(void* data, int channels, size_t offset = 0);

    void createProgram(const char* kernelName);

    void createKernel(const char* kernelName);

    template<typename... Args>
    void setKernelArgs(Args&... args);

    void setImageProperties(int width, int height);

    void printProfilingInfo() const;

private:
    std::string loadKernelSource(const char* filename);
    // Image Properties
    int width{};
    int height{};
    // OpenCL Objects
    cl_device_id device{nullptr};
    cl_platform_id platform{nullptr};
    cl_context context{nullptr};
    cl_command_queue queue{nullptr};
    cl_program program{nullptr};
    cl_kernel kernel{nullptr};
    cl_event readEvent{nullptr}, writeEvent{nullptr}, kernelEvent{nullptr};
    cl_uint platformCount{0}, deviceCount{0};
    cl_int err{0};
    cl_mem inputBuffer{nullptr}, outputBuffer{nullptr}, kernelBuffer{nullptr};

    static constexpr float gaussianKernel[25] = {
        0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
        0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
        0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
        0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
        0.003765, 0.015019, 0.023792, 0.015019, 0.003765
    };
};

template<typename... Args>
void CLPipeline::setKernelArgs(Args&... args) {
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &outputBuffer);

    cl_uint index = 2;
    auto applyArg = [&](const auto& arg) {
        err = clSetKernelArg(kernel, index++, sizeof(arg), &arg);
    };

    (applyArg(args), ...);
}

#endif //CLPIPELINE_H
