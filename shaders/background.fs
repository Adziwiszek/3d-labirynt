#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 1) uniform vec3 playerPos;
layout(location = 2) uniform bool touchedTarget;
out vec4 Color;
in  vec3 pos;

void main()
{   
    vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
    vec4 blue = vec4(0.0, 0.0,1.0,1.0 );
    float stepValueX = step(1.0, mod(pos.x * 33.0, 9.0));
    float stepValueY = step(1.0, mod(pos.y * 33.0, 9.0));
    float stepValueZ = step(1.0, mod(pos.z * 33.0, 9.0));

    float isBackground = stepValueX * stepValueY * stepValueZ;
    vec4 color_target  = vec4(0.0,1.0,0.0,1.0);
    float d = distance(playerPos, vec3(-1.0,-1.0, 1.0))*0.2;
    vec4 kolor = mix(red, color_target, d);

    Color = mix(kolor, black, isBackground);
    if (touchedTarget){
        Color=mix(blue, black, isBackground);
    }

}