#version 430
#define NUM_CURVE_POINTS 10
layout( isolines, equal_spacing) in;

uniform mat4 view;
uniform mat4 model;

uniform vec3 world_up;

uniform sampler2D noiseTexture;

in CS_OUT{

	float thickness;
	vec3 point;
	vec3 point_color;
	float transparency;
	vec3 neighbor1;
	vec3 neighbor2;
	vec3 neighbor3;
	vec3 barycentrics;
	vec3 normal;
	
}es_in[];

out ES_OUT{

	float thickness;
	vec4 point;
	vec3 point_color;
	float transparency;
	
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
	
	vec3 p0 = es_in[0].point;
	vec3 p1 = es_in[1].point;
	vec3 p2 = es_in[2].point;
	vec3 p3 = es_in[3].point;
	
	float baryc0_1 = es_in[0].barycentrics.x;
	float baryc0_2 = es_in[0].barycentrics.y;
	float baryc0_3 = es_in[0].barycentrics.z;
	
	float baryc1_1 = es_in[1].barycentrics.x;
	float baryc1_2 = es_in[1].barycentrics.y;
	float baryc1_3 = es_in[1].barycentrics.z;
	
	float baryc2_1 = es_in[2].barycentrics.x;
	float baryc2_2 = es_in[2].barycentrics.y;
	float baryc2_3 = es_in[2].barycentrics.z;
	
	float baryc3_1 = es_in[3].barycentrics.x;
	float baryc3_2 = es_in[3].barycentrics.y;
	float baryc3_3 = es_in[3].barycentrics.z;

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
    //float noise_ = 2.0 * TessCoord.x;
    float noise_ = 2.0 * TessCoord.y;
    TessCoord *= vec2(1.0 * (1.0 * 1.0), 0.2);
    finalPosition.x += noise_ * (1.0 - 2.0 * texture(noiseTexture, TessCoord.xy).r) * 0.5;
    finalPosition.y += noise_ * (1.0 - 2.0 * texture(noiseTexture, TessCoord.xy ).r) * 0.5;
    finalPosition.z += noise_ * (1.0 - 2.0 * texture(noiseTexture, TessCoord.xy ).r) * 0.5;
	
	
	es_out.point =  view * model * vec4(finalPosition.xyz,1.0);
	es_out.thickness = es_in[0].thickness;
	es_out.point_color = es_in[0].point_color;
	es_out.transparency = es_in[0].transparency;

}