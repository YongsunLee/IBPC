#version 450

layout(location=0) out vec4 FragColor;

in vec3 vPos;
in vec3 vDir;
in float fSpeed;
in float fCollideTime;

void main()
{
	FragColor = vec4(1, 0, 0, 1);
}
