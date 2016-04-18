//
//  test.c
//  OpenGLFrame
//
//  Created by 钟擎 on 16/1/31.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#version 330 core

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec3 vNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fTexCoord;
out vec3 fNormal;
out vec3 fragPos;

void main()
{
    fTexCoord = vTexCoord;
    fNormal = mat3(transpose(inverse(model))) * vNormal;    //法线变换，确保非等比例缩放正常
    fragPos = vec3(model * vec4(vPosition, 1.0));   //变换过后的模型位置
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
}
