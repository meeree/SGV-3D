#version 450 core

layout (location=0) in vec3 position;
layout (location=1) in vec4 color;
layout (location=2) uniform mat4 model;

out VS_OUT
{   
    vec4 color;
} vs_out;

void main(void)
{
    gl_Position = model * vec4(position, 1.0);
    vs_out.color = color;
}
