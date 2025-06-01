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

    void execute(int width, int height);

    cl_mem createBuffer(BufferType type, int width = 0, int height = 0, int channels = 0, cl_mem_flags flags = 0,
                        void* ptr = nullptr);

    void writeBuffer(cl_mem buffer, const void* data, int width, int height, int channels, size_t offset = 0);

    void readBuffer(cl_mem buffer, void* data, int width, int height, int channels, size_t offset = 0);

    void createProgram(const char* kernelName);

    void createKernel(const char* kernelName);

    template<typename... Args>
    void setKernelArgs(Args&&... args);

    void printProfilingInfo() const;

private:
    std::string loadKernelSource(const char* filename);

    void checkError(cl_int err, const char* msg) const;

    // OpenCL Objects
    cl_int err{0};
    cl_device_id device{nullptr};
    cl_platform_id platform{nullptr};
    cl_context context{nullptr};
    cl_command_queue queue{nullptr};
    cl_program program{nullptr};
    cl_kernel kernel{nullptr};
    cl_event readEvent{nullptr};
    cl_event writeEvent{nullptr};
    cl_event kernelEvent{nullptr};
    cl_uint platformCount{0};
    cl_uint deviceCount{0};
    cl_mem inputBuffer{nullptr};
    cl_mem outputBuffer{nullptr};
    cl_mem kernelBuffer{nullptr};

    static constexpr float gaussianKernel[25] = {
        0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
        0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
        0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
        0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
        0.003765, 0.015019, 0.023792, 0.015019, 0.003765
    };
};

template<typename... Args>
void CLPipeline::setKernelArgs(Args&&... args) {
    cl_uint index = 0;

    auto applyArg = [&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        clSetKernelArg(kernel, index++, sizeof(T), &arg);
    };

    (applyArg(std::forward<Args>(args)), ...);
}

#endif //CLPIPELINE_H
