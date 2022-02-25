#version 430

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 4) out;
//layout (line_strip, max_vertices = 2) out;

uniform mat4 projection;

uniform vec3 camera_front;
uniform vec3 camera_position;

float invWidth = 1.0 / 800; 
float invHeight= 1.0 / 768;
float ratio = 800 / 768;

in VS_OUT{

	float thickness;
	vec3 eye_vector;
	vec4 point;
	
}gs_in[];


void main(){

	// The adjacent vertices
	vec4 p0 = gs_in[0].point;
	vec4 p3 = gs_in[3].point;

	//float radius1 = gs_in[0].thickness * 0.5;
	//float radius2 = gs_in[0].thickness * 0.5;
	
	float radius1 = 0.05;
	float radius2 = 0.05;
	
	vec4 p1 =  gs_in[1].point;
	vec4 p2 =  gs_in[2].point;

	//vec3 tangent = normalize(p1.xyz - p2.xyz);
	vec3 tangent1 = normalize(p0.xyz - p1.xyz);
	vec3 tangent2 = normalize(p1.xyz - p2.xyz);
	vec3 tangent3 = normalize(p2.xyz - p3.xyz);
	
	
	vec3 eye_vec = vec3(0,0,-1);
	vec3 side_vector1 = normalize(cross(eye_vec,tangent1));
	vec3 side_vector2 = normalize(cross(eye_vec,tangent2));
	vec3 side_vector3 = normalize(cross(eye_vec,tangent3));
	
	vec3 side_vector012 = (side_vector1 + side_vector2) / 2;
	vec3 side_vector123 = (side_vector2 + side_vector3) / 2;

	vec3 Pos;

	Pos = p1.xyz + (side_vector012 * radius1);
	gl_Position = projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p1.xyz - (side_vector012 * radius1);
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz + (side_vector123 * radius2);
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz - (side_vector123 * radius2);
	gl_Position = projection * vec4(Pos, 1.0);
	EmitVertex();

	EndPrimitive();
	
}



