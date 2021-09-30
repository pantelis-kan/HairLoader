#version 430
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;
uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

uniform vec3 lightPos;
uniform vec3 kd;
uniform vec3 lightColor;
uniform sampler2D texture_diffuse1;
uniform vec3 camera_position;

void main(void)
{ 
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;
	
	// we normalize the vectors, so that the dot product gives the cosine, since their length |u|,|v| = 1 : u*v = |u||v|cos(u,v)
	vec3 norm = normalize(Normal);
	
	// the direction vector between the light source and the fragment's position
	vec3 light_direction = normalize(lightPos - FragPos);
	
	// The greater the angle between both vectors, the darker diffuse component is.
	//If the angle between both vectors is greater than 90 degrees then the result of the dot product will actually become negative 
	// and we end up with a negative diffuse component. For that reason we use the max function that returns the highest of both its parameters 
	// to make sure the diffuse component (and thus the colors) never become negative.
	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse = diff * lightColor;
	
	float specular_strength = 0.5;
	vec3 view_direction = normalize(camera_position - FragPos);
	
	//Note that we negate the lightDir vector. The reflect function expects the first vector to point from the light source towards 
	// the fragment's position, but the lightDir vector is currently pointing the other way around: from the fragment towards the light source 
	// (this depends on the order of subtraction earlier on when we calculated the lightDir vector)
	vec3 reflection = reflect(-light_direction,norm);
	
	//This shininess value is the shininess value of the highlight. The higher the shininess value of an object,
	// the more it properly reflects the light instead of scattering it all around and thus the smaller the highlight becomes. 
	int shininess = 4;
	float spec = pow(max(dot(view_direction, reflection), 0.0), shininess);
	vec3 specular = specular_strength * spec * lightColor;
	
	float light_strength = 1.0;
	vec3 result = ambient + diffuse + specular;
	color = texture(texture_diffuse1,TexCoords) * vec4(kd, 1.0) * vec4(result, 1.0) * light_strength;
}


