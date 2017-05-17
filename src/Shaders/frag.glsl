#version 450 core

out vec4 color;

uniform vec3 lightIntensities = vec3(0.7);
uniform vec3 ambient = vec3(0.7);

layout (location=2) uniform vec3 uniColor;

layout (location=3) uniform mat4 pMat;
layout (location=4) uniform mat4 vMat;
layout (location=5) uniform mat4 mMat;

layout (location=6) uniform float scalar;
layout (location=7) uniform uint colorScheme;
layout (location=8) uniform vec3 camPos;
//layout (location=9) uniform bool lineDraw;
//
layout (location=12) uniform vec4 mini;
layout (location=13) uniform vec4 maxi;

in VS_OUT
{   
    vec3 position;
    vec3 normal;
} fs_in;

void main(void)
{
//    if (lineDraw)
//    {
//        color = vec4(fs_in.normal,1.0);
//        return;
//    }
    vec4 col;
    if (colorScheme == 0)
    {
//        float k = length(mMat*vec4(fs_in.position,1.0))/length(vec3(15,15,10));
//        col = mix(vec4(0.4,0.22,0.11,1.0),vec4(0.1,0.5,0.2,1.0),k);
        col = vec4(uniColor,1.0);
    }

//    vec3 l = normalize(mat3(vMat*mMat)*(camPos*fs_in.position));
    vec3 l = normalize(mat3(vMat*mMat)*(camPos-scalar*fs_in.position));
    float brightness = clamp(abs(dot(mat3(vMat*mMat)*fs_in.normal, l)), 0.0, 1.0);
    color = vec4((ambient + brightness * lightIntensities) * col.rgb, col.a); 
}
