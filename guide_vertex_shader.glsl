#version 430

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aTransparency;
layout (location = 3) in float aSegmentIndex;
layout (location = 4) in float aThickness;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 color;
out float transparency;


void main()
{
    gl_Position = projection *  view * model * vec4(aPos, 1.0);
	
	color = aColor;
	transparency = aTransparency;
}