//
//  Camera.hpp
//  OpenGLFrame
//
//  Created by 钟擎 on 16/2/1.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#pragma once
#include <iostream>
#include <ctime>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
public:
    Camera();
    ~Camera();
    
private:
    double deltaTime;   // 当前帧遇上一帧的时间差
    double lastFrame;   // 上一帧的时间
    
    glm::vec3 pos;
    glm::vec3 front;
    glm::vec3 up;
    GLfloat speed;
    GLfloat height;
    
    GLfloat lastX, lastY;
    GLfloat yaw, pitch;
    bool firstMouse;
    
    GLfloat fovy;
    GLfloat near;
    GLfloat far;
    GLfloat aspect;
    
public:
    void doMovement(bool up, bool down, bool left, bool right);
    void mouseCallback(double xpos, double ypos);
    void calcFrame();
    void calcFrame(double delta) { deltaTime = delta; };
    glm::mat4 viewMatrix();
    glm::mat4 projMatrix();
    glm::vec3 position() { return pos; };
    void setPos(glm::vec3 position) { pos = position; };
    void setFovy(GLfloat degree) { fovy = degree; };
    void setRange(GLfloat near, GLfloat far) { this->near = near; this->far = far; };
    void setAspect(GLfloat aspect) { this->aspect = aspect; };
    glm::vec3 getFront() { return front; };
};
