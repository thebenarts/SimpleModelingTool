#version 330 core
out vec4 FragColor;

in VS_OUT{
mat4 projection;
mat4 view;
} vs_in;

float far = gl_DepthRange.far;
float near = gl_DepthRange.near;

in vec3 nearPoint;
in vec3 farPoint;

vec4 grid(vec3 fragPos3D, float scale)
{
	vec2 coord = fragPos3D.xz * scale;		// use the scale variable to set the distance between the lines
	vec2 derivative = fwidth(coord);
	vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
	float line = min(grid.x, grid.y);
	float minimumz = min(derivative.y, 1);
	float minimumx = min(derivative.x, 1);
	vec4 color = vec4(0.2,0.2,0.2,1.0 - min(line, 1.0));

	// z axis
	if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
		color.z = 1.0;

	// x axis
	if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
		color.x = 1.0;

	return color;
}

float computeDepth(vec3 pos)
{
	vec4 clip_space_pos = vs_in.projection * vs_in.view * vec4(pos.xyz, 1.0);
	return (clip_space_pos.z / clip_space_pos.w);
}

void main()
{
	float t = -nearPoint.y / (farPoint.y - nearPoint.y);
	vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

	float ndc_depth = computeDepth(fragPos3D);
	float depth = (((far-near) * ndc_depth) + near + far) / 2.0;
	gl_FragDepth = depth;
	FragColor = grid(fragPos3D, 1) * float(t > 0);
}