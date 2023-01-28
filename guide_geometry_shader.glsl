#version 430

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projection;

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

	
	vec3 eyeVec = p1.xyz;
	vec3 side_vector = normalize(cross(eyeVec, tangent));

	float radius1 = gs_in[0].thickness * 0.05;
	float radius2 = gs_in[0].thickness * 0.05;
	
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



