#version 430

layout (lines) in
layout (triangle_strip, max_vertices = 4) out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main(){

	mat4 model_view = view*model;

}