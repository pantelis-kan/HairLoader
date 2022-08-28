#version 430


layout( isolines, equal_spacing) in;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform mat4 bspline_mat;

in CS_OUT{

	float thickness;
	vec4 point;
	//vec4 tangent;
	vec3 point_color;
	float transparency;
	
}es_in[];



out ES_OUT{

	float thickness;
	vec4 point;
	vec3 point_color;
	float transparency;
	
} es_out;


void main( )
{
	vec4 p0 = es_in[0].point;
	vec4 p1 = es_in[1].point;
	vec4 p2 = es_in[2].point;
	vec4 p3 = es_in[3].point;
	
	float u = clamp(gl_TessCoord.x, 0.0, 1.0);
	
	// B-Spline basis functions. I can do the same with matrix calculations instead
	float b0 = (-1.0*u*u*u + 3.0*u*u - 3.0*u + 1.0) * (1.0/6.0);
	float b1 = (3.0*u*u*u - 6.0*u*u + 4.0 ) * (1.0/6.0);
	float b2 = (-3.0*u*u*u + 3.0*u*u + 3.0*u + 1.0) * (1.0/6.0);
	float b3 = (u*u*u) * (1.0/6.0);
	
	//mat4 b = bspline_mat * mat4(p0 , p1 , p2 , p3);
	
	es_out.point =  view * model * (b0*p0 + b1*p1 + b2*p2 + b3*p3);
	es_out.thickness = es_in[0].thickness;
	es_out.point_color = es_in[0].point_color;
	es_out.transparency = es_in[0].transparency;

}