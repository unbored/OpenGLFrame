//
//  Light.cpp
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/17.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#include "Light.h"

//衰减值表
const Light::AttTab Light::attTab[12] = {
    {7,		glm::vec3(1.0,	0.7, 	1.8)},
    {13,	glm::vec3(1.0,	0.35,	0.44)},
	{20,	glm::vec3(1.0,	0.22,	0.20)},
	{32,	glm::vec3(1.0,	0.14,	0.07)},
	{50,	glm::vec3(1.0,	0.09,	0.032)},
	{65,	glm::vec3(1.0,	0.07,	0.017)},
	{100,	glm::vec3(1.0,	0.045,	0.0075)},
	{160,	glm::vec3(1.0,	0.027,	0.0028)},
	{200,	glm::vec3(1.0,	0.022,	0.0019)},
	{325,	glm::vec3(1.0,	0.014,	0.0007)},
	{600,	glm::vec3(1.0,	0.007,	0.0002)},
	{3250,	glm::vec3(1.0,	0.0014,	0.000007)}
};

Light::Light()
{
    type = Directional;
    location = glm::vec3(0.0);
    direction = glm::vec3(0.0, 0.0, -1.0);
    ambient = glm::vec3(0.1);
    diffuse = glm::vec3(0.5);
    specular = glm::vec3(1.0);
    attenuate = glm::vec3(1.0,	0.045,	0.0075);
    setCutOff(15.5f, 17.5f);
    enabled = true;
}

Light::Light(LightType t)
{
    type = t;
    location = glm::vec3(0.0);
    direction = glm::vec3(0.0, 0.0, -1.0);
    ambient = glm::vec3(0.1);
    diffuse = glm::vec3(0.5);
    specular = glm::vec3(1.0);
    attenuate = glm::vec3(1.0,	0.045,	0.0075);
    setCutOff(15.5f, 17.5f);
    enabled = true;
}

Light::~Light()
{
    
}

//查表返回合适的衰减值
void Light::searchAtt(int dist)
{
    int len = sizeof(attTab) / sizeof(AttTab);
    for (int i = 0; i < len; i++)
    {
        if (dist <= attTab[i].dist)
        {
            attenuate = attTab[i].att;
            return;
        }
    }
    attenuate = attTab[len-1].att;
}

void Light::setUniform(const GLint program, const char *name)
{
    std::string lightName = name;
    glUniform1i(glGetUniformLocation(program, (lightName + ".type").c_str()), (int)type);
    glUniform3f(glGetUniformLocation(program, (lightName + ".location").c_str()),
                location.x, location.y, location.z);
    glUniform3f(glGetUniformLocation(program, (lightName + ".direction").c_str()),
                direction.x, direction.y, direction.z);
    glUniform3f(glGetUniformLocation(program, (lightName + ".ambient").c_str()),
                ambient.r, ambient.g, ambient.b);
    glUniform3f(glGetUniformLocation(program, (lightName + ".diffuse").c_str()),
                diffuse.r, diffuse.g, diffuse.b);
    glUniform3f(glGetUniformLocation(program, (lightName + ".specular").c_str()),
                specular.r, specular.g, specular.b);
    glUniform3f(glGetUniformLocation(program, (lightName + ".attenuate").c_str()),
                attenuate.x, attenuate.y, attenuate.z);
    glUniform2f(glGetUniformLocation(program, (lightName + ".cutoff").c_str()),
                cutoff.x, cutoff.y);
    glUniform1i(glGetUniformLocation(program, (lightName + ".enabled").c_str()), (GLint)enabled);
}