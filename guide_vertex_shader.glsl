#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aTransparency;
layout (location = 3) in float aSegmentIndex;
layout (location = 4) in float aThickness;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 camera_position;
uniform vec3 camera_front;

out vec3 color;
out float transparency;


out VS_OUT{


	float thickness;
	vec3 eye_vector;
	vec4 point;
	
}vs_out;

void main()
{
     gl_Position =  view* model * vec4(aPos, 1.0);

	color = aColor;
	
	//vec4 cam =  view * vec4(camera_position, 1.0);
	//vec4 view_cam_pos =  projection*view * vec4(normalize(camera_position), 1.0);
	//vec3 cam =  camera_front;
	//vs_out.eye_vector = normalize(cam.xyz - gl_Position.xyz);

	//vs_out.eye_vector = vec3(0.0 , 0.0 , -1.0);
	
	vs_out.thickness = aThickness;
	vs_out.point = gl_Position;

	transparency = aTransparency;
	
}

