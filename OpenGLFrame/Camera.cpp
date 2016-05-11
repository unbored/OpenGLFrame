//
//  Camera.cpp
//  OpenGLFrame
//
//  Created by 钟擎 on 16/2/1.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#include "Camera.h"

Camera::Camera()
{
    deltaTime = 0.0f;   // 当前帧遇上一帧的时间差
    lastFrame = 0.0f;   // 上一帧的时间
    
    height = 0.0f;  //相机高度
    pos   = glm::vec3(0.0f, height,  3.0f);
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    up    = glm::vec3(0.0f, 1.0f,  0.0f);
    speed = 500.0f;
    
    yaw   = -90.0f;  //注意朝向z负轴
    pitch = 0.0f;
    
    fovy = 45.0f;
    near = 0.1f;
    far = 1000.0f;
    aspect = 1.0f;
}

Camera::~Camera()
{
}

void Camera::doMovement(bool upPressed, bool downPressed, bool leftPressed, bool rightPressed)
{
    // 摄像机控制
    glm::vec3 tempFront;
//    tempFront = front;   //自由运动
    tempFront = glm::normalize(glm::vec3(front.x, height, front.z));   //FPS运动
    GLfloat cameraSpeed = speed * deltaTime;
//    std::cout << cameraSpeed << std::endl;
    if(upPressed)
        pos += cameraSpeed * tempFront;
    if(downPressed)
        pos -= cameraSpeed * tempFront;
    if(leftPressed)
        pos -= glm::normalize(glm::cross(tempFront, up)) * cameraSpeed;
    if(rightPressed)
        pos += glm::normalize(glm::cross(tempFront, up)) * cameraSpeed;
}

void Camera::doViewing(double xoffset, double yoffset)
{
	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 cameraFront;
	cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront.y = sin(glm::radians(pitch));
	cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(cameraFront);
}

void Camera::doZooming(GLfloat deltaDegree)
{
	fovy += deltaDegree;
	if (fovy > 179.0f)
		fovy = 179.0f;
	if (fovy < 1.0f)
		fovy = 1.0f;
}

glm::mat4 Camera::viewMatrix()
{
    return glm::lookAt(pos, pos + front, up);
};

glm::mat4 Camera::projMatrix()
{
    return glm::perspective(glm::radians(fovy), aspect, near, far);
}

void Camera::calcFrame()
{
    double currentFrame = std::clock() * 1000.0 / CLOCKS_PER_SEC;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
//    std::cout << deltaTime << std::endl;
}