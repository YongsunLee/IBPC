#version 450

in vec4 aPos;
uniform mat4 u_matrix;

out vec4 FragColor;

void main()
{
	gl_Position = u_matrix * vec4(aPos);
}
