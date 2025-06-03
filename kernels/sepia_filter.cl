__kernel void sepia_filter(__global const uchar4* input,
                           __global uchar4* output,
                           const int width,
                           const int height) {
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    if (x >= width || y >= height) 
        return;

    const int idx = (y * width + x);

    uchar4 rgba = input[idx];
    // Apply sepia transformation
    // The sepia transformation matrix is:
    // | 0.393 0.769 0.189 |
    // | 0.349 0.686 0.168 |
    // | 0.272 0.534 0.131 |
    float r = dot(convert_float3(rgba.xyz), (float3)(0.393f, 0.769f, 0.189f));
    float g = dot(convert_float3(rgba.xyz), (float3)(0.349f, 0.686f, 0.168f));
    float b = dot(convert_float3(rgba.xyz), (float3)(0.272f, 0.534f, 0.131f));
   
    // Clamp to 255
    output[idx] = (uchar4)(
        fmin(r, 255.0f),
        fmin(g, 255.0f),
        fmin(b, 255.0f),
        255);

}
