#version 430

in vec3 fragment_color;
in float Transparency;
out vec4 FragColor;


void main()
{
	vec3 static_color = vec3(1,0.95,0.56);
	
	FragColor = vec4(static_color , Transparency);
	
	// Fix color values!
	//FragColor = vec4(fragment_color , Transparency);
}