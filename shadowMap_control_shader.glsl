#version 430

layout (vertices = 4) out;

float uOuter0 = 25.0;

// number of spline segments
float uOuter1 = 1.0;



in VS_OUT{
	vec4 point;
	float thickness;
	vec3 normal;
}vs_out[];

out CS_OUT{
	vec4 point;
	float thickness;
	vec3 normal;
} cs_out[];


void main( )
{
		
	cs_out[ gl_InvocationID ].point = vs_out[ gl_InvocationID ].point; 
	cs_out[ gl_InvocationID ].thickness = vs_out[ gl_InvocationID ].thickness; 
	cs_out[ gl_InvocationID ].normal = vs_out[ gl_InvocationID ].normal; 


	
	gl_TessLevelOuter[0] = uOuter0;
	gl_TessLevelOuter[1] = uOuter1;

}



