#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 selectionID;

void main()
{
    FragColor = vec4(1.0f, 0.5f, 0.0f, 1.0f);
    selectionID = vec4(0.0);
}