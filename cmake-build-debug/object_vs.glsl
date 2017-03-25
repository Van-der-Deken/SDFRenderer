#version 450

in vec3 VertexPosition;
in float SDF;

out float Distance;
uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(VertexPosition,1.0);
    Distance = SDF;
}