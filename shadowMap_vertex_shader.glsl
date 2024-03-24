#version 430

layout (location = 0) in vec3 aPos;
layout (location = 4) in float aThickness;
layout (location = 7) in vec3 aBarycentrics;
layout (location = 10) in vec3 aNormal;
layout (location = 11) in vec3 aNormal_Neighb1;
layout (location = 12) in vec3 aNormal_Neighb2;
layout (location = 13) in vec3 aNormal_Neighb3;
layout (location = 14) in vec3 aGuide_Neighb2;
layout (location = 15) in vec3 aGuide_Neighb3;

uniform mat4 lightSpaceMatrix;
uniform mat4 light_model;
uniform mat4 light_view;
uniform mat4 light_projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out VS_OUT{

	vec3 point;
	float thickness;
	vec3 normal;
	vec3 barycentrics;
	
	vec3 normal_neighb1;
	vec3 normal_neighb2;
	vec3 normal_neighb3;
	
	vec3 guide_neighb2;
	vec3 guide_neighb3;
	
}vs_out;


void main()
{

	vs_out.point =   aPos;
	
	vs_out.thickness = aThickness;
	vs_out.barycentrics = aBarycentrics;
	vs_out.normal = aNormal;
	
	vs_out.normal_neighb1 = aNormal_Neighb1;
	vs_out.normal_neighb2 = aNormal_Neighb2;
	vs_out.normal_neighb3 = aNormal_Neighb3;
	vs_out.guide_neighb2 = aGuide_Neighb2;
	vs_out.guide_neighb3 = aGuide_Neighb3;
	//gl_Position = lightSpaceMatrix * model* vec4(aPos,1.0);

	
}

