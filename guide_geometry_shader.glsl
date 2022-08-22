#version 430

//layout (lines_adjacency) in;
layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;
//layout (line_strip, max_vertices = 2) out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 InverseWorldMatrix;

uniform vec3 camera_front;
uniform vec3 camera_position;

float invWidth = 1.0 / 800; 
float invHeight= 1.0 / 768;
float ratio = 800 / 768;

out vec3 fragment_color;
out float Transparency;

in ES_OUT{

	float thickness;
	vec4 point;
	vec3 point_color;
	float transparency;
	
}gs_in[];


void main(){

	vec4 p1 = gs_in[0].point;
	vec4 p2 = gs_in[1].point;

	vec3 tangent = p2.xyz - p1.xyz;
	tangent = normalize(tangent);

	
	//vec3 eyeVec = (InverseWorldMatrix * vec4(camera_position, 1) ).xyz - p1.xyz;
	vec3 eyeVec = p1.xyz;
	vec3 side_vector = normalize(cross(eyeVec, tangent));

	float radius1 = gs_in[0].thickness * 0.1;
	float radius2 = gs_in[0].thickness * 0.1;
	
	vec3 Pos;

	Pos = p1.xyz + (side_vector * radius1);
	gl_Position =  projection *  vec4(Pos, 1.0);
	EmitVertex();

	Pos = p1.xyz - (side_vector * radius1);
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz + (side_vector * radius2);
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz - (side_vector * radius2);
	gl_Position = projection * vec4(Pos, 1.0);
	EmitVertex();
	
	fragment_color = gs_in[0].point_color;
	Transparency = gs_in[0].transparency;

	EndPrimitive();
	
}



