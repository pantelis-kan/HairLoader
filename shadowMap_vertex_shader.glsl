#version 430

layout (location = 0) in vec3 aPos;
layout (location = 4) in float aThickness;
layout (location = 10) in vec3 aNormal;

uniform mat4 lightSpaceMatrix;
uniform mat4 light_model;
uniform mat4 light_view;
uniform mat4 light_projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out VS_OUT{

	vec4 point;
	float thickness;
	vec3 normal;
	
}vs_out;


void main()
{

	vs_out.point =   vec4(aPos,1.0);
	
	vs_out.thickness = aThickness;
	vs_out.normal = aNormal;
	//gl_Position = lightSpaceMatrix * gl_Position;

	
}

