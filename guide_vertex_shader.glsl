#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aTransparency;
layout (location = 3) in float aSegmentIndex;
layout (location = 4) in float aThickness;
layout (location = 5) in vec3 aTangent;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 camera_position;
uniform vec3 camera_front;


out VS_OUT{


	float thickness;
	vec4 point;
	vec4 tangent;
	vec3 point_color;
	float transparency;
	
}vs_out;

void main()
{
     //gl_Position =  view* model * vec4(aPos, 1.0);
	 gl_Position = view*model*vec4(aPos, 1.0);

	vs_out.point_color = aColor;
	vs_out.transparency = aTransparency;

	vs_out.thickness = aThickness;
	vs_out.point = gl_Position;
	//vs_out.tangent = vec4(aTangent, 1.0);
	vs_out.tangent = view*model*vec4(aTangent, 1.0);

	
}

