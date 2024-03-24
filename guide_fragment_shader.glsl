#version 430

in vec3 Tangent;
in vec3 Normal;
in vec4 posWorld;
in vec4 posWorld2;
in vec2 texCoord;
in vec4 viewSpace;

in vec3 Barycentrics;

in vec3 Eye;
in vec3 lightDir;

in vec3 HairBarycentric;

uniform vec3 lightPos;
uniform vec3 kd;
uniform vec3 lightColor;
uniform vec3 camera_position;
uniform vec4 camera_position_view;
uniform float specular_strength;
uniform mat4 lightSpaceMatrix;

uniform sampler2D specularTexture;
uniform sampler2D hairColorTexture;
uniform sampler2D noiseTexture;
uniform sampler2D additionalTexture;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 light_model;
 
uniform sampler2D shadowMap;
uniform sampler2D opacityMap;
 uniform sampler2D varianceMap;
 uniform sampler2D Marschner_M;
 uniform sampler2D Marschner_N;
 
 uniform float shadowBias;
uniform float minBias;

 uniform vec4 LayerSize;
 
#define M_PI           3.14159265358979323846  /* pi */
uniform float nearPlane;
uniform	float farPlane;

in vec4 shadowCoord;

float alpha = 0.99;

int PCFKernel = 1;

vec3 specularColor = vec3(0.5,0.5,0.5);
vec3 baseColor = vec3(0.9,0.8,0.47);

vec3 lightDirection = vec3(0.0,1.0,0.0);

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

float ChebyshevUpperBound3(vec2 Moments, float t){
	float p = (t <= Moments.x)? 1.0 : 0.0;
	float Variance = Moments.y - (Moments.x*Moments.x);
	float g_MinVariance = 0.0007;
	Variance = max(Variance,g_MinVariance);
	float d = t - Moments.x;
	float p_max = Variance / (Variance + d*d);
	return max(p,p_max);
}


float ChebyshevUpperBound2(vec2 moments, float depth, float minVariance) {
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, minVariance); // Avoid division by zero or negative variance
    
    float d = depth - moments.x; // Depth difference
    float pMax = variance / (variance + d * d); // Chebyshev's inequality
    
    return max(pMax, 0.0); // Shadow factor
}

float SimpleShadowCalculation()
{
    // perform perspective divide
	vec4 sCoord = lightSpaceMatrix * (posWorld);
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords.xyz = projCoords.xyz * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    //float currentDepth = (projCoords.z - nearPlane) / (farPlane + abs(nearPlane));;
    // check whether current frag pos is in shadow
	    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-lightPos);
    float bias = max(shadowBias * (1.0 - dot(normal, -lightDir)), minBias);
    float shadow = currentDepth > closestDepth  ?  1.0 : 0.0;

    return 1.0 - shadow;
}

float FakeOpacityShadow()
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
	
	const float d = 0.05;

	float layer1  = 0.f;
	float layer2  = 0.f;
	float layer3  = 0.f;

	vec4 OpacityValue = texture(opacityMap, projCoords.xy);
	
	float opacity = 0.0;
	if (currentDepth >= closestDepth && currentDepth < closestDepth + d) {
		opacity = OpacityValue.r;
	} else if (currentDepth >= closestDepth + d && currentDepth < closestDepth + 2.f * d) {
		opacity = OpacityValue.g;
	} else if (currentDepth >= closestDepth + 2.f * d) { 
		opacity = OpacityValue.b;
	}


	return opacity;
	
}


float PCFShadowCalculation()
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
    //float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	//float bias = 0.009;
	//float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    
	// PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -PCFKernel; x <= PCFKernel; ++x)
    {
        for(int y = -PCFKernel; y <= PCFKernel; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;       
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

float opacityShadowCalculation(){
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
	
	vec4 myLayerSize = LayerSize;
	//vec4 myLayerSize = vec4(2,2.5,3,3.5);
	myLayerSize /= (sCoord.w*sCoord.w);

	vec4 zi;
	zi.x = closestDepth + myLayerSize.x;
	zi.y = zi.x + myLayerSize.y;
	zi.z = zi.y + myLayerSize.z;
	zi.w = zi.z + myLayerSize.w;
	
	float Shadow = 0.0;
	vec4 OpacityValue = texture(opacityMap, projCoords.xy);
	
	float weight1 = max(0.0 , 1.0 - ((abs(zi.x - currentDepth))/myLayerSize.x));
	float weight2 = max(0.0 , 1.0 - ((abs(zi.y - currentDepth))/myLayerSize.y));
	float weight3 = max(0.0 , 1.0 - ((abs(zi.z - currentDepth))/myLayerSize.z));
	float weight4 = max(0.0 , 1.0 - ((abs(zi.w - currentDepth))/myLayerSize.w));

	vec4 mixer = vec4(weight1,weight2,weight3,weight4);
	
	if (currentDepth < zi.x)
		Shadow = mix(0.0, OpacityValue.r, mixer.x);
	else if (currentDepth < zi.y)
		Shadow = mix(OpacityValue.r, OpacityValue.g, mixer.y);
	else if (currentDepth < zi.z)
		Shadow = mix(OpacityValue.g, OpacityValue.b, mixer.z);
	else if(currentDepth < zi.w)
		Shadow = mix(OpacityValue.b, OpacityValue.a, mixer.w);
	else
		Shadow = OpacityValue.a;
	
	Shadow = clamp(Shadow,0.0,1.0);
	Shadow = 1.0 - Shadow ;
	return Shadow ;
}


float opacityShadowCalculationPCF(){

	vec4 sCoord = lightSpaceMatrix * (posWorld);
    vec3 projCoords = sCoord.xyz / sCoord.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
   
	// get depth of current fragment from light's perspective
    float currentDepth;
	currentDepth = projCoords.z;
	
	vec4 myLayerSize = LayerSize;

	float Shadow = 0.0;
	
	vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-lightPos);
    float bias = max(shadowBias * (1.0 - dot(normal, lightDir)), minBias);
	
	
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -PCFKernel; x <= PCFKernel; ++x)
    {
        for(int y = -PCFKernel; y <= PCFKernel; ++y)
        {
		
		
            float depthTexture = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			
			//if(currentDepth - bias > depthTexture){
			
				vec4 zi;
				zi.x = depthTexture + myLayerSize.x;
				zi.y = zi.x + myLayerSize.y;
				zi.z = zi.y + myLayerSize.z;
				zi.w = zi.z + myLayerSize.w;
				
				vec4 OpacityValue = texture(opacityMap, projCoords.xy + vec2(x, y) * texelSize);
		
				float weight1 = max(0.0 , 1.0 - ((abs(zi.x - currentDepth))/myLayerSize.x));
				float weight2 = max(0.0 , 1.0 - ((abs(zi.y - currentDepth))/myLayerSize.y));
				float weight3 = max(0.0 , 1.0 - ((abs(zi.z - currentDepth))/myLayerSize.z));
				float weight4 = max(0.0 , 1.0 - ((abs(zi.w - currentDepth))/myLayerSize.w));

				
				vec4 mixer = vec4(weight1,weight2,weight3,weight4);
				float pcfDepth = 0.0;
				
				
				if (currentDepth < zi.x)
					pcfDepth += mix(0.0, OpacityValue.r, mixer.x);
				else if (currentDepth < zi.y)
					pcfDepth += mix(OpacityValue.r, OpacityValue.g, mixer.y);
				else if (currentDepth < zi.z)
					pcfDepth += mix(OpacityValue.g, OpacityValue.b, mixer.z);
				else if(currentDepth < zi.w)
					pcfDepth += mix(OpacityValue.b, OpacityValue.a, mixer.w);
				else
					pcfDepth += OpacityValue.a;
					
				Shadow += currentDepth - bias > depthTexture  ? pcfDepth : 0.0; 
			//}
        }    
    }
    
	Shadow /= float(pow((PCFKernel * 2) + 1 , 2));
	
	Shadow = clamp(Shadow,0.0,1.0);
	Shadow = 1.0 - Shadow ;

	return Shadow ;
}

float ChebyshevUpperBound(vec2 Moments, float t) {   
	// One-tailed inequality valid if t > Moments.x    
	float p = (t <= Moments.x) ? 1.0f : 0.0f;   
	// Compute variance.    
	float Variance = Moments.y - (Moments.x * Moments.x);
	
	float g_MinVariance	= 0.07;

	Variance = max(Variance, g_MinVariance);  
	 // Compute probabilistic upper bound.    
	float d = t - Moments.x;
	 float p_max = Variance / (Variance + d*d);   
	 //return p_max;
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
	 //return ChebyshevUpperBound(Moments, projCoords.z); 
	 //return ChebyshevUpperBound2(Moments, projCoords.z,0.2); 
	 return ChebyshevUpperBound3(Moments, projCoords.z); 
 } 



vec3 KajiyaKay() {
	
	
	vec3 L = normalize(lightPos );
	//vec3 T = normalize(Normal);
	vec3 T = shiftedTangent(normalize(Normal), normalize(Normal),  20*Barycentrics.x);
	vec3 E = normalize(Eye);

	float color_variation_multiplier = 0.2;
	vec3 color_variation = vec3(color_variation_multiplier*Barycentrics.x);
	
	if(Barycentrics.x > Barycentrics.y){
		color_variation = vec3(-color_variation_multiplier*Barycentrics.x);
	}
	
	vec3 hairColor = texture(hairColorTexture,(texCoord)).rgb + color_variation;

	float cosTL = dot(T,L);
	float cosTE = abs(dot(T,E));

	float sinTL = sin(acos(cosTL));
	float sinTE = sin(acos(cosTE));

	float Kd = 0.7;
	//float NdotL = max(dot(T, L),0.0);
    vec3 diffuse = hairColor * cosTL * Kd;
	//vec3 diffuse = clamp( mix(0.25,1.0,dot(T,L)) ,0.0,1.0) * hairColor * Kd;
	
	float Ks = 1.0 - Kd;
	int Glossiness = 256;
	float specularIntensity = 1.4;
	
	// original
	vec3 specular = Ks * pow(cosTL*cosTE + sinTL*sinTE, Glossiness) * vec3(specularIntensity);


	vec3 ambient = vec3(0.2) * hairColor ;

	float shadow = 1.0;
	//shadow = PCFShadowCalculation();
	shadow = 0.7 * opacityShadowCalculationPCF();
	vec3 finalColor = ambient + shadow * (diffuse + specular);
	//vec3 finalColor = diffuse;
	return finalColor;

}

float computeAO2() {
    float ao = 0.0;
    int numSamples = 4; // Number of samples for AO calculation
    float radius = 0.01; // Sampling radius, adjust based on your needs
    float totalOpacity = 0.0;
	
	vec2 resolution = vec2 (1024.0 , 1024.0);
	
		// perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    for (int i = 0; i < numSamples; ++i) {
        // Offset coordinates for sampling
        float angle = float(i) * 6.28318530718 / float(numSamples);
        vec2 sampleOffset = radius * vec2(cos(angle), sin(angle));
        
        // Sample the opacity map at this offset
        vec3 opacityLayers = texture(opacityMap, projCoords.xy + sampleOffset / resolution).rgb;
        
        // Aggregate opacity from all layers
        float layerOpacity = (opacityLayers.r + opacityLayers.g + opacityLayers.b) / 3.0;
        
        // Accumulate total opacity (simple method, consider weighting based on distance if desired)
        totalOpacity += layerOpacity;
    }
    
    // Normalize and invert the accumulated opacity to get AO factor (1.0 is fully lit, 0.0 is fully occluded)
    ao = 1.0 - (totalOpacity / float(numSamples));
    
    return ao;
}

float computeAO() {
    float ao = 1.0; // Start fully unoccluded
	
		
	// perform perspective divide
	vec4 sCoord = lightSpaceMatrix * posWorld;
    vec3 projCoords = sCoord.xyz / sCoord.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	
    vec3 opacityLayers = texture(opacityMap, projCoords.xy).rgb; // Sample the opacity map
    
    // Weighting for each layer's contribution to AO. Adjust based on your layering strategy.
    float depthWeights[3] = float[](0.03, 0.13, 0.24); // Example weights for simplicity
    
    // Compute AO based on the accumulated opacity from each layer
    for (int i = 0; i < 3; i++) {
        // Use the opacity value directly or adjust the calculation based on your needs
        ao -= (1.0 - opacityLayers[i]) * depthWeights[i];
    }
    
    ao = max(ao, 0.0); // Ensure AO doesn't go negative
    
    return ao; // Returns AO factor (0: fully occluded, 1: fully lit)
}


 
	
vec3 Marschner2(){

	vec3 L = normalize(lightPos );
	//vec3 L = normalize(lightDir );

	float color_variation_multiplier = 3.2;
	vec3 color_variation = vec3(color_variation_multiplier * (Barycentrics.y+0.1) );
	
	/*
	if(Barycentrics.x > 0.3)
		color_variation = vec3(-0.1*Barycentrics.x);
	*/
	
	vec3 T = normalize(Tangent);
	//vec3 T = shiftedTangent(normalize(Tangent), normalize(Tangent),  HairBarycentric.x);
	vec3 E = normalize(Eye) ;
	
	vec3 hairTexture = texture(hairColorTexture,(texCoord)).rgb;
	//vec3 hairColor = texture(hairColorTexture,(texCoord)).rgb + color_variation;
	vec3 hairColor = hairTexture + color_variation*hairTexture;

	//hairColor = vec3(1,0.85,0.32);

	float Kd = 0.7;
	//vec3 diffuse = clamp( mix(0.25,1.0,mydot(normalize(Normal),-L)) ,0.0,1.0) * lightColor * Kd * hairColor;
	//vec3 lightDirTangentSpace = normalize(lightPos - dot(lightPos,normalize(Normal)) * normalize(Normal));
	//float diffuseTerm = max(dot(lightDirTangentSpace, T), 0.0);
	
	vec3 diffuse =  lightColor * Kd * hairColor ;
	
	float Ks = 1.0 - Kd;
	
	vec2 Lookup1Coords;
	vec2 Lookup2Coords;
	
	float SinThetaI = dot(L, T); 
	float SinThetaO = (dot(T, E));  
		
	Lookup1Coords = 0.5*vec2(SinThetaI,SinThetaO) + 0.5;
	 
	vec3 lightPerp = L - SinThetaI * T;  
	vec3 eyePerp = E - SinThetaO * T;  
	
	//float CosPhiD = dot(eyePerp, lightPerp) * pow(dot(eyePerp, eyePerp) *  dot(lightPerp, lightPerp), -0.5);
	float CosPhiD = dot(eyePerp, lightPerp) * inversesqrt(dot(eyePerp, eyePerp) * dot(lightPerp, lightPerp));
	 //CosPhiD = 0.5*CosPhiD + 0.5;

	
	vec3 texture_M = texture(Marschner_M,Lookup1Coords).rgb;
	float MR = texture_M.x;
	float MTT = texture_M.y;
	float MTRT = texture_M.z;
	//float cosThetaD = texture_M.w;
	//float cosThetaD = cos( (asin(2 * SinThetaI - 1 ) - asin(2 * SinThetaO - 1  ) ) / 2 );
	float cosThetaD = ( ((2 * SinThetaI - 1 ) - (2 * SinThetaO - 1  ) ) / 2 );
	//float cosThetaD = cos( (asin(2 * Lookup1Coords.x - 1) - asin(2 * Lookup1Coords.y - 1) ) / 2 );

	Lookup2Coords = 0.5 * vec2(CosPhiD,cosThetaD) + 0.5;
	//Lookup2Coords =  vec2(CosPhiD,cosThetaD) ;
	
	vec3 texture_N = texture(Marschner_N,(Lookup2Coords)).rgb;
	float NR = texture_N.x;
	float NTT = texture_N.y;
	float NTRT = texture_N.z;
	
	
	float primaryIntensity = 0.6;
	float secondaryIntensity = 0.9;
	float transmissionIntensity = 4.5;
	
	vec3 R = MR * NR * primaryIntensity * lightColor;
	vec3 TT = MTT * NTT * transmissionIntensity * lightColor;
	vec3 TRT = MTRT * NTRT * secondaryIntensity * hairColor; 
	vec3 reflection = R + TT + TRT;
	//vec3 reflection = TRT;
	

	vec3 specular = reflection;
	/*if (cosThetaD != 0.0)
		specular = reflection / vec3(cosThetaD * cosThetaD) ;*/
		
	specular = clamp(specular,0.0,1.0);	

	float ambientLightTerm = 0.3;
	vec3 ambientLightColor = vec3(ambientLightTerm);
	vec3 ambient = 0.1 * vec3(lightColor) * hairColor ;

	float shadow = 0.0;
	//shadow = 0.7 * PCFShadowCalculation();
	//shadow = 0.7 * SimpleShadowCalculation();
	shadow = 0.57 * opacityShadowCalculationPCF();
	//shadow = 0.7 * VSMShadowContribution();
	//shadow = 0.7 * opacityShadowCalculation();
	//shadow = 1.0;

	//float ambientOcclussion = computeAO();
	//vec3 AO = vec3(ambientOcclussion);
	
	//ambient = 0.1*AO * vec3(lightColor) * hairColor ;
	vec3 finalColor =  ambient + shadow * (diffuse + specular);
	//vec3 finalColor = vec3(0.1*AO);
	//vec3 finalColor = vec3(shadow);
	//vec3 finalColor = (diffuse + specular);
	//if(shadow < 0.0) finalColor = vec3(1.0,0.0,0.0);
	//if(ambientOcclussion < 0.0) finalColor = vec3(1.0,0.0,0.0);
	return clamp(finalColor,0.0,1.0);

}

void main()
{
	vec3 diffuse_color = vec3(1,0.85,0.32);
	
	vec3 hairTexture = texture(hairColorTexture,(texCoord)).rgb;

	FragColor = vec4( Marschner2() ,alpha);
	//FragColor = vec4( hairTexture ,1.0);
	//FragColor = vec4( KajiyaKay() ,alpha);
	

}


