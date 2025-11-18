#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

out vec4 Color;
in vec3 pos;

vec3 newpos=fract(pos*6)-0.5;
float PI = 3.142;

void main()
{   
    vec4 color1 = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 blue = vec4(0.0, 0.80, 1.0, 1.0);
    float pattern = mod(pos.y * 40.0, 2.0);
    Color = mix(color1, blue, pattern);
}