#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;


uniform vec3 CameraRight_ws;
uniform vec3 CameraUp_ws;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec3 location;	
uniform vec2 size;


void main()
{
	
	vec3 positionWorld = location 
	+ CameraRight_ws * aPos.x * size.x 
	+ CameraUp_ws * aPos.y * size.y;


	gl_Position	= projection * view * vec4(positionWorld,1.0);
	TexCoords = aPos.xy + vec2(0.5,0.5);
}