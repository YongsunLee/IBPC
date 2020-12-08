#version 450


in vec3 vPos;
in vec3 vDir;
in float fSpeed;
in float fCollideTime;

out vec4 FragColor;

void main()
{
	if(fCollideTime > 0.0f)
		FragColor = vec4(1, 0, 0, 1);
	else
		FragColor = vec4(0,0,0,1);
}
