#version 430

layout (location = 0) in vec3 aPos;


uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{

	vec4 worldPos = model * vec4(aPos,1.0);
	vec4 lightSpaceWorldPos = lightSpaceMatrix * worldPos;
	gl_Position = lightSpaceWorldPos;
	//gl_Position = vec4(0.5);
	//fragPos = fragPos / fragPos.w;
	
}

