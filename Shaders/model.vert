//
//  test.c
//  OpenGLFrame
//
//  Created by 钟擎 on 16/1/31.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vNormal;
layout (location = 2) in vec2 vTexCoord;
layout (location = 3) in vec3 vTangent;
layout (location = 4) in vec3 vBitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

uniform mat4 model; //模型矩阵
uniform mat4 view;  //视图矩阵
uniform mat4 projection;    //投影矩阵

uniform vec3 viewPos;

uniform bool hasAnimation;
layout(std140) uniform Bone //骨骼偏移矩阵
{
    mat4 bones[100];
};
//uniform mat4 bones[100];

out vec2 fTexCoord;
out mat3 TBN;
out vec3 tanFragPos;
out vec3 tanViewPos;
out vec3 tanNormal;

void main()
{
    mat4 modelAll = model;
    if (hasAnimation)
    {
        mat4 animat = bones[boneIDs.x] * weights.x;
        animat += bones[boneIDs.y] * weights.y;
        animat += bones[boneIDs.z] * weights.z;
        animat += bones[boneIDs.w] * weights.w;
        modelAll = animat;
    }
    gl_Position = projection * view * modelAll * vec4(vPosition, 1.0);
    vec3 fragPos = vec3(modelAll * vec4(vPosition, 1.0));   //变换过后的模型位置
    
    mat3 matNormal = mat3(transpose(inverse(modelAll)));    //法线矩阵
    vec3 N = normalize(matNormal * vNormal);    //法线变换，确保非等比例缩放正常
    vec3 T = normalize(matNormal * vTangent);
    vec3 B = normalize(matNormal * vBitangent);
    
    fTexCoord = vTexCoord;
    TBN = transpose(mat3(T, B, N));
    tanFragPos = TBN * fragPos;
    tanViewPos = TBN * viewPos;
    tanNormal = TBN * N;

}
