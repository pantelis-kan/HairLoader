#version 430

in vec3 fragment_color;
in float Transparency;
out vec4 FragColor;


void main()
{
	vec3 col = vec3(1,0.95,0.56);
	//FragColor = vec4(1.0 , 1.0 , 0.0 , 1.0);
	//FragColor = vec4(col , Transparency);
	
	// !!! FIX transparency flickering error !!!
	FragColor = vec4(col , 1.0);
}