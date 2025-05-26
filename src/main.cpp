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
        } else if (argc == 2 && (!std::strcmp(argv[1], "-v") || !std::strcmp(argv[1], "--version"))) {
            std::cout << "version " << VERSION;
        } else {
            std::cout << usage;
            return args;
        }
        return args;
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
                throw std::runtime_error("Error: Unknown Format: " + std::string(args.format));
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

    if (args.image == nullptr) return 1;

    Image in{}, out{};
    in.load(args.image);

    const ImageFormat format = img::getFormat(args.format);

    if (!std::strcmp(args.effect, "gb") || !std::strcmp(args.effect, "sep")) {
        out.create(in.width(), in.height(), in.channels(), format);
    } else if (!std::strcmp(args.effect, "gs")) {
        out.create(in.width(), in.height(), 1, format);
    }

    CLPipeline pipeline;
    pipeline.setImageProperties(in.width(), in.height());

    pipeline.createBuffer(BufferType::INPUT, in.channels(), CL_MEM_READ_ONLY);
    pipeline.createBuffer(BufferType::OUTPUT, out.channels(), CL_MEM_WRITE_ONLY);

    pipeline.writeBuffer(in.raw(), in.channels());

    const int width = in.width();
    const int height = in.height();

    if (!std::strcmp(args.effect, "gb")) {
        constexpr int kernelRadius = 2;
        cl_mem kernelBuffer = pipeline.createBuffer(BufferType::KERNEL);
        // Create Program
        pipeline.createProgram("gaussian_blur");
        // Create Kernel
        pipeline.createKernel("gaussian_blur");
        pipeline.setKernelArgs(width, height, kernelBuffer, kernelRadius);
    } else if (!std::strcmp(args.effect, "gs")) {
        // Create Program
        pipeline.createProgram("grayscale");
        // Create Kernel
        pipeline.createKernel("grayscale");
        pipeline.setKernelArgs(width, height);
    } else if (!std::strcmp(args.effect, "sep")) {
        // Create Program
        pipeline.createProgram("sepia_filter");
        // Create Kernel
        pipeline.createKernel("sepia_filter");
        pipeline.setKernelArgs(width, height);
    }

    pipeline.execute();

    pipeline.readBuffer(out.raw(), out.channels());

    out.write(args.outfile);

    if constexpr (PROFILE)
        pipeline.printProfilingInfo();

    return 0;
}
