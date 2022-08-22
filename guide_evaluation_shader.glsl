#version 430


layout( isolines, equal_spacing) in;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

in CS_OUT{

	float thickness;
	vec4 point;
	vec4 tangent;
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
	

	/*
	vec4 t0 = vec4(0.117762, -0.0235176, -8.14166,1.0);
	vec4 t1 = vec4(0.0781674, -0.0412169, -4.05511,1.0);
*/
	
	float u = gl_TessCoord.x;
	
	
	// Bezier basis functions:
	
	float b0 = (1.-u) * (1.-u) * (1.-u);
	float b1 = 3. * u * (1.-u) * (1.-u);
	float b2 = 3. * u * u * (1.-u);
	float b3 = u * u * u;
	
	
	/*
	// 	B-spline basis functions?	
	float b0 = 2.*u*u*u - 3.*u*u + 1.;
	float b1 = -2.*u*u*u + 3.*u*u;
	float b2 = u*u*u - 2.*u*u + u;
	float b3 = u*u*u - u*u;
*/
	
	es_out.point = (b0*p0 + b1*p1 + b2*p2 + b3*p3);
	//es_out.point = view*(b0*p0 + b1*p1 + b2*t0 + b3*t1);
	es_out.thickness = es_in[0].thickness;
	es_out.point_color = es_in[0].point_color;
	es_out.transparency = es_in[0].transparency;

}