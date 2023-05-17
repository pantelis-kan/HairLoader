#version 430

out vec2 variance;
in vec4 posWorld;

void main()
{
	gl_FragDepth =  gl_FragCoord.z ;
	variance = vec2(gl_FragCoord.z);
	//shadow = vec4( gl_FragCoord.z,  gl_FragCoord.z,  gl_FragCoord.z, 0.0);

	
}


