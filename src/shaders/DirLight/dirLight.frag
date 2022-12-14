#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out int selectionID;

struct DirLight{
	vec3 direction;
	vec3 ambient;	
	vec3 diffuse;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform DirLight light;

uniform unsigned int resourceID;

void main()
{
	vec3 ambient = light.ambient * vec3(0.5,0.5,0.5);

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(0.5,0.5,0.5);

	FragColor = vec4(ambient+diffuse, 1.0);
	selectionID = int(resourceID);
}