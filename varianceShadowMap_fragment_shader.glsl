#version 430

out vec2 variance;
in vec4 posWorld;

uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;

void main()
{

	// perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
	//vec4 sCoord =  gl_FragCoord;
    vec3 shadowCoord = sCoord.xyz / sCoord.w;
	shadowCoord = shadowCoord * 0.5 + 0.5;
	
	
	 // Sample the shadow map texture using the current shadow coordinate
    float depth = texture(shadowMap, shadowCoord.xy).r;
    
    // Compute the partial derivatives of the depth texture
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float dx = dFdx(depth) * texelSize.x;
    float dy = dFdy(depth) * texelSize.y;
    
    // Compute the filtered depth and depth variance
    float filteredDepth = depth;
    float filteredVariance = depth*depth + (dx*dx + dy*dy) / 4.0;
    
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
 
            vec2 texCoord = shadowCoord.xy + vec2(float(x), float(y)) * texelSize;
			//texCoord = texCoord * 0.5 + 0.5;
            float depthSample = texture(shadowMap, texCoord.xy).r;

            dx = dFdx(depthSample) * texelSize.x;
            dy = dFdy(depthSample) * texelSize.y;
            float varianceSample = depthSample*depthSample + (dx*dx + dy*dy) / 4.0;
            
            filteredDepth = min(filteredDepth, depthSample);
            filteredVariance = min(filteredVariance, varianceSample);
        }
    }
	
	variance = vec2(filteredDepth,filteredVariance);
	
}


