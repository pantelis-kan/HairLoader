#version 430

layout (vertices = 4) out;

float uOuter0 = 20.0;

// number of spline segments
float uOuter1 = 3.0;

in VS_OUT{

	float thickness;
	vec3 point;
	vec3 point_color;
	float transparency;
	vec3 neighbor1;
	vec3 neighbor2;
	vec3 neighbor3;
	vec3 barycentrics;
	vec3 normal;
	vec4 shadowCoord;

}vs_out[];

out CS_OUT{

	float thickness;
	vec3 point;
	vec3 point_color;
	float transparency;
	vec3 neighbor1;
	vec3 neighbor2;
	vec3 neighbor3;
	vec3 barycentrics;
	vec3 normal;
	vec4 shadowCoord;
	
} cs_out[];


void main( )
{
		
	cs_out[ gl_InvocationID ].point = vs_out[ gl_InvocationID ].point; 
	cs_out[ gl_InvocationID ].thickness = vs_out[ gl_InvocationID ].thickness;
	cs_out[ gl_InvocationID ].transparency = vs_out[ gl_InvocationID ].transparency;
	cs_out[ gl_InvocationID ].point_color = vs_out[ gl_InvocationID ].point_color;
	cs_out[ gl_InvocationID ].neighbor1 = vs_out[ gl_InvocationID ].neighbor1;
	cs_out[ gl_InvocationID ].neighbor2 = vs_out[ gl_InvocationID ].neighbor2;
	cs_out[ gl_InvocationID ].neighbor3 = vs_out[ gl_InvocationID ].neighbor3;
	cs_out[ gl_InvocationID ].barycentrics = vs_out[ gl_InvocationID ].barycentrics;
	cs_out[ gl_InvocationID ].normal = vs_out[ gl_InvocationID ].normal;
	cs_out[ gl_InvocationID ].shadowCoord = vs_out[ gl_InvocationID ].shadowCoord;

	
	gl_TessLevelOuter[0] = uOuter0;
	gl_TessLevelOuter[1] = uOuter1;

}



