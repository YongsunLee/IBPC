#version 450

in vec3 a_Position;
in vec3 a_Normal;
in vec4 a_Color;

uniform mat4 u_ProjView;
uniform mat4 u_Transform;

out vec4 v_Color;

void main()
{
  gl_Position = u_ProjView * u_Transform * vec4(a_Position, 1.f);
  v_Color = a_Color;
}