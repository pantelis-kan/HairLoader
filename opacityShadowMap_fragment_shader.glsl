#version 430

layout(location = 0) out vec4 opacity;
in vec4 posWorld;

//layout(location = 1) out vec4 varianceData;

uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;
uniform vec4 LayerSize;
uniform float Opacity;
uniform float nearPlane;
uniform float farPlane;


float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

vec2 VarianceCalculation(){
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
	
	vec2 variance = vec2(filteredDepth,filteredVariance);
	return variance;

}


void main()
{
	// perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
	//vec4 sCoord =  gl_FragCoord;
    vec3 shadowCoord = sCoord.xyz / sCoord.w;
	shadowCoord = shadowCoord * 0.5 + 0.5;


	// Sample the shadow map texture using the current shadow coordinate
    float depth = texture(shadowMap, shadowCoord.xy).r;

	vec4 myLayerSize = LayerSize;
	//vec4 myLayerSize = vec4(2,2.5,3,3.5);
	//myLayerSize = vec4(abs(depth - shadowCoord.z) / 3);
	//vec4 myLayerSize = vec4(0.05 , 0.04 , 0.04 , 0.03);
	myLayerSize /= (sCoord.w*sCoord.w);

	vec4 zi;
	zi.x = depth + myLayerSize.x;
	zi.y = zi.x + myLayerSize.y;
	zi.z = zi.y + myLayerSize.z;
	zi.w = zi.z + myLayerSize.w;

	vec4 Color = vec4(0.0);

	if(shadowCoord.z < zi.x)
	{
		Color.r = Opacity;
		Color.g = Opacity;
		Color.b = Opacity;
		Color.a = Opacity;
	}
	else if(shadowCoord.z < zi.y)
	{
		Color.g = Opacity;
		Color.b = Opacity;
		Color.a = Opacity;
	}
	else if(shadowCoord.z < zi.z)
	{
		Color.b = Opacity;
		Color.a = Opacity;
	}
	else
	{
		Color.a = Opacity;
	}


	//opacity = vec4(gl_FragCoord.z,0.0,0.0,1.0);
	//opacity = vec4(sCoord.w);
	opacity = vec4(Color);

	//vec2 variance = VarianceCalculation();	    
	//vec2 variance = vec2(depth, depth * depth);
	//vec2 variance = ComputeMoments(depth);
	
	//varianceData = vec4(variance.x,variance.y,0.0,1.0);
}


