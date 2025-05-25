__kernel void grayscale(__global const uchar* input,
                        __global uchar* output,
                        int width,
                        int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    if (x >= width || y >= height)
        return;

    int idx = (y * width + x) * 3;

    uchar r = input[idx];
    uchar g = input[idx + 1];
    uchar b = input[idx + 2];

    // Use standard luminance formula
    uchar gray = (uchar)(0.299f * r + 0.587f * g + 0.114f * b);

    output[y * width + x] = gray;
}
