#version 430
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 view;

uniform vec3 lightPos;
uniform vec3 kd;
uniform vec3 lightColor;
uniform sampler2D texture_diffuse1;
uniform vec3 camera_position;
uniform float specular_strength;
uniform mat4 lightSpaceMatrix;
uniform sampler2D shadowMap;
uniform float shadowBias;
uniform float minBias;

int PCFKernel = 1;

float ChebyshevUpperBound2(vec2 moments, float depth, float minVariance) {
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, minVariance); // Avoid division by zero or negative variance
    
    float d = depth - moments.x; // Depth difference
    float pMax = variance / (variance + d * d); // Chebyshev's inequality
    
    return max(pMax, 0.0); // Shadow factor
}


float ChebyshevUpperBound3(vec2 Moments, float t){
	float p = (t <= Moments.x)? 1.0 : 0.0;
	float Variance = Moments.y - (Moments.x*Moments.x);
	float g_MinVariance = 0.005;
	Variance = max(Variance,g_MinVariance);
	float d = t - Moments.x;
	float p_max = Variance / (Variance + d*d);
	return max(p,p_max);
}


 float VSMShadowContribution(vec4 posWorld) {   
     // perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	
	 // Read the moments from the variance shadow map.    
	 //vec2 Moments = texture(varianceMap, projCoords.xy).xy;   
	 // Compute the Chebyshev upper bound.    
	 //return ChebyshevUpperBound(Moments, projCoords.z); 
	 
	 float depth = texture(shadowMap, projCoords.xy).r; 
	 float depthSquared = depth * depth;
	 vec2 Moments = vec2(depth,depthSquared);
	 return ChebyshevUpperBound3(Moments, projCoords.z); 
 } 


float SimpleShadowCalculation(vec4 posWorld)
{
    // perform perspective divide
	vec4 sCoord = lightSpaceMatrix * normalize(posWorld);
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords.xyz = projCoords.xyz * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-lightPos);
	float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), minBias);
    float shadow = currentDepth - bias > closestDepth  ?  1.0 : 0.0;

    return 1.0 - shadow;
}

float PCFShadowCalculation(vec4 posWorld)
{

    // perform perspective divide
	vec4 sCoord = lightSpaceMatrix * (posWorld);
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    //float closestDepth = textureLod(shadowMap, projCoords.xy,0.5).r; 
    
	// get depth of current fragment from light's perspective
    float currentDepth;
	currentDepth = projCoords.z;

    
	// calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-lightPos);
    float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), minBias);
	//float bias = 0.01;
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
	// PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -PCFKernel; x <= PCFKernel; ++x)
    {
        for(int y = -PCFKernel; y <= PCFKernel; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0; ;        
        }    
    }
    shadow /= float(pow((PCFKernel * 2) + 1 , 2));
	
	    // Compare the depth of the current fragment with the depth stored in the shadow map
    float visibility = 1.0;
    
    /*if (currentDepth > shadow + bias) {
        float shadowDist = currentDepth - shadow;
        float maxShadowDist = 0.1;
        visibility = 1.0 - smoothstep(0.0, maxShadowDist, shadowDist);
    }*/
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
	
        
    return 1.0 - shadow;
}


void main(void)
{ 
	float ambientStrength = 0.2;
	vec3 ambient = ambientStrength * lightColor;
	
	// we normalize the vectors, so that the dot product gives the cosine, since their length |u|,|v| = 1 : u*v = |u||v|cos(u,v)
	vec3 norm = normalize(Normal);
	
	// the direction vector between the light source and the fragment's position
	vec3 light_direction = normalize(-lightPos);
	//vec3 light_direction = vec3(0.0,1.0,0.0);
	
	// The greater the angle between both vectors, the darker diffuse component is.
	//If the angle between both vectors is greater than 90 degrees then the result of the dot product will actually become negative 
	// and we end up with a negative diffuse component. For that reason we use the max function that returns the highest of both its parameters 
	// to make sure the diffuse component (and thus the colors) never become negative.
	float diff = max(dot(norm, light_direction), 0.0);
	
	vec3 diffuse = diff * lightColor;

	mat4 invView = inverse(view); // TODO: compute ahead of time?
	vec3 cameraDir = invView[3].xyz;
	vec3 view_direction = normalize(cameraDir - FragPos);
	
	//Note that we negate the lightDir vector. The reflect function expects the first vector to point from the light source towards 
	// the fragment's position, but the lightDir vector is currently pointing the other way around: from the fragment towards the light source 
	// (this depends on the order of subtraction earlier on when we calculated the lightDir vector)
	vec3 reflection = reflect(-light_direction,norm);
	
	//This shininess value is the shininess value of the highlight. The higher the shininess value of an object,
	// the more it properly reflects the light instead of scattering it all around and thus the smaller the highlight becomes. 
	int shininess = 4;
	float spec = pow(max(dot(view_direction, reflection), 0.0), shininess);
	vec3 specular = specular_strength * spec * lightColor;
	
	float light_strength = 1.0;
	//float shadow = SimpleShadowCalculation(vec4(FragPos,1.0));
	//float shadow =  PCFShadowCalculation(vec4(FragPos,1.0));
	float shadow =  VSMShadowContribution(vec4(FragPos,1.0));
	vec3 result = ambient + shadow*(diffuse + specular);
	//vec3 result = vec3(shadow);
	color = texture(texture_diffuse1,TexCoords) * vec4(kd, 1.0) * vec4(result, 1.0) * light_strength;
	//color = vec4(vec3(shadow),1.0);

}


