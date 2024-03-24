#version 430 core

uniform sampler2D uDepthTexture; // Input texture
layout(r32f, binding = 0) uniform image2D uOutputTexture; // Output texture

layout (local_size_x = 16, local_size_y = 16) in;
/*
void main() {
    ivec2 texCoord = ivec2(gl_GlobalInvocationID.xy);
    
    // Example: Read the depth value
    float depth = texture(uDepthTexture, vec2(texCoord) / vec2(textureSize(uDepthTexture, 0))).r;

    // Apply processing (e.g., Gaussian blur) to the depth value...

    // Write the processed value to the output texture
    imageStore(uOutputTexture, texCoord, vec4(depth, 0.0, 0.0, 0.0));
}

*/
void main() {
    ivec2 texSize = textureSize(uDepthTexture, 0);
    ivec2 uv = ivec2(gl_GlobalInvocationID.xy);
    
    vec4 sum = vec4(0.0);
    int kernel[4] = int[](1, 2, 3, 4); // Example offsets for Kawase blur iterations
    
    // For a simple Kawase iteration, just use one offset from the kernel
    // In a full implementation, you might loop through these or use multiple passes
    int offset = kernel[0];
    
    for (int j = -offset; j <= offset; j++) {
        for (int i = -offset; i <= offset; i++) {
			float depth = texture(uDepthTexture, vec2(uv + vec2(i,j)) / vec2(texSize)).r;
            //sum += imageLoad(uDepthTexture, uv + ivec2(i, j));
            sum += depth;
        }
    }
    
    // Normalizing the sum by the number of samples taken
    sum /= (2 * offset + 1) * (2 * offset + 1);
    
    imageStore(uOutputTexture, uv, sum);
}