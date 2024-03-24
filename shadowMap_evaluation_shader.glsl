#version 430
layout( isolines, equal_spacing) in;

uniform mat4 view;
uniform mat4 model;

uniform vec3 world_up;

uniform sampler2D noiseTexture;
uniform mat4 lightSpaceMatrix;

uniform mat4 light_model;
uniform mat4 light_view;
uniform mat4 light_projection;

layout(std430, binding = 0) buffer SSBO {
    vec3 data[];
};

in CS_OUT{
	vec3 point;
	float thickness;
	vec3 normal;
	vec3 barycentrics;
	
	vec3 guide_neighb2;
	vec3 guide_neighb3;
	
	vec3 normal_neighb1;
	vec3 normal_neighb2;
	vec3 normal_neighb3;
}es_in[];

out ES_OUT{
	vec4 point;
	float thickness;
	vec4 pointWorld;
	vec4 pointCameraSpace;
	
	bool last_segment;
	bool first_segment;
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

vec4 SingleStrandInterpolation(float v, vec4 position, vec3 normal, float barycentric){
	float radius = 1.0;
	
	float r = 4.0 * sqrt(rand(vec2(v)));
    float theta = 6.283 * rand(vec2(0.9 * v));
	
	float noise = rand(vec2(v));
	noise = clamp(noise, 0.0, 1.0);
	float g_rootRadius = 5.3;
	float g_tipRadius = 0.5;
	noise *= g_rootRadius*0.5 + g_tipRadius*0.5;
	
	vec3 norm = normalize(vec3(-normal.x,normal.y,0.f));
	vec3 norm2 = cross(normal,norm);
	
	vec4 finalPos = position + vec4(r * cos(theta)*norm + r * sin(theta)*norm2,0.0) ;
	
    float noise_ = es_in[0].barycentrics.x;
	
	finalPos.x += noise_ *  0.5;
    finalPos.y += noise_ * 0.5;
    finalPos.z += noise_ * 0.5;
	
	return finalPos;

}

void main( )
{


	float v = gl_TessCoord.y;
	
	
	vec3 p0 =  es_in[0].point;
	vec3 p1 = es_in[1].point;
	vec3 p2 = es_in[2].point;
	vec3 p3 = es_in[3].point;

	//vec3 test_barycentric = vec3(0.33,0.33,0.33);
	// Ensure that gl_TessCoord.y is normalized to fit within the range of the array
    float arrayIndex = clamp(v * (128.0 - 1.0), 0.0, 128.0 - 1.0);

    // Round the array index to the nearest integer
    int index = int(round(arrayIndex));

	
	vec3 test_barycentric = data[int(gl_TessCoord.y * (data.length() - 1))];
	//vec3 test_barycentric = vec3(0.333 , 0.333, 0.333);
	vec3 interpolated_normal;
	
	bool doMultiStrand = test_barycentric.x > 0.3 ? true: false;
	
	
	if(es_in[0].normal_neighb2.x != -1 && doMultiStrand){
		p0 = test_barycentric.x * es_in[0].point + test_barycentric.y * es_in[0].guide_neighb2 + test_barycentric.z * es_in[0].guide_neighb3;
		p1 = test_barycentric.x * es_in[1].point + test_barycentric.y * es_in[1].guide_neighb2 + test_barycentric.z * es_in[1].guide_neighb3;
		p2 = test_barycentric.x * es_in[2].point + test_barycentric.y * es_in[2].guide_neighb2 + test_barycentric.z * es_in[2].guide_neighb3;
		p3 = test_barycentric.x * es_in[3].point + test_barycentric.y * es_in[3].guide_neighb2 + test_barycentric.z * es_in[3].guide_neighb3;
	
		interpolated_normal = test_barycentric.x * es_in[0].normal_neighb1 + test_barycentric.y * es_in[0].normal_neighb2 + test_barycentric.z * es_in[0].normal_neighb3;

	}
	
	
	float u = clamp(gl_TessCoord.x, 0.0, 1.0);
	
	vec4 position = B_spline(p0,p1,p2,p3,gl_TessCoord.x);
	vec4 tangent = Tangent(p0,p1,p2,p3,u);

	vec4 finalPosition;

	vec3 norm = normalize(vec3(-es_in[0].normal.x,es_in[0].normal.y,0.f));
	vec3 norm2 = cross(es_in[0].normal,norm);
	
	if(es_in[0].normal_neighb2.x == -1 ){
		finalPosition = SingleStrandInterpolation(v, position,es_in[0].normal,es_in[0].barycentrics.x);
		//finalPosition = position;
	}
	else{
	
		if(!doMultiStrand) finalPosition = SingleStrandInterpolation(v, position,es_in[0].normal,es_in[0].barycentrics.x);

		else finalPosition =    v * vec4(interpolated_normal,0.0)  +   position;
		//finalPosition =      position;

	}
	
	
	//finalPosition = SingleStrandInterpolation(v, position,es_in[0].normal,es_in[0].barycentrics.x);

	es_out.point =  light_view  *model* vec4(finalPosition.xyz,1.0);
	es_out.pointWorld = model* finalPosition;
	es_out.pointCameraSpace = view* model* vec4(finalPosition.xyz,1.0);
	es_out.thickness = es_in[0].thickness;
	if(p2.x == p3.x && p2.y == p3.y && p3.z == p2.z) es_out.last_segment = true;
	es_out.first_segment = false;
	if(p0.x == p1.x && p0.y == p1.y && p0.z == p1.z) es_out.first_segment = true;

}