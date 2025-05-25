#include <iostream>
#include <fstream>
#include "clPipeline.h"
#include "image.h"

#ifndef PROFILE
#define PROFILE 1
#endif

int main(int argc, char** argv) {
    CLPipeline pipeline;
    Image inImage{}, outImage{};

    inImage.load("lenna.png");
    outImage.create(inImage.width(), inImage.height(), inImage.channels(), ImageFormat::PNG);

    pipeline.setImageProperties(inImage.width(), inImage.height(), inImage.channels());

    pipeline.createBuffer(BufferType::INPUT, CL_MEM_READ_ONLY);
    pipeline.createBuffer(BufferType::OUTPUT, CL_MEM_WRITE_ONLY);
    pipeline.createBuffer(BufferType::KERNEL);

    pipeline.writeBuffer(inImage.raw());

    // Create Program
    pipeline.createProgram("gaussian_blur");

    // Create Kernel
    pipeline.createKernel("gaussian_blur");

    constexpr int kernelRadius = 2;
    const int width = inImage.width();
    const int height = inImage.height();
    const cl_mem kernelBuffer = pipeline.getBuffer(BufferType::KERNEL);

    pipeline.setKernelArgs(width, height, kernelBuffer, kernelRadius);

    pipeline.execute();

    pipeline.readBuffer(outImage.raw());

    outImage.write("out.png");

    if constexpr (PROFILE)
        pipeline.printProfilingInfo();

    return 0;
}
