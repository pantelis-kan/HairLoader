#version 430

in vec3 fragment_color;
in float Transparency;
in vec3 Tangent;
in vec3 Normal;
in vec4 posWorld;
in vec4 posWorld2;
in vec2 texCoord;
in vec4 viewSpace;

uniform vec3 lightPos;
uniform vec3 kd;
uniform vec3 lightColor;
uniform vec3 camera_position;
uniform float specular_strength;
uniform mat4 lightSpaceMatrix;

uniform sampler2D specularTexture;
uniform sampler2D hairColorTexture;
uniform sampler2D noiseTexture;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
 
uniform sampler2D shadowMap;
 uniform sampler2D varianceMap;


uniform float nearPlane;
uniform	float farPlane;

in vec4 shadowCoord;

float alpha = 0.97;

int PCFKernel = 1;

vec3 specularColor = vec3(0.5,0.5,0.5);
vec3 baseColor = vec3(0.9,0.8,0.47);


out vec4 FragColor;

float sqr(float x) {
  return x * x;
}

float mydot(vec3 u, vec3 v){
	return max(dot(u,v),0.0);
}
float linearizeDepth(float depth){


	float linearDepth = (2.0*nearPlane ) / (farPlane + nearPlane - depth*(farPlane -nearPlane)) ;
	
	return linearDepth;

}


float rand( vec2 p )
{
    return fract(sin(dot(p,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 shiftedTangent(vec3 T, vec3 N, float shift){

	vec3 shiftedT = T + shift * N;
	return normalize(shiftedT);
}


/*
float ShadowCalculation()
{

    // perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(varianceMap, projCoords.xy).r; 
    //float closestDepth = textureLod(shadowMap, projCoords.xy,0.5).r; 
    
	// get depth of current fragment from light's perspective
    float currentDepth;
	currentDepth = projCoords.z;
	//currentDepth = linearizeDepth(currentDepth);
    
	// calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - posWorld.xyz);
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float bias = 0.05;
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
	// PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(varianceMap, 0);
    for(int x = -PCFKernel; x <= PCFKernel; ++x)
    {
        for(int y = -PCFKernel; y <= PCFKernel; ++y)
        {
            float pcfDepth = texture(varianceMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            
			//float pcfDepth = linearizeDepth(texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r); 
            shadow += currentDepth - bias > pcfDepth  ? 0.7 : 0.0;        
        }    
    }
    shadow /= float(pow((PCFKernel * 2) + 1 , 2));
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
	
        
    return shadow;
}
*/


float PCFShadowCalculation()
{

    // perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(varianceMap, projCoords.xy).r; 
    //float closestDepth = textureLod(shadowMap, projCoords.xy,0.5).r; 
    
	// get depth of current fragment from light's perspective
    float currentDepth;
	currentDepth = projCoords.z;

    
	// calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - posWorld.xyz);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	//float bias = 0.05;
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
	// PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(varianceMap, 0);
    for(int x = -PCFKernel; x <= PCFKernel; ++x)
    {
        for(int y = -PCFKernel; y <= PCFKernel; ++y)
        {
            float pcfDepth = texture(varianceMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += pcfDepth;        
        }    
    }
    shadow /= float(pow((PCFKernel * 2) + 1 , 2));
	
	    // Compare the depth of the current fragment with the depth stored in the shadow map
    float visibility = 1.0;
    
    if (currentDepth > shadow + bias) {
        float shadowDist = currentDepth - shadow;
        float maxShadowDist = 0.1;
        visibility = 1.0 - smoothstep(0.0, maxShadowDist, shadowDist);
    }
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    /*if(projCoords.z > 1.0)
        shadow = 0.0;*/
	
        
    return visibility;
}




float ChebyshevUpperBound(vec2 Moments, float t) {   
	// One-tailed inequality valid if t > Moments.x    
	float p = (t <= Moments.x) ? 1.0f : 0.0f;   
	// Compute variance.    
	float Variance = Moments.y - (Moments.x * Moments.x);
	
	float g_MinVariance	= 0.002;

	Variance = max(Variance, g_MinVariance);  
	 // Compute probabilistic upper bound.    
	float d = t - Moments.x;
	 float p_max = Variance / (Variance + d*d);   
	 return max(p, p_max); 
 } 
 
 float VSMShadowContribution() {   
     // perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	
	 // Read the moments from the variance shadow map.    
	 vec2 Moments = texture(varianceMap, projCoords.xy).xy;   
	 // Compute the Chebyshev upper bound.    
	 return ChebyshevUpperBound(Moments, projCoords.z); 
 } 




vec3 KajiyaKay() {

    vec3 L = normalize(lightPos - posWorld.xyz);
	vec3 E = normalize(camera_position - posWorld.xyz);
    vec3 H = normalize(L + E);
	
    float NdotL = max(dot(normalize(Normal), L), 0.0);
    float NdotH = max(dot(normalize(Normal), H), 0.0);
	
    float specular = pow(NdotH, 5.0) ;
    vec3 hairColor = texture(hairColorTexture,(texCoord)).rgb;
    vec3 diffuse =  hairColor ;
    vec3 ambient = vec3(0.08) * hairColor ;
	
	float shadow;
	shadow = 1.0 - PCFShadowCalculation();  
	//shadow = 1.0 - VSMShadowContribution();  
	
    vec3 finalColor = ambient + shadow * (diffuse + specular);
	return finalColor;
}




vec3 Marschner(){

	vec3 N = shiftedTangent(Tangent,Normal,rand(posWorld.xy));
	
	vec3 lightDir = normalize(lightPos - posWorld.xyz);
	vec3 viewDir = normalize(camera_position - posWorld.xyz);

    vec3 V = normalize(viewDir);
    vec3 L = normalize(lightDir);

    float NdotV = max(dot(N, V),0.0) ;
    float NdotL = max(dot(N, L),0.0);
	
	vec3 hairColor = texture(hairColorTexture,(texCoord)).rgb;
	//vec3 hairColor = vec3(1,0.85,0.32);

    vec3 diffuse = hairColor * NdotL ;

	float primaryHighlightIntensity = 0.2;
	float secondaryHighlightIntensity = 0.6;

	float theta = asin(NdotL) + asin(NdotV);
	
	vec3 term1 = normalize(L-NdotL*N);
	vec3 term2 = normalize(V-N*NdotV);
	float cosPhi = dot(term1,term2);

	vec3 R_ = vec3(clamp(primaryHighlightIntensity * pow( abs(cos(theta - 4.0)) , 8.0) , 0.0, 1.0))  ;
	
	vec3 TRT = vec3(clamp(secondaryHighlightIntensity * pow( abs(cos(theta - 64.0)), 64.0 ),0.0, 1.0)) ;
	
	float rimIntensity = 0.1;
	vec3 TT = vec3(clamp(rimIntensity * max(0.0, cosPhi) * pow( cos(theta - 2.0) , 2.0 ), 0.0, 1.0)) ;
	vec3 specular =  R_ + TRT + TT;

	float ambientStrength = 0.1;
	vec3 ambient = hairColor * ambientStrength;

	float shadow;
	//shadow = 1.0 - PCFShadowCalculation();  
	shadow = 1.0 - VSMShadowContribution();  


	vec3 color = ambient + shadow * (diffuse + specular);
	return clamp(color * lightColor,0.0,1.0);
	
}

void main()
{
	vec3 diffuse_color = vec3(1,0.85,0.32);
	

	//FragColor = vec4(vec3( 1.0-  PCFShadowCalculation() ),1.0);
	//FragColor = vec4(vec3( 1.0 - VSMShadowContribution() ),1.0);
	
	FragColor = vec4( Marschner() ,alpha);
	//FragColor = vec4( KajiyaKay() ,alpha);


}


