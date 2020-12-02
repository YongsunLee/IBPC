#version 450

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 u_ProjView;
uniform vec3 u_CameraPos;

in vec3 v_Pos[];
in vec3 v_Dir[];
in float f_Speed[];

out vec3 vPos;
out vec3 vDir;
out float fSpeed;

const float size = 0.05f;

void main()
{
	vec3 base = v_Pos[0].xyz;
	vec3 up = vec3(0, 1, 0);
	vec3 to_camera = normalize(u_CameraPos - base);
	vec3 right = cross(up, to_camera);
	up = cross(to_camera, right);

	gl_Position = u_ProjView * vec4(base + ((right + up) * size), 1);
    EmitVertex();   
    gl_Position = u_ProjView * vec4(base + ((right - up) * size), 1);
    EmitVertex();
    gl_Position = u_ProjView * vec4(base + ((-right + up) * size), 1);
    EmitVertex();
    gl_Position = u_ProjView * vec4(base + ((-right - up) * size), 1);
    EmitVertex();

    EndPrimitive();
}
