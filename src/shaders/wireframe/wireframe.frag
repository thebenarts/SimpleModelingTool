#version 330 core
//uniform vec3 WIRE_COL;
//uniform vec3 FILL_COL;
in vec3 dist;
out vec4 FragColor;
void main()
{
  // Undo perspective correction.      
  vec3 dist_vec = dist * gl_FragCoord.w;
 
  // Wireframe rendering is better like this:
  //vec3 dist_vec = dist;
 
  // Compute the shortest distance to the edge
  float d = min(dist_vec[0], min(dist_vec[1], dist_vec[2]));
 
  // Compute line intensity and then fragment color
  float I = exp2(-2.0*d*d);
 
  FragColor.xyz = I*vec3(0.0,0.7,0.5) + (1.0 - I)*(0.3,0.3,0.3); 
  FragColor.a = 0.3;
}