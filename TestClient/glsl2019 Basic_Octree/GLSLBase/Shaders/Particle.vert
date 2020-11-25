#version 450

in vec3 a_Pos;
in vec3 a_Dir;
in float a_Speed;
in float a_CollideTime;

const vec3 g_gravity = vec3(0, -1, 0);

out vec3 v_Pos;
out vec3 v_Dir;
out float f_Speed;
out float f_CollideTime;

uniform float u_Time;

void main()
{
	v_Pos = a_Pos + ((a_Dir * a_Speed) + (g_gravity * u_Time * a_Speed));
	v_Dir = a_Dir;
	f_Speed = a_Speed;
	f_CollideTime = a_CollideTime;
	
	gl_Position = vec4(a_Pos, 1);
}
