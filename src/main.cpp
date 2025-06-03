#include <iostream>
#include <fstream>
#include "clPipeline.h"
#include "image.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

#define STRINGIFY0(s) # s
#define STRINGIFY(s) STRINGIFY0(s)
#define VERSION STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

#ifndef PROFILE
#define PROFILE 1
#endif

typedef struct Args {
    const char* effect;
    const char* format;
    const char* image;
    const char* outfile;
    int quality;
} Args;

static Args parseArgs(int argc, char** argv) {
    static const char* usage = "OVERVIEW: An OpenCL-based image processing tool\n\n"
            "USAGE: pixcl [options] <image file>\n\n"
            "OPTIONS:\n"
            "  -e, --effect          Effect to be applied[gb/gs/sep]\n"
            "  -f, --format          File format[jpg <quality 0-100>?/png/bmp/tga/raw]\n"
            "  -o, --outfile         Output file name\n"
            "  -h, --help            Display available options\n"
            "  -v, --version         Display the version of this program\n";

    Args args = {nullptr, nullptr, nullptr, nullptr};
    if (argc < 8) {
        if (argc == 2 && (!std::strcmp(argv[1], "-h") || !std::strcmp(argv[1], "--help"))) {
            std::cout << usage;
            return args;
        }

        if (argc == 2 && (!std::strcmp(argv[1], "-v") || !std::strcmp(argv[1], "--version"))) {
            std::cout << "version " << VERSION;
            return args;
        }

        throw std::runtime_error("Invalid number of arguments");
    }

    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "-e") || !std::strcmp(argv[i], "--effect")) {
            args.effect = argv[++i];
        } else if (!std::strcmp(argv[i], "-f") || !std::strcmp(argv[i], "--format")) {
            args.format = argv[++i];

            if (std::strcmp(args.format, "jpg") != 0 &&
                std::strcmp(args.format, "png") != 0 &&
                std::strcmp(args.format, "bmp") != 0 &&
                std::strcmp(args.format, "tga") != 0 &&
                std::strcmp(args.format, "raw") != 0) {
                throw std::runtime_error("Unknown Format: " + std::string(args.format));
            }

            int tmp = i;
            args.quality = static_cast<int>(strtol(argv[++tmp], nullptr, 10));
        } else if (!std::strcmp(argv[i], "-o") || !std::strcmp(argv[i], "--outfile")) {
            args.outfile = argv[++i];
        } else {
            args.image = argv[i];
        }
    }

    return args;
}

int main(int argc, char** argv) {
    // Parse Arguments
    const Args args = parseArgs(argc, argv);
    if (args.image == nullptr) return 0;

    Image in{}, out{};
    in.load(args.image);

    const ImageFormat format = Image::getFormat(args.format);
    out.create(in.width(), in.height(), 4, format);

    CLPipeline pipeline;
    cl_mem inputBuffer = pipeline.createBuffer(BufferType::INPUT, in.width(), in.height(),
                                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, in.raw());
    cl_mem outputBuffer = pipeline.createBuffer(BufferType::OUTPUT, out.width(), out.height(), CL_MEM_WRITE_ONLY);

    if (!std::strcmp(args.effect, "gb")) {
        constexpr int kernelRadius = 2;
        cl_mem kernelBuffer = pipeline.createBuffer(BufferType::KERNEL);
        // Create Program
        pipeline.createProgram("gaussian_blur");
        // Create Kernel
        pipeline.createKernel("gaussian_blur");
        pipeline.setKernelArgs(inputBuffer, outputBuffer, in.width(), in.height(), kernelBuffer, kernelRadius);
    } else if (!std::strcmp(args.effect, "gs")) {
        // Create Program
        pipeline.createProgram("grayscale");
        // Create Kernel
        pipeline.createKernel("grayscale");
        pipeline.setKernelArgs(inputBuffer, outputBuffer, in.width(), in.height());
    } else if (!std::strcmp(args.effect, "sep")) {
        // Create Program
        pipeline.createProgram("sepia_filter");
        // Create Kernel
        pipeline.createKernel("sepia_filter");
        pipeline.setKernelArgs(inputBuffer, outputBuffer, in.width(), in.height());
    }

    pipeline.execute(in.width(), in.height());

    pipeline.readBuffer(outputBuffer, out.raw(), out.width(), out.height());

    out.write(args.outfile);

    if constexpr (PROFILE)
        pipeline.printProfilingInfo();

    return 0;
}
