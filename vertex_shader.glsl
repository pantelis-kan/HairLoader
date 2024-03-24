#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 diffuse;


out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normal_matrix;

void main()
{
    TexCoords = aTexCoords;
	Normal = normal_matrix * aNormal;
	
	// per-fragment world position:
	//multiplying the vertex position attribute with the model matrix only (not the view and projection matrix) to transform it to world space coordinates
	FragPos = vec3( model * vec4(aPos, 1.0));
	

    gl_Position = projection *  view * model * vec4(aPos, 1.0);
}