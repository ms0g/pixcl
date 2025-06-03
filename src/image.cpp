#include "image.h"
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Image::~Image() {
    if (mRaw == nullptr) return;

    switch (mAllocType) {
        case AllocationType::STB_ALLOCATED:
            stbi_image_free(mRaw);
            break;
        case AllocationType::CUSTOM_ALLOCATED:
            delete[] mRaw;
            break;
    }

    mRaw = nullptr;
}

ImageFormat Image::getFormat(const char* name) {
    return !std::strcmp(name, "jpg") || !std::strcmp(name, "jpeg")
              ? ImageFormat::JPG
              : !std::strcmp(name, "png")
                    ? ImageFormat::PNG
                    : !std::strcmp(name, "bmp")
                          ? ImageFormat::BMP
                          : !std::strcmp(name, "tga")
                                ? ImageFormat::TGA
                                : ImageFormat::RAW;
}

void Image::load(const char* name) {
    mRaw = stbi_load(name, &mWidth, &mHeight, &mChannels, STBI_rgb_alpha);

    if (mRaw == nullptr) {
        throw std::runtime_error("Failed to load image");
    }

    mSize = mWidth * mHeight * mChannels;
    mAllocType = AllocationType::STB_ALLOCATED;
}

void Image::create(const int width, const int height, const int channels, const ImageFormat format) {
    mWidth = width;
    mHeight = height;
    mChannels = channels;
    mFormat = format;
    mAllocType = AllocationType::CUSTOM_ALLOCATED;
    mSize = mWidth * mHeight * mChannels;
    mRaw = new uint8_t[mSize];
}

void Image::write(const char* name, const int quality) const {
    switch (mFormat) {
        case ImageFormat::JPG:
            stbi_write_jpg(name, mWidth, mHeight, mChannels, mRaw, quality);
            break;
        case ImageFormat::PNG:
            stbi_write_png(name, mWidth, mHeight, mChannels, mRaw, mWidth * mChannels);
            break;
        case ImageFormat::BMP:
            stbi_write_bmp(name, mWidth, mHeight, mChannels, mRaw);
            break;
        case ImageFormat::TGA:
            stbi_write_tga(name, mWidth, mHeight, mChannels, mRaw);
            break;
        case ImageFormat::RAW: {
            std::ofstream outfile(name, std::ios::binary);
            outfile.write(reinterpret_cast<const char*>(mRaw), static_cast<long>(mSize));
            outfile.close();
            break;
        }
    }
}
