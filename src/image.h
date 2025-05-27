#ifndef IMAGE_H
#define IMAGE_H

#include <cstdlib>

enum class ImageFormat {
    JPG, PNG, BMP, TGA, RAW
};

enum class AllocationType {
    STB_ALLOCATED,
    CUSTOM_ALLOCATED
};

class Image {
public:
    Image() = default;

    ~Image();

    static ImageFormat getFormat(const char* name);

    [[nodiscard]] int width() const { return mWidth; }

    [[nodiscard]] int height() const { return mHeight; }

    [[nodiscard]] int channels() const { return mChannels; }

    [[nodiscard]] size_t size() const { return mSize; }

    [[nodiscard]] uint8_t* raw() const { return mRaw; }

    void load(const char* name);

    void create(int width, int height, int channels, ImageFormat format);

    void write(const char* name, int quality = 100) const;

private:
    int mWidth{};
    int mHeight{};
    int mChannels{};
    size_t mSize{};
    ImageFormat mFormat{};
    AllocationType mAllocType{};
    uint8_t* mRaw{nullptr};
};

#endif
