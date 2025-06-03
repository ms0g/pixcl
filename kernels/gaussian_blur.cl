#define TILE_SIZE 16

__kernel void gaussian_blur(__global const uchar4* input, 
                            __global uchar4* output, 
                            const int width, 
                            const int height, 
                            __constant float* mkernel, 
                            int kernel_radius) {
    const int x = get_global_id(0);
    const int y = get_global_id(1);
    const int lx = get_local_id(0);  // Local x
    const int ly = get_local_id(1);  // Local y
    const int idx = y * width + x;

    if (x >= width || y >= height) 
		return;

    __local uchar4 tile[TILE_SIZE][TILE_SIZE];

    tile[ly][lx] = input[idx];
    
    barrier(CLK_LOCAL_MEM_FENCE);

    const int kernel_size = 2 * kernel_radius + 1;
    
    float3 sum = (float3)(0.0f, 0.0f, 0.0f);
    for (int ky = -kernel_radius; ky <= kernel_radius; ky++) {
        for (int kx = -kernel_radius; kx <= kernel_radius; kx++) {
            int ix = clamp(lx + kx, 0, TILE_SIZE - 1);
            int iy = clamp(ly + ky, 0, TILE_SIZE - 1);
            
            uchar4 rgba = tile[iy][ix];
            
            float weight = mkernel[(ky + kernel_radius) * kernel_size + (kx + kernel_radius)];
            sum += convert_float3(rgba.xyz) * weight;
        }
    }

    output[idx] = (uchar4)(
        clamp(sum.x, 0.0f, 255.0f), 
        clamp(sum.y, 0.0f, 255.0f), 
        clamp(sum.z, 0.0f, 255.0f), 
        255);
}