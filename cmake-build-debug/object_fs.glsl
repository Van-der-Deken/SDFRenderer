#version 450

in float Distance;
layout (location = 0) out vec4 FragColor;
uniform float Alpha;

void main() {
    if(Distance >= 0.0)
        FragColor = vec4(1.0, 0.0, 0.0, Alpha);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 0.7);
}