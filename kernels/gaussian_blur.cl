#define TILE_SIZE 16
#define KERNEL_RADIUS 2
#define KERNEL_SIZE (2 * KERNEL_RADIUS + 1)

__kernel void gaussian_blur(__global const uchar4* input, 
                            __global uchar4* output, 
                            const int width, 
                            const int height, 
                            __constant float* mkernel) {
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
    
    float4 sum = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
    for (int ky = -KERNEL_RADIUS; ky <= KERNEL_RADIUS; ky++) {
        int kernel_y = (ky + KERNEL_RADIUS) * KERNEL_SIZE;
        
        for (int kx = -KERNEL_RADIUS; kx <= KERNEL_RADIUS; kx++) {
            int ix = clamp(lx + kx, 0, TILE_SIZE - 1);
            int iy = clamp(ly + ky, 0, TILE_SIZE - 1);
            
            uchar4 rgba = tile[iy][ix];
            
            float weight = mkernel[kernel_y + (kx + KERNEL_RADIUS)];
            sum += convert_float4(rgba) * weight;
        }
    }

    output[idx] = (uchar4)(
        clamp(sum.x, 0.0f, 255.0f), 
        clamp(sum.y, 0.0f, 255.0f), 
        clamp(sum.z, 0.0f, 255.0f), 
        255);
}