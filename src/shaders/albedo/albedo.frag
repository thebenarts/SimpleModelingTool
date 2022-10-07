#version 330 core
out vec4 FragColor;

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

uniform sampler2D albedo;

void main()
{
	vec3 color = texture(albedo, TexCoords).rgb;
	vec3 ambient = light.ambient * color;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * color;
	FragColor = vec4(ambient + diffuse, 1.0);
}