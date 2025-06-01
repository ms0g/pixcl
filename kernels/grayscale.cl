__kernel void grayscale(__global const uchar4* input,
                        __global uchar* output,
                        const int width,
                        const int height) {
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    if (x >= width || y >= height)
        return;

    const int idx = (y * width + x);

    uchar4 rgba = input[idx];

    uchar gray = (uchar)dot(convert_float3(rgba.xyz), (float3)(0.299f, 0.587f, 0.114f));

    output[y * width + x] = gray;
}
