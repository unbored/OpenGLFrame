//
//  test.c
//  OpenGLFrame
//
//  Created by 钟擎 on 16/1/31.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#version 330 core

uniform vec3 lightPos;
uniform vec3 lightColor;

out vec4 fragColor;

void main()
{
    fragColor = vec4(lightColor, 1.0f);
}
