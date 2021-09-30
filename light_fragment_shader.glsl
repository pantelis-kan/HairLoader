#version 430

out vec4 FragColor;

void main()
{
    // color of the light cube
	// We create a different shader because we want the cube color to not be 
	// affected by the ambient, diffuse and specular colro of the light
    FragColor = vec4(1.0); // set all 4 vector values to 1.0
}