//
//  test.c
//  OpenGLFrame
//
//  Created by 钟擎 on 16/1/31.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#version 330 core

layout (location = 0) in vec3 vPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(vPosition, 1.0);
}
