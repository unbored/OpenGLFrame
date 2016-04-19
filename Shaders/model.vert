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

uniform mat4 model; //模型矩阵
uniform mat4 view;  //视图矩阵
uniform mat4 projection;    //投影矩阵
uniform mat4 mesh;  //内部网格矩阵

out vec2 fTexCoord;
out vec3 fNormal;
out vec3 fragPos;

void main()
{
    fTexCoord = vTexCoord;
    fNormal = mat3(transpose(inverse(model * mesh))) * vNormal;    //法线变换，确保非等比例缩放正常
    fragPos = vec3(model * mesh * vec4(vPosition, 1.0));   //变换过后的模型位置
    gl_Position = projection * view * (model * mesh) * vec4(vPosition, 1.0);
}
