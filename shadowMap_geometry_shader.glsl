#version 430

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform mat4 light_model;
uniform mat4 light_view;
uniform mat4 light_projection;

uniform mat4 lightSpaceMatrix;

out vec4 posWorld;

in ES_OUT{
	vec4 point;
	float thickness;
	vec4 pointWorld;
}gs_in[];


void main(){
	
	vec4 p1 = gs_in[0].point;
	vec4 p2 = gs_in[1].point;
	
	vec4 worldSpacePos1 = gs_in[0].pointWorld;
	

	vec3 tangent = p2.xyz - p1.xyz;
	tangent = normalize(tangent);
    
   
	vec3 eyeVec = p1.xyz;
	vec3 side_vector = normalize(cross(eyeVec, tangent));

	
	float radius1 = gs_in[0].thickness * 0.05;
	float radius2 = gs_in[0].thickness * 0.05;
	
	float length = length(p2.xyz - p1.xyz);
	
	vec3 Pos;
	
	Pos = p1.xyz + ((side_vector + tangent) * radius1);
	posWorld = worldSpacePos1;
	gl_Position =  light_projection *  vec4(Pos, 1.0);
	EmitVertex();

	Pos = p1.xyz - ((side_vector - tangent) * radius1);
	posWorld =  worldSpacePos1;
	gl_Position =  light_projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz + ((side_vector + tangent) * radius2);
	posWorld = worldSpacePos1;
	gl_Position = light_projection *  vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz - ((side_vector - tangent) * radius2);
	posWorld =  worldSpacePos1;
	gl_Position = light_projection * vec4(Pos, 1.0);
	EmitVertex();
	
	EndPrimitive();
	
}



