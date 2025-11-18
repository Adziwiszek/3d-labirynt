#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 1) uniform float time;
layout(location = 2) uniform bool isTarget;
out vec4 Color;
in vec3 pos;

vec3 newpos=fract(pos*6)-0.5;
float PI = 3.142;

void main()
{   
    vec3 color1 = vec3(pos*0.5 +0.5);
    float d = length(pos.xyz);
    d = sin(d*8+time)/8;
    d = 0.05/abs(d);

    color1*=d;
    // d = smoothstep(0.0,0.05,d);
    Color = vec4(color1, 0.5);

    if (isTarget){
        vec3 c = vec3(0.0,1.0,0.0);
        float g = length(newpos.xyz);
        g =sin(g*8+time)/8;
        g = 0.01/abs(g);
        Color = vec4(c*g,1.0);
    }
}