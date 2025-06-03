__kernel void gaussian_blur(__global const uchar4* input, 
                            __global uchar4* output, 
                            const int width, 
                            const int height, 
                            __constant float* mkernel, 
                            int kernel_radius) {
    const int x = get_global_id(0);
    const int y = get_global_id(1);

    if (x >= width || y >= height) 
		return;

    const int kernel_size = 2 * kernel_radius + 1;
    
    float3 sum = (float3)(0.0f, 0.0f, 0.0f);
    for (int ky = -kernel_radius; ky <= kernel_radius; ky++) {
        for (int kx = -kernel_radius; kx <= kernel_radius; kx++) {
            int ix = clamp(x + kx, 0, width - 1);
            int iy = clamp(y + ky, 0, height - 1);

            int idx = (iy * width + ix);
            
            uchar4 rgba = input[idx];
            
            float weight = mkernel[(ky + kernel_radius) * kernel_size + (kx + kernel_radius)];
            sum += convert_float3(rgba.xyz) * weight;
        }
    }

    int out_idx = (y * width + x);

    output[out_idx] = (uchar4)(clamp(sum.x, 0.0f, 255.0f), 
                               clamp(sum.y, 0.0f, 255.0f), 
                               clamp(sum.z, 0.0f, 255.0f), 
                               255);
}