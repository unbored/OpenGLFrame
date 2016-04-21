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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"

/////////////窗口/////////////////////
const GLint resX = 800, resY = 600;
GLFWwindow* window;

/////////////相机///////////////////////
Camera testCamera;

////////////Shader//////////////////
Shader modelShader;

/////////////模型////////////////////

Model testModel;

//////////////////纹理//////////////////


//////////////////光照////////////////////

Light lightDirect(Directional), lightSpot(Spot);

/////////////////键盘//////////////////
bool keys[1024];

////////////////帧率//////////////////
double lastFrame;

/////////////程序开始///////////////////////

//检查GL错误
void glCheckError(const char* position)
{
    GLenum e;
    while ((e = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL 错误：" << e << " 位于 " << position << std::endl;
    }
}

//主显示函数
void glDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, resX, resY);
    
    //视图、投影、模型矩阵
    glm::mat4 view;
    view = testCamera.viewMatrix();
    glm::mat4 projection;
    projection = testCamera.projMatrix();
    glm::mat4 model;
    model = glm::scale(model, glm::vec3(0.1));
    
    //=======绘制模型============
    modelShader.use();
    modelShader.setUniform("view", 1, GL_FALSE, &view);
    modelShader.setUniform("projection", 1, GL_FALSE, &projection);
    modelShader.setUniform("model", 1, GL_FALSE, &model);
    
    //传入相机位置
    glm::vec3 cameraPos = testCamera.position();
    modelShader.setUniform("viewPos", cameraPos);

    //平行光
    lightDirect.setUniform(modelShader.getProgram(), "lightDirect");
    //聚光灯方位与相机一致
    lightSpot.setLocation(cameraPos);
    lightSpot.setDirection(testCamera.getFront());
    lightSpot.setUniform(modelShader.getProgram(), "lightSpot");

    testModel.draw(modelShader.getProgram());
    
    glCheckError("model");
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
    glGetTexImage(target, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
    
    int bpp = 32;
    int pitch = ((((bpp * width) + 31) / 32) * 4);
    
    FIBITMAP *bmp = FreeImage_ConvertFromRawBits(pixels, width, height, pitch, bpp, 8, 8, 8);
    FreeImage_Save(FIF_PNG, bmp, filename);
    FreeImage_Unload(bmp);
    delete [] pixels;
}

GLuint loadTextureFromFile(const char* filename)
{
    FREE_IMAGE_FORMAT fif;
    FIBITMAP *bmpIn;
    fif = FreeImage_GetFileType(filename);
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename);
    bmpIn = FreeImage_Load(fif, filename);
    FIBITMAP *bmp = FreeImage_ConvertTo32Bits(bmpIn);
    FreeImage_Unload(bmpIn);
    
    BYTE* bits = FreeImage_GetBits(bmp);
    unsigned int width = FreeImage_GetWidth(bmp);
    unsigned int height = FreeImage_GetHeight(bmp);
    
    GLuint tex;
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    FreeImage_Unload(bmp);
    return tex;
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

////////////各种初始化//////////////////////

//Shader初始化
void shaderInit()
{
    modelShader.init();
    modelShader.attach(GL_VERTEX_SHADER, "Shaders/model.vert");
    modelShader.attach(GL_FRAGMENT_SHADER, "Shaders/model.frag");
    modelShader.link();
}

//glfw初始化
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
        std::cout << "创建GLFW窗口失败" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
//    glfwSwapInterval(100);
    
    glewExperimental = GL_TRUE; //必须打开
    if (glewInit() != GLEW_OK)
    {
        std::cout << "初始化GLEW失败" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glCheckError("glInit");
}

void glInfo()
{
    using namespace std;
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
}

int main(int argc, const char * argv[])
{
    glfwInitialize();
    glInfo();
    FreeImage_Initialise();
    shaderInit();
    
    testModel.loadModel("Models/alcohol/alcohol.obj", true);
    
    lightDirect.setLocation(glm::vec3(0.0, 5.0, 5.0));
    lightDirect.setDirection(glm::vec3(0.0, -5.0, -5.0));
    lightSpot.setAmbient(glm::vec3(0.1));
    lightSpot.turnOnOff(false);
    
    testCamera.setAspect((GLfloat)resX / resY);
    testCamera.setPos(glm::vec3(0.0, 0.0, 10.0));
    
    glClearColor(0.2, 0.2, 0.2, 1.0);
    std::cerr << "==========绘制开始============" << std::endl;
    
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
    
    FreeImage_DeInitialise();
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
    
    return 0;
}
