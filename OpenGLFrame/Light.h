//
//  Light.h
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/17.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#pragma once

#include <iostream>
#include <string>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

enum LightType { Directional, Point, Spot };

class Light
{
public:
    Light();
    Light(LightType t);
    ~Light();

private:
    struct AttTab
    {
        int dist;
        glm::vec3 att;
    };
    
private:
    LightType type;
    glm::vec3 location;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 attenuate;
    glm::vec2 cutoff;
    bool enabled;
    static const AttTab attTab[];
    
private:
    void searchAtt(int dist);
    
public:
    void setType(LightType t) { type = t; };
    void setLocation(const glm::vec3 loc) { location = loc; };
    void setDirection(const glm::vec3 dir) { direction = -dir; };
    void setAmbient(const glm::vec3 color) { ambient = color; };
    void setDiffuse(const glm::vec3 color) { diffuse = color; };
    void setSpecular(const glm::vec3 color) { specular = color; };
    void setLightDistance(const int distance) { searchAtt(distance); };
    void setCutOff(const float innerAngle, const float outerAngle)
    { cutoff = glm::cos(glm::radians(glm::vec2(innerAngle, outerAngle))); };
    void turnOnOff() { enabled = !enabled; };
    void turnOnOff(bool state) { enabled = state; };
    void setUniform(const GLuint program, const char* name);
};

