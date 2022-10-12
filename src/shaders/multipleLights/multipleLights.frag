#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out int selectionID;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_in;

uniform vec3 viewPos;
uniform sampler2D albedo;

struct DirLight{
	vec3 direction;
	vec3 ambient;	
	vec3 diffuse;
};

uniform DirLight dirLight;

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    bool bCalculate;
};
#define NR_POINT_LIGHTS 50

struct SpotLight
{
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    bool bCalculate;
};
#define NR_SPOT_LIGHTS 50

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(vs_in.Normal);
    vec3 viewDir = normalize(viewPos - vs_in.FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < NR_POINT_LIGHTS; ++i)
    {
        if(pointLights[i].bCalculate)
        {
            result += CalcPointLight(pointLights[i], norm, vs_in.FragPos, viewDir);
        }
    }
    
    for(int i = 0; i < NR_SPOT_LIGHTS; ++i)
    {
        if(spotLights[i].bCalculate)
        {
            result+= CalcSpotLight(spotLights[i], norm, vs_in.FragPos, viewDir);
        }
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir),0.0);

    vec3 ambient = light.ambient * vec3(texture(albedo, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(albedo, TexCoords));

    return (ambient + diffuse);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);        // vector pointing from fragPos to light

    float diff = max(dot(normal, lightDir), 0.0);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(albedo, TexCoords));
    vec3 diffuse = light.diffuse *  diff * vec3(texture(albedo, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;

    return (ambient + diffuse);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);        // vector pointing from fragPos to light

    float diff = max(dot(normal, lightDir), 0.0);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0,1.0);

    vec3 ambient = light.ambient * vec3(texture(albedo, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(albedo, TexCoords));

   ambient *= attenuation * intensity;
   diffuse *= attenuation * intensity;

   return (ambient + diffuse);
}