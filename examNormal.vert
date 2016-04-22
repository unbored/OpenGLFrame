#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;

out vec3 gNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 mesh;  //内部网格矩阵

uniform sampler2D bumpTex;

void main()
{
    gl_Position = projection * view * (model * mesh) * vec4(vPosition, 1.0f);
//    mat3 matNormal = mat3(transpose(inverse(view * (model * mesh))));
//    gNormal = normalize(vec3(projection * vec4(matNormal * vNormal, 1.0)));
//    vec3 N = normalize(matNormal * vNormal);    //法线变换，确保非等比例缩放正常
//    vec3 T = normalize(matNormal * vTangent);
//    vec3 B = normalize(matNormal * vBitangent);
//    
//    mat3 TBN = mat3(T, B, N);
    
    gNormal = texture(bumpTex, vTexCoord).xyz * 2 - vec3(1.0);
    
//    gNormal = normalize(vec3(projection * vec4(TBN * texNormal, 1.0)));
}