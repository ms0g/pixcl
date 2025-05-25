__kernel void sepia_filter(__global const uchar* input,
                           __global uchar* output,
                           const int width,
                           const int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    int idx = (y * width + x) * 3;

    if (x >= width || y >= height) return;

    uchar r = input[idx];
    uchar g = input[idx + 1];
    uchar b = input[idx + 2];

    float new_r = (r * 0.393f + g * 0.769f + b * 0.189f);
    float new_g = (r * 0.349f + g * 0.686f + b * 0.168f);
    float new_b = (r * 0.272f + g * 0.534f + b * 0.131f);

    // Clamp to 255
    output[idx] = (uchar)(fmin(new_r, 255.0f));
    output[idx + 1] = (uchar)(fmin(new_g, 255.0f));
    output[idx + 2] = (uchar)(fmin(new_b, 255.0f));
}
