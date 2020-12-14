#version 450

// Data Packing
// word ����
struct Vertex{
	vec3 pos;
	float speed;
	vec3 dir;
	float collide_time;
};

layout (std430, binding = 0) buffer a_Particle{
	Vertex particles[];
};

out vec3 v_Pos;
out vec3 v_Dir;
out float f_Speed;
out float f_collideTime;

const vec3 g_gravity = vec3(0, -1, 0);

void main()
{
	v_Pos = particles[gl_VertexID].pos;
	v_Dir = particles[gl_VertexID].dir;
	f_Speed =  particles[gl_VertexID].speed;
	f_collideTime =  particles[gl_VertexID].collide_time;

	gl_Position = vec4(v_Pos, 1);
}