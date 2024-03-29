#version 430


out float FragDepth;
in vec4 posWorld;

layout(location = 0) out vec4 varianceData;


vec2 ComputeMoments(float Depth) {   
	vec2 Moments;   // First moment is the depth itself.  
	Moments.x = Depth;   // Compute partial derivatives of depth.    
	float dx = dFdx(Depth);   
	float dy = dFdy(Depth);   // Compute second moment over the pixel extents.   
	Moments.y = Depth*Depth + 0.25*(dx*dx + dy*dy);   
	return Moments; 
} 


uniform float nearPlane;
uniform float farPlane;
void main()
{
	//float normalizedDepth = (gl_FragCoord.z - nearPlane) / (farPlane + abs(nearPlane));
	FragDepth =  gl_FragCoord.z ;
	
	vec2 Moments = ComputeMoments(FragDepth);
	varianceData = vec4(Moments.x,Moments.y, 0.0 , 1.0);

}


