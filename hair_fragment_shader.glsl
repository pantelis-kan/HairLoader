#version 430

in vec3 Color;
in float Transparency;

out vec4 FragColor;


uniform float default_transparency;

void main()
{

    //FragColor = vec4(Color , Transparency);
	FragColor = vec4(Color , default_transparency);
}