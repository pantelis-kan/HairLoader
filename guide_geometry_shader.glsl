#version 430

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;

uniform vec3 bboxMin;
uniform vec3 bboxMax;
uniform mat4 lightSpaceMatrix;
uniform vec3 camera_position;
uniform vec3 camera_direction;
uniform vec3 camera_front;
uniform vec4 camera_position_view;
uniform vec3 lightPos;

out vec3 Tangent;
out vec3 Normal;
out vec4 posWorld;
out vec4 posWorld2;
out vec4 viewSpace;
out vec2 texCoord;
out vec4 shadowCoord;

out vec3 Barycentrics;

out vec3 Eye;
out vec3 lightDir;

out vec3 HairBarycentric;

in ES_OUT{

	float thickness;
	vec4 point;
	vec4 pointWorld;
	vec4 pointWorld2;
	vec3 Tangent;
	vec3 Normal;
	vec4 shadowCoord;
	vec3 barycentrics;
	
	bool last_segment;
	bool first_segment;
	vec3 HairBarycentric;
	
}gs_in[];


void main(){

	//Tangent = gs_in[1].Tangent;
	vec4 worldSpacePos1 = gs_in[0].pointWorld;
	vec4 worldSpacePos2 = gs_in[1].pointWorld;
	vec4 worldSpacePos1_2 = gs_in[0].pointWorld2;
	vec4 worldSpacePos2_2 = gs_in[1].pointWorld2;
	//Tangent =  gs_in[0].Tangent;
	Normal = gs_in[0].Normal;
	
	vec4 p1 = gs_in[0].point;
	vec4 p2 = gs_in[1].point;
	
	vec2 t0 = vec2(0.0, 0.0);
    vec2 t1 = vec2(1.0, 0.0);

	vec3 tangent = p2.xyz - p1.xyz;
	//vec3 tangent = worldSpacePos2.xyz - worldSpacePos1.xyz;
	
	tangent = normalize(tangent);
    
	
	// Calculate the view direction for the current vertex
    vec3 viewDir = normalize((view * vec4(p1.xyz, 1.0)).xyz - gl_Position.xyz);
    
	vec3 eyeVec = p1.xyz;
	//vec3 eyeVec = normalize(camera_position - worldSpacePos1.xyz);
	vec3 side_vector = normalize(cross(eyeVec, tangent));

		// Calculate the camera direction vector (in world space)
	vec3 cameraDirection = camera_position - worldSpacePos1.xyz;


	
	float radius1 = gs_in[0].thickness * 0.13;
	float radius2 = gs_in[1].thickness * 0.13;

	if(gs_in[0].first_segment == true){
		radius2 = gs_in[0].thickness * 0.13 * 1.5;
	}
	
	if(gs_in[0].last_segment == true){
		radius2 = gs_in[1].thickness * 0.13 * 0.3;
	}

	
	float length = length(p2.xyz - p1.xyz);
    texCoord = vec2(0.0, 0.0);
    texCoord.y = length;
	
	Barycentrics = gs_in[0].barycentrics;
	
	mat4 invView = inverse(view); // TODO: compute ahead of time?
	//vec3 cameraDir = vec3(invView[3][0], invView[3][1], invView[3][2]);
	vec3 cameraDir = invView[3].xyz;
	
	vec3 camera_pos = cameraDir  ;
	
	vec3 Pos;
	vec4 position;
	
	HairBarycentric = gs_in[0].HairBarycentric;

	Pos = p1.xyz + ((side_vector + tangent) * radius1);
	posWorld = worldSpacePos1;
	posWorld2 =  worldSpacePos1_2;
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	//Tangent = (model*vec4(tangent,1.0)).xyz;
	Tangent = (worldSpacePos2.xyz - worldSpacePos1.xyz);
	//Eye = camera_pos - p1.xyz ;
	Eye =  camera_pos - posWorld.xyz;
	//Eye =  posWorld.xyz;
	lightDir =  - posWorld.xyz;
	texCoord.x = 0.0;
	
	/*position =  view * vec4(worldSpacePos1.xyz,1.0);
	Pos = position.xyz + vec3(-radius1,0.0,0.0);
	gl_Position = projection * vec4(Pos,1.0);*/
	
	gl_Position =  projection *  vec4(Pos, 1.0);
	viewSpace = view * posWorld;
	EmitVertex();

	Pos = p1.xyz - ((side_vector - tangent) * radius1);
	posWorld = worldSpacePos1;
	posWorld2 =  worldSpacePos1_2;
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	viewSpace = view * posWorld;
	//Tangent = (model*vec4(tangent,1.0)).xyz;
	Tangent = (worldSpacePos2.xyz - worldSpacePos1.xyz);
	//Eye = camera_pos - p1.xyz ;
	Eye =  camera_pos - posWorld.xyz;
	//Eye =  posWorld.xyz;
	lightDir =  - posWorld.xyz;
	texCoord.x = 1.0;
	
	/*position =   view * vec4(worldSpacePos1.xyz,1.0);
	Pos = position.xyz + vec3(radius1,0.0,0.0);
	gl_Position = projection * vec4(Pos,1.0);*/
	
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();
	

	Pos = p2.xyz + ((side_vector + tangent) * radius2);
	posWorld = worldSpacePos1;
	posWorld2 =  worldSpacePos1_2;
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	//Tangent = (model*vec4(tangent,1.0)).xyz;
	Tangent = (worldSpacePos2.xyz - worldSpacePos1.xyz);
	//Eye = camera_pos - p2.xyz ;
	Eye =  camera_pos - posWorld.xyz;
	//Eye =  posWorld.xyz;
	lightDir =  - posWorld.xyz;
	viewSpace = view * posWorld;
	texCoord.x = 0.0;
	
	/*position =   view * vec4(worldSpacePos2.xyz,1.0);
	Pos = position.xyz + vec3(-radius2,0.0,0.0);
	gl_Position = projection * vec4(Pos,1.0);*/
	
	gl_Position =  projection * vec4(Pos, 1.0);
	EmitVertex();

	Pos = p2.xyz - ((side_vector - tangent) * radius2);
	posWorld = worldSpacePos1;
	posWorld2 =  worldSpacePos1_2;
	shadowCoord = lightSpaceMatrix * vec4(posWorld.xyz,1.0);
	//Tangent = (model*vec4(tangent,1.0)).xyz;
	Tangent = (worldSpacePos2.xyz - worldSpacePos1.xyz);
	//Eye = camera_pos - p2.xyz ;
	Eye =  camera_pos - posWorld.xyz;
	//Eye =  posWorld.xyz;
	lightDir =  - posWorld.xyz; 
	viewSpace = view * posWorld;
	texCoord.x = 1.0;
	
	/*position = view * vec4(worldSpacePos2.xyz,1.0);
	Pos = position.xyz + vec3(radius2,0.0,0.0);
	gl_Position =  projection *  vec4(Pos,1.0);*/
	
	gl_Position = projection * vec4(Pos, 1.0);
	EmitVertex();


	EndPrimitive();
	
}



