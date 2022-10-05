#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 nearPoint;
out vec3 farPoint;

out VS_OUT{
	mat4 projection;
	mat4 view;
} vs_out;

uniform mat4 projection;
uniform mat4 view;

vec3 UnprojectPoint(float z)
{
	mat4 viewInv = inverse(view);
	mat4 projInv = inverse(projection);

	vec4 unprojectedPoint = viewInv * projInv * vec4(aPos.x, aPos.y , z , 1.0);
	
	return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
	vs_out.projection =projection;
	vs_out.view = view;

	nearPoint = UnprojectPoint(0.0f);
	farPoint = UnprojectPoint(1.0f);
	gl_Position = vec4(aPos,1.0);
}