#version 430

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform vec3 bboxMin;
uniform vec3 bboxMax;
uniform mat4 lightSpaceMatrix;

out vec3 fragment_color;
out float Transparency;
out vec3 Tangent;
out vec3 Normal;
out vec4 posWorld;
out vec4 posWorld2;
out vec4 viewSpace;
out vec2 texCoord;
out vec4 shadowCoord;

in ES_OUT{

	float thickness;
	vec4 point;
	vec4 pointWorld;
	vec4 pointWorld2;
	vec3 point_color;
	float transparency;
	vec3 Tangent;
	vec3 Normal;
	vec4 shadowCoord;
	
}gs_in[];


void main(){

	//Tangent = gs_in[1].Tangent;
	vec4 worldSpacePos1 = gs_in[0].pointWorld;
	vec4 worldSpacePos2 = gs_in[1].pointWorld;
	vec4 worldSpacePos1_2 = gs_in[0].pointWorld2;
	vec4 worldSpacePos2_2 = gs_in[1].pointWorld2;
	fragment_color = gs_in[0].point_color;
	Transparency = gs_in[0].transparency;
	//Tangent =  gs_in[0].Tangent;
	Normal = gs_in[1].Normal;
	
	vec4 p1 = gs_in[0].point;
	vec4 p2 = gs_in[1].point;
	
	vec2 t0 = vec2(0.0, 0.0);
    vec2 t1 = vec2(1.0, 0.0);

   // vec4 p00 = projection * view * model * p1;
   // vec4 p11 = projection * view * model * p2;
    // t0 = vec2((p00.x - bboxMin.x) / (bboxMax.x - bboxMin.x), (p00.y - bboxMin.y) / (bboxMax.y - bboxMin.y));
    // t1 = vec2((p11.x - bboxMin.x) / (bboxMax.x - bboxMin.x), (p11.y - bboxMin.y) / (bboxMax.y - bboxMin.y));

	
	vec3 tangent = p2.xyz - p1.xyz;
	Tangent = (model*vec4(tangent,1.0)).xyz;
	if(gs_in[0].thickness == 0 || gs_in[1].thickness == 0) Tangent = worldSpacePos2.xyz;
	tangent = normalize(tangent);
    
	
	
   // Calculate the curvature vector for the current segment
    vec3 curvature = normalize(cross(tangent, vec3(0.0, 1.0, 0.0)));
    
	// Calculate the view direction for the current vertex
    vec3 viewDir = normalize((view * vec4(p1.xyz, 1.0)).xyz - gl_Position.xyz);
    
	vec3 eyeVec = p1.xyz;
	vec3 side_vector = normalize(cross(eyeVec, tangent));

	
	float radius1 = gs_in[0].thickness * 0.05;
	float radius2 = gs_in[0].thickness * 0.05;
	
	float length = length(p2.xyz - p1.xyz);
    texCoord = vec2(0.0, 0.0);
    texCoord.y = length;
	
	vec3 Pos;

	Pos = p1.xyz + ((side_vector + tangent) * radius1);
	posWorld = worldSpacePos1;
	posWorld2 = model * worldSpacePos2_2;
	//shadowCoord = lightSpaceMatrix * model * vec4(Pos,1.0);
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	texCoord.x = 0.0;
	gl_Position =  projection *  vec4(Pos, 1.0);
	viewSpace = view * posWorld;
	EmitVertex();

	Pos = p1.xyz - ((side_vector - tangent) * radius1);
	posWorld = worldSpacePos1;
	posWorld2 = model * worldSpacePos2_2;
	//shadowCoord = lightSpaceMatrix * model * vec4(Pos,1.0);
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	viewSpace = view * posWorld;
	texCoord.x = 1.0;
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz + ((side_vector + tangent) * radius2);
	posWorld = worldSpacePos1;
	posWorld2 = model * worldSpacePos2_2;
	//shadowCoord = lightSpaceMatrix * model * vec4(Pos,1.0);
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	viewSpace = view * posWorld;
	texCoord.x = 0.0;
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz - ((side_vector - tangent) * radius2);
	posWorld = worldSpacePos1;
	posWorld2 = model * worldSpacePos2_2;
	//shadowCoord = lightSpaceMatrix * model * vec4(Pos,1.0);
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	viewSpace = view * posWorld;
	texCoord.x = 1.0;
	gl_Position = projection * vec4(Pos, 1.0);
	EmitVertex();


	EndPrimitive();
	
}



