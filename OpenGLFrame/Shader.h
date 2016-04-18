#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <cstdlib>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader
{
public:
    Shader();
    ~Shader();
    
private:
    GLuint program;
    char errorFlags;	//用于标记各种错误
    bool linkFlag;
    
private:
    void checkGLErrors(const char* desc);	//检查并输出OpenGL错误
    void checkShaderErrors();	//检查Shader生成过程中的错误
    GLuint createShader(const GLenum type, const char* filename);	//通过文件创建Shader
    
public:
    bool init();
    bool attach(const GLenum type, const char* filename);	//通过文件加载Shader
    bool link();	//根据已经绑好的shader来链接程序
    GLuint getProgram() { return program; };	//使用program
    void use() { glUseProgram(program); };
    GLint location(const char *name) { return glGetUniformLocation(program, name); };
    
    void setUniform(const char *name, const GLint value)
    {
        glUniform1i(glGetUniformLocation(program, name), value);
    };
    void setUniform(const char *name, const GLsizei size, const GLint value[])
    {
        glUniform1iv(glGetUniformLocation(program, name), size, value);
    }
    //    void setUniform(const char *name, glm::ivec2 value);
    //    void setUniform(const char *name, glm::ivec3 value);
    //    void setUniform(const char *name, glm::ivec4 value);
    void setUniform(const char *name, const GLfloat value)
    {
        glUniform1f(glGetUniformLocation(program, name), value);
    };
    void setUniform(const char *name, const glm::vec2 value)
    {
        glUniform2f(glGetUniformLocation(program, name), value.x, value.y);
    };
    void setUniform(const char *name, const glm::vec3 value)
    {
        glUniform3f(glGetUniformLocation(program, name), value.x, value.y, value.z);
    };
    void setUniform(const char *name, const glm::vec4 value)
    {
        glUniform4f(glGetUniformLocation(program, name), value.w, value.x, value.y, value.z);
    };
    void setUniform(const char *name, const GLsizei size, const GLfloat *value)
    {
        glUniform1fv(glGetUniformLocation(program, name), size, value);
    };
    void setUniform(const char *name, const GLsizei size, const glm::vec2 *value)
    {
        glUniform2fv(glGetUniformLocation(program, name), size * 2, glm::value_ptr(value[0]));
    };
    void setUniform(const char *name, const GLsizei size, const glm::vec3 *value)
    {
        glUniform3fv(glGetUniformLocation(program, name), size * 3, glm::value_ptr(value[0]));
    };
    void setUniform(const char *name, const GLsizei size, const glm::vec4 *value)
    {
        glUniform4fv(glGetUniformLocation(program, name), size * 4, glm::value_ptr(value[0]));
    };
    void setUniform(const char *name, const GLsizei size, GLboolean transpose, const glm::mat2 *value)
    {
        glUniformMatrix2fv(glGetUniformLocation(program, name), size, transpose, glm::value_ptr(value[0]));
    };
    void setUniform(const char *name, const GLsizei size, GLboolean transpose, const glm::mat3 *value)
    {
        glUniformMatrix3fv(glGetUniformLocation(program, name), size, transpose, glm::value_ptr(value[0]));
    };
    void setUniform(const char *name, const GLsizei size, GLboolean transpose, const glm::mat4 *value)
    {
        glUniformMatrix4fv(glGetUniformLocation(program, name), size, transpose, glm::value_ptr(value[0]));
    };
};

