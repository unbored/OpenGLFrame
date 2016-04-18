//
//  main.cpp
//  FreeglutFrame
//
//  Created by 钟擎 on 16/1/31.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#include <iostream>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "SOIL/SOIL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"

/////////////Window/////////////////////
const GLint resX = 800, resY = 600;
GLFWwindow* window;

/////////////View///////////////////////
Camera testCamera;

////////////Shader//////////////////
Shader modelShader;
Shader lightShader;

/////////////Vertex Info////////////////////
GLfloat vertices[] = {
    //vertex, texture, normal
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,  0.0f, -1.0f,
    
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  0.0f, 1.0f,
    
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
    
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,  0.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  0.0f,  0.0f,
    
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
    0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, -1.0f,  0.0f,
    
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
    0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f
};

GLuint indices[] = { // 起始于0!
    //前
    0, 1, 3, // 第一个三角形
    1, 2, 3,  // 第二个三角形
    //后
    4, 5, 7, // 第一个三角形
    5, 6, 7,  // 第二个三角形
    //左
    4, 5, 7, // 第一个三角形
    5, 6, 7,  // 第二个三角形
    //右
    4, 5, 7, // 第一个三角形
    5, 6, 7,  // 第二个三角形
    //上
    4, 5, 7, // 第一个三角形
    5, 6, 7,  // 第二个三角形
    //下
    4, 5, 7, // 第一个三角形
    5, 6, 7  // 第二个三角形
    
};

GLuint vaoID, lightVaoID;

//////////////////Texture//////////////////
GLuint texID[2];

//////////////////光照////////////////////
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
Light lightDirect(Directional), lightSpot(Spot);

/////////////////键盘//////////////////
bool keys[1024];

////////////////帧率//////////////////
double lastFrame;

/////////////程序开始///////////////////////
void glCheckError(const char* position)
{
    GLenum e;
    while ((e = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error " << e << " at " << position << std::endl;
    }
}



void glDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, resX, resY);
    
//    glm::mat4 model;
//    model = glm::rotate(model, (GLfloat)glfwGetTime() * 1.0f, glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 view;
    view = testCamera.viewMatrix();
    glm::mat4 projection;
    projection = glm::perspective(45.0f, float(resX) / resY, 0.1f, 100.0f);
    
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    
    //=======绘制模型============
    modelShader.use();
    modelShader.setUniform("view", 1, GL_FALSE, &view);
    modelShader.setUniform("projection", 1, GL_FALSE, &projection);
//    glUniformMatrix4fv(testShader.location("model"), 1, GL_FALSE, glm::value_ptr(model));
//    glUniformMatrix4fv(modelShader.location("view"), 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(modelShader.location("projection"), 1, GL_FALSE, glm::value_ptr(projection));
//    glUniform3f(modelShader.location("lightPos"), lightPos.x, lightPos.y, lightPos.z);
//    glUniform3f(modelShader.location("lightColor"), lightColor.r, lightColor.g, lightColor.b);
    glm::vec3 cameraPos = testCamera.position();
    modelShader.setUniform("viewPos", cameraPos);
//    glUniform3f(modelShader.location("viewPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    lightDirect.setUniform(modelShader.getProgram(), "lightDirect");
    
    lightSpot.setLocation(cameraPos);
    lightSpot.setDirection(testCamera.getFront());
    lightSpot.setUniform(modelShader.getProgram(), "lightSpot");

    modelShader.setUniform("tex", 0);
//    glUniform1i(modelShader.location("tex"), 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    modelShader.setUniform("tex1", 1);
//    glUniform1i(modelShader.location("tex1"), 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID[1]);
    glBindVertexArray(vaoID);
//    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
    for(GLuint i = 0; i < 10; i++)
    {
        glm::mat4 model;
        model = glm::translate(model, cubePositions[i]);
        GLfloat angle = 20.0f * i;
        model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
        modelShader.setUniform("model", 1, GL_FALSE, &model);
//        glUniformMatrix4fv(modelShader.location("model"), 1, GL_FALSE, glm::value_ptr(model));
        
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glCheckError("modelDrawing");
    
    //========绘制光照点==========
//    lightShader.use();
//    glUniformMatrix4fv(lightShader.location("view"), 1, GL_FALSE, glm::value_ptr(view));
//    glUniformMatrix4fv(lightShader.location("projection"), 1, GL_FALSE, glm::value_ptr(projection));
//    glUniform3f(lightShader.location("lightPos"), lightPos.x, lightPos.y, lightPos.z);
//    glUniform3f(lightShader.location("lightColor"), lightColor.r, lightColor.g, lightColor.b);
//    glm::mat4 model;
//    model = glm::translate(model, lightPos);
//    model = glm::scale(model, glm::vec3(0.2f));
//    glUniformMatrix4fv(lightShader.location("model"), 1, GL_FALSE, glm::value_ptr(model));
//    glBindVertexArray(lightVaoID);
//    glDrawArrays(GL_TRIANGLES, 0, 36);
//    glBindVertexArray(0);
//    
//    glCheckError("lightDrawing");
}

////////////各种初始化//////////////////////
void shaderInit()
{
    modelShader.init();
    modelShader.attach(GL_VERTEX_SHADER, "Shaders/model.vert");
    modelShader.attach(GL_FRAGMENT_SHADER, "Shaders/model.frag");
    modelShader.link();
    
    lightShader.init();
    lightShader.attach(GL_VERTEX_SHADER, "Shaders/light.vert");
    lightShader.attach(GL_FRAGMENT_SHADER, "Shaders/light.frag");
    lightShader.link();
}

void vboInit()
{
    //物体VAO
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    //光照点VAO
    glGenVertexArrays(1, &lightVaoID);
    glBindVertexArray(lightVaoID);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

//    GLuint EBO;
//    glGenBuffers(1, &EBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void saveTextureToFile(GLenum target, GLuint tex, const char* filename)
{
    glBindTexture(target, tex);
    int width, height, depth;
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, &depth);
    
    height *= depth;
    unsigned char* pixels = new unsigned char[width * height * 4];
    glGetTexImage(target, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    //翻转图像（来自SOIL源码）
    int i, j;
    for (j = 0; j * 2 < height; ++j)
    {
        int index1 = j * width * 4;
        int index2 = (height - 1 - j) * width * 4;
        for (i = width * 4; i > 0; --i)
        {
            unsigned char temp = pixels[index1];
            pixels[index1] = pixels[index2];
            pixels[index2] = temp;
            ++index1;
            ++index2;
        }
    }
    
    SOIL_save_image(filename, SOIL_SAVE_TYPE_BMP, width, height, 4, pixels);
}

void loadTextureFromFile(const char* filename, GLuint tex)
{
    int width, height;
    unsigned char *data;
    data = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
    
    //翻转图像（来自SOIL源码）
    int i, j;
    for (j = 0; j * 2 < height; ++j)
    {
        int index1 = j * width * 4;
        int index2 = (height - 1 - j) * width * 4;
        for (i = width * 4; i > 0; --i)
        {
            unsigned char temp = data[index1];
            data[index1] = data[index2];
            data[index2] = temp;
            ++index1;
            ++index2;
        }
    }
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(data);
}

void textureInit()
{
    glGenTextures(2, texID);
    loadTextureFromFile("test.png", texID[0]);
    loadTextureFromFile("test1.png", texID[1]);
}

void errorCallback(int error, const char* description)
{
    std::cerr << description << std::endl;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (0 <= key && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
    
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
        lightSpot.turnOnOff();
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    testCamera.mouseCallback(xpos, ypos);
}

void glfwInitialize()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(resX, resY, "Simple example", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
//    glfwSwapInterval(100);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);
    
    glCheckError("glInit");
}

void glInfo()
{
    using namespace std;
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "OpenGL vendor: " << glGetString(GL_VENDOR) << endl;
    cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
}

int main(int argc, const char * argv[])
{
    glfwInitialize();
    glInfo();
    shaderInit();
    vboInit();
    textureInit();
    
    std::cerr << "==========绘制开始============" << std::endl;
    
    lightDirect.setLocation(glm::vec3(0.0, 5.0, 5.0));
    lightDirect.setDirection(glm::vec3(0.0, -5.0, -5.0));
    lightSpot.setDiffuse(glm::vec3(0.8));
    
    while (!glfwWindowShouldClose(window))
    {
        lastFrame = glfwGetTime();
        glDisplay();
        glfwSwapBuffers(window);
        glfwPollEvents();
        double newFrame = glfwGetTime();
        testCamera.calcFrame(newFrame - lastFrame);
        lastFrame = newFrame;
        testCamera.doMovement(keys[GLFW_KEY_W],
                              keys[GLFW_KEY_S],
                              keys[GLFW_KEY_A],
                              keys[GLFW_KEY_D]);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    
    return 0;
}
