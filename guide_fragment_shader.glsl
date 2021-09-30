#version 430

in vec3 color;
in float transparency;
out vec4 FragColor;



void main()
{
	//FragColor = vec4(1.0 , 1.0 , 0.0 , 1.0);
	FragColor = vec4(color , transparency);
}