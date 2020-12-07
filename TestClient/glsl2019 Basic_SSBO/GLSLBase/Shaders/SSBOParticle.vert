#version 450


layout (std430, binding = 0) buffer a_Pos{
	vec3 pos[];
};


layout (std430, binding = 1) buffer a_Dir{
	vec3 dir[];
};

layout (std430, binding = 2) buffer a_Speed{
	float speed[];
};

layout (std430, binding = 3) buffer a_CollideTime{
	float collideTime[];
};

out vec3 v_Pos;
out vec3 v_Dir;
out float f_Speed;
out float f_collideTime;

void main()
{
	v_Pos = pos[gl_InstanceID]; // pos[gl_InstanceID] + ((dir[gl_InstanceID] * speed) + (g_gravity * u_Time * speed));
	v_Dir = dir[gl_InstanceID];
	f_Speed = speed[gl_InstanceID];
	f_collideTime = collideTime[gl_InstanceID];
	
	gl_Position = vec4(v_Pos, 1);
}