#version 430

layout (vertices = 4) out;

float uOuter0 = 30.0;
//float uOuter0 = 20.0;

// number of spline segments
float uOuter1 = 2.0;
//float uOuter1 = 3.0;

uniform mat4 model;

in VS_OUT{

	float thickness;
	vec3 point;
	vec3 barycentrics;
	vec3 normal;
	vec4 shadowCoord;
	
	vec3 normal_neighb1;
	vec3 normal_neighb2;
	vec3 normal_neighb3;
	
	vec3 guide_neighb2;
	vec3 guide_neighb3;

}vs_out[];

out CS_OUT{

	float thickness;
	vec3 point;
	vec3 barycentrics;
	vec3 normal;
	vec4 shadowCoord;
	
	vec3 guide_neighb2;
	vec3 guide_neighb3;
	
	vec3 normal_neighb1;
	vec3 normal_neighb2;
	vec3 normal_neighb3;
	
	vec3 interpolated_normal;
} cs_out[];





void main( )
{

	
	cs_out[ gl_InvocationID ].point = vs_out[ gl_InvocationID ].point; 
	cs_out[ gl_InvocationID ].guide_neighb2 = vs_out[ gl_InvocationID ].guide_neighb2; 
	cs_out[ gl_InvocationID ].guide_neighb3 = vs_out[ gl_InvocationID ].guide_neighb3; 
		
	cs_out[ gl_InvocationID ].thickness = vs_out[ gl_InvocationID ].thickness;
	cs_out[ gl_InvocationID ].barycentrics = vs_out[ gl_InvocationID ].barycentrics;
	cs_out[ gl_InvocationID ].normal = vs_out[ gl_InvocationID ].normal;
	cs_out[ gl_InvocationID ].normal_neighb1 = vs_out[ gl_InvocationID ].normal_neighb1;
	cs_out[ gl_InvocationID ].normal_neighb2 = vs_out[ gl_InvocationID ].normal_neighb2;
	cs_out[ gl_InvocationID ].normal_neighb3 = vs_out[ gl_InvocationID ].normal_neighb3;
	cs_out[ gl_InvocationID ].shadowCoord = vs_out[ gl_InvocationID ].shadowCoord;

	
	gl_TessLevelOuter[0] = uOuter0;
	gl_TessLevelOuter[1] = uOuter1;

}



