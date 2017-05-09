#version 450 core

out vec4 color;

uniform vec3 lightIntensities1 = vec3(0.4,0.2,0.2);
uniform vec3 lightIntensities2 = vec3(0.2,0.2,0.5);
uniform vec3 ambient = vec3(0.4);

layout (location=3) uniform mat4 vMat;
layout (location=4) uniform mat4 mMat;

layout (location=5) uniform float scalar;
layout (location=6) uniform uint colorScheme;
layout (location=7) uniform vec3 camPos;
layout (location=8) uniform uint meshIndex;
//layout (location=8) uniform bool lineDraw;
//
layout (location=11) uniform vec4 mini;
layout (location=12) uniform vec4 maxi;

in VS_OUT
{   vec3 position;
    vec3 normal;
} fs_in;

void main(void)
{
//    if (lineDraw)
//    {
//        color = vec4(fs_in.normal,1.0);
//        return;
//    }
    float p = length(fs_in.position);
    vec4 col;
    if (colorScheme == 0)
    {
        float k = 0.85+0.15*(length(fs_in.position)-mini.w)/(maxi.w-mini.w);
//        col = 0.5*vec4(mix(mix(vec3(0.5451, 0.5137, 0.4706), vec3(0.5176, 0.3294, 0.1255), 2*min(k, 0.5)), vec3(0.3804, 0.8118, 0.0941), 2*(max(k, 0.5)-0.5)), 2.0);
        col = meshIndex%3 == 0 ? vec4(k*vec3(0.6, 0.5, 0.5), 1.0) : meshIndex%3 == 1 ? vec4(k*vec3(0.45, 0.5, 0.5), 1.0) : vec4(k*vec3(0.5, 0.5, 0.5), 1.0);
    }

//    vec3 l = normalize(mat3(vMat*mMat)*(camPos*fs_in.position));
    vec3 l1 = normalize(mat3(vMat*mMat)*(vec3(0.0,10.0,0.0)-scalar*fs_in.position));
    float brightness1 = clamp(abs(dot(mat3(vMat*mMat)*fs_in.normal, l1)), 0.0, 1.0);
    vec3 l2 = normalize(mat3(vMat*mMat)*(vec3(10.0,0.0,0.0)-scalar*fs_in.position));
    float brightness2 = clamp(abs(dot(mat3(vMat*mMat)*fs_in.normal, l2)), 0.0, 1.0);
    color = vec4((ambient + brightness1 * lightIntensities1 + brightness2 * lightIntensities2) * col.rgb, col.a);
}
