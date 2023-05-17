#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aTransparency;
layout (location = 3) in float aSegmentIndex;
layout (location = 4) in float aThickness;
layout (location = 7) in vec3 aBarycentrics;
/*
layout (location = 8) in vec3 aNormal1;
layout (location = 9) in vec3 aNormal2;
*/
layout (location = 10) in vec3 aNormal;

uniform mat4 lightSpaceMatrix;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;



out VS_OUT{

	float thickness;
	vec3 point;
	vec3 point_color;
	float transparency;
	vec3 neighbor1;
	vec3 neighbor2;
	vec3 neighbor3;
	vec3 barycentrics;
	vec3 normal;
	vec4 shadowCoord;
	
}vs_out;

void main()
{
	vs_out.point_color = aColor;
	vs_out.transparency = aTransparency;
	vs_out.thickness = aThickness;
	vs_out.point = aPos;

	vs_out.barycentrics = aBarycentrics;
	/*
	vs_out.normal1 = aNormal1;
	vs_out.normal2 = aNormal2;
	*/
	vs_out.normal = aNormal;
	
	vec4 clipPosition = projection * view * model * vec4(aPos,1.0);
	vec4 worldPosition = model * vec4(aPos,1.0);
	vec4 texCoords = lightSpaceMatrix * worldPosition;
	vec2 ndcCoords = texCoords.xy / texCoords.w;
	vec2 texCoords1 = 0.5 * ndcCoords + vec2(0.5);
	vs_out.shadowCoord = vec4(texCoords1, clipPosition.z, clipPosition.w);

	
}

