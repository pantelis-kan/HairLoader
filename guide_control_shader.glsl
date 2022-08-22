#version 430

layout (vertices = 4) out;

float uOuter0 = 1.0;
float uOuter1 = 10.0;

in VS_OUT{
	float thickness;
	vec4 point;
	vec4 tangent;
	vec3 point_color;
	float transparency;
	
}vs_out[];

out CS_OUT{

	float thickness;
	vec4 point;
	vec4 tangent;
	vec3 point_color;
	float transparency;
	
} cs_out[];


void main( )
{
	//gl_out[ gl_InvocationID ].gl_Position = gl_in[ gl_InvocationID ].gl_Position;
	cs_out[ gl_InvocationID ].point = vs_out[ gl_InvocationID ].point; 
	//cs_out[ gl_InvocationID ].tangent = vs_out[ gl_InvocationID ].tangent;
	cs_out[ gl_InvocationID ].thickness = vs_out[ gl_InvocationID ].thickness;
	cs_out[ gl_InvocationID ].transparency = vs_out[ gl_InvocationID ].transparency;
	cs_out[ gl_InvocationID ].point_color = vs_out[ gl_InvocationID ].point_color;
	
	

}



