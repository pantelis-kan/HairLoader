#version 430
#define NUM_CURVE_POINTS 10
layout( isolines, equal_spacing) in;

uniform mat4 view;
uniform mat4 model;

uniform vec3 world_up;

uniform sampler2D noiseTexture;
uniform mat4 lightSpaceMatrix;

uniform mat4 light_model;
uniform mat4 light_view;
uniform mat4 light_projection;

in CS_OUT{
	vec4 point;
	float thickness;
	vec3 normal;
}es_in[];

out ES_OUT{
	vec4 point;
	float thickness;
	vec4 pointWorld;
} es_out;

float rand( vec2 p )
{
    return fract(sin(dot(p,vec2(12.9898,78.233))) * 43758.5453);
}


vec4 B_spline(vec3 p0, vec3 p1, vec3 p2, vec3 p3 , float tessCoordX){

	float u = clamp(tessCoordX, 0.0, 1.0);

	float b0 = (-1.0*u*u*u + 3.0*u*u - 3.0*u + 1.0) * (1.0/6.0);
	float b1 = (3.0*u*u*u - 6.0*u*u + 4.0 ) * (1.0/6.0);
	float b2 = (-3.0*u*u*u + 3.0*u*u + 3.0*u + 1.0) * (1.0/6.0);
	float b3 = (u*u*u) * (1.0/6.0);
	
	return vec4((b0*p0 + b1*p1 + b2*p2 + b3*p3) , 1.0);
}


vec4 Tangent(vec3 p0, vec3 p1, vec3 p2, vec3 p3, float u){

	u = clamp(gl_TessCoord.x, 0.0, 1.0);
	
	float b0 = 0.5*u*u - 1.0*u + 0.5;
	float b1 = -1.0*u*u + 1.0*u + 0.5;
	float b2 = 0.5*u*u;
	
	vec3 T0 = p1 - p0; 
	vec3 T1 = p2 - p1;
	vec3 T2 = p3 - p2;

	return vec4((b0*T0 + b1*T1 + b2*T2), 1.0);
}

void main( )
{

	float v = gl_TessCoord.y;
	
	vec3 p0 = es_in[0].point.xyz;
	vec3 p1 = es_in[1].point.xyz;
	vec3 p2 = es_in[2].point.xyz;
	vec3 p3 = es_in[3].point.xyz;
	

	float lengthToRoot = 2.0;
	float g_clumpWidth = 2.0;

	float u = clamp(gl_TessCoord.x, 0.0, 1.0);
	
	vec4 position = B_spline(p0,p1,p2,p3,gl_TessCoord.x);
	vec4 tangent = Tangent(p0,p1,p2,p3,u);
	
	vec3 coord1 = tangent.xyz;
	vec3 coord2 = normalize(cross(coord1,world_up));
	vec3 coord3 = normalize(cross(coord1,coord2));
	
	vec4 Xaxis = vec4(coord1, 1.0);
	vec4 Zaxis = vec4(coord2, 1.0);
	vec4 Yaxis = vec4(coord3, 1.0);
	
	//float radius = g_clumpWidth * ( g_rootRadius*(1-lengthToRoot) + g_tipRadius*lengthToRoot );
	float radius = 1.0;
	
	float r = 4.0 * sqrt(rand(vec2(v)));
    float theta = 6.283 * rand(vec2(0.9 * v));
	
	float noise = rand(vec2(v));
	noise = clamp(noise, 0.0, 1.0);
	float g_rootRadius = 5.3;
	float g_tipRadius = 0.5;
	noise *= g_rootRadius*0.5 + g_tipRadius*0.5;
	
	vec3 norm = normalize(vec3(-es_in[0].normal.x,es_in[0].normal.y,0.f));
	vec3 norm2 = cross(es_in[0].normal,norm);
	
	// Single strand interpolation
	vec4 finalPosition = position + vec4(r * cos(theta)*norm + r * sin(theta)*norm2,0.0) ;//+ noise*Zaxis + noise*Yaxis;
	
	
	vec2 TessCoord = gl_TessCoord.xy;
	 // Apply noise to offset position.
    float noise_ = TessCoord.y;
    TessCoord *= vec2(1.0 * (1.0 * 1.0), 0.2);

	
	finalPosition.x += noise_ *  0.5;
    finalPosition.y += noise_ * 0.5;
    finalPosition.z += noise_ * 0.5;
	
	es_out.point =  light_view * light_model * model* vec4(finalPosition.xyz,1.0);
	es_out.pointWorld = model* finalPosition;
	es_out.thickness = es_in[0].thickness;

}