__kernel void gaussian_blur(__global const uchar* input, 
                            __global uchar* output, 
                            int width, 
                            int height, 
                            __constant float* mkernel, 
                            int kernel_radius) {
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    if (x >= width || y >= height) 
		return;

    const int channels = 3;
    const int kernel_size = 2 * kernel_radius + 1;
    float3 sum = (float3)(0.0f, 0.0f, 0.0f);
    
    for (int ky = -kernel_radius; ky <= kernel_radius; ky++) {
        for (int kx = -kernel_radius; kx <= kernel_radius; kx++) {
            int ix = clamp(x + kx, 0, width - 1);
            int iy = clamp(y + ky, 0, height - 1);

            int idx = (iy * width + ix) * channels;
            
            float3 pixel;
       
            pixel.x = input[idx];
            pixel.y = input[idx + 1];
            pixel.z = input[idx + 2];
            
            float weight = mkernel[(ky + kernel_radius) * kernel_size + (kx + kernel_radius)];
            sum += pixel * weight;
        }
    }

    int out_idx = (y * width + x) * channels;

    output[out_idx] = (uchar)clamp(sum.x, 0.0f, 255.0f);
    output[out_idx + 1] = (uchar)clamp(sum.y, 0.0f, 255.0f);
    output[out_idx + 2] = (uchar)clamp(sum.z, 0.0f, 255.0f);
}