#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out int selectionID;

in vec2 TexCoords;

uniform sampler2D albedo;
uniform unsigned int resourceID;

void main()
{
	FragColor = texture(albedo, TexCoords);
	selectionID = int(resourceID);
}