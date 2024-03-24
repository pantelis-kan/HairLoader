#version 430

layout (location = 0) in vec3 aPos;
layout (location = 3) in float aSegmentIndex;
layout (location = 4) in float aThickness;
layout (location = 7) in vec3 aBarycentrics;
layout (location = 10) in vec3 aNormal;
layout (location = 11) in vec3 aNormal_Neighb1;
layout (location = 12) in vec3 aNormal_Neighb2;
layout (location = 13) in vec3 aNormal_Neighb3;
layout (location = 14) in vec3 aGuide_Neighb2;
layout (location = 15) in vec3 aGuide_Neighb3;

uniform mat4 lightSpaceMatrix;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;



out VS_OUT{

	float thickness;
	vec3 point;
	vec3 barycentrics;
	vec3 normal;
	vec4 shadowCoord;
	
	vec3 normal_neighb1;
	vec3 normal_neighb2;
	vec3 normal_neighb3;
	
	vec3 guide_neighb2;
	vec3 guide_neighb3;
	
}vs_out;

void main()
{

	vs_out.thickness = aThickness;
	vs_out.point = aPos;

	vs_out.barycentrics = aBarycentrics;
	vs_out.normal = aNormal;
	
	vs_out.normal_neighb1 = aNormal_Neighb1;
	vs_out.normal_neighb2 = aNormal_Neighb2;
	vs_out.normal_neighb3 = aNormal_Neighb3;
	vs_out.guide_neighb2 = aGuide_Neighb2;
	vs_out.guide_neighb3 = aGuide_Neighb3;

	
	vec4 clipPosition = projection * view * model * vec4(aPos,1.0);
	vec4 worldPosition = model * vec4(aPos,1.0);
	vec4 texCoords = lightSpaceMatrix * worldPosition;
	vec2 ndcCoords = texCoords.xy / texCoords.w;
	vec2 texCoords1 = 0.5 * ndcCoords + vec2(0.5);
	vs_out.shadowCoord = vec4(texCoords1, clipPosition.z, clipPosition.w);

	
}

