#version 430

layout(location = 0) out vec4 varianceData;

vec2 ComputeMoments(float Depth) {   
	vec2 Moments;   // First moment is the depth itself.  
	Moments.x = Depth;   // Compute partial derivatives of depth.    
	float dx = dFdx(Depth);   
	float dy = dFdy(Depth);   // Compute second moment over the pixel extents.   
	Moments.y = Depth*Depth + 0.25*(dx*dx + dy*dy);   
	return Moments; 
} 

void main()
{
	gl_FragDepth = gl_FragCoord.z;
	
	vec2 Moments = ComputeMoments(gl_FragDepth);
	varianceData = vec4(Moments.x,Moments.y, 0.0 , 1.0);
	
	/*float nearPlane = 0.1;
	float farPlane = 1000.0;
	float linearDepth = (2.0*nearPlane ) / (farPlane + nearPlane - gl_FragCoord.z*(farPlane -nearPlane)) ;
	gl_FragDepth = linearDepth;*/
}


