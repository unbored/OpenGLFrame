#include "Shader.h"

Shader::Shader()
{
    program = 0;
    errorFlags = 0x0f;		//所有标记置1
    linkFlag = false;	//未链接
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::checkGLErrors(const char* desc) {
    GLenum e;
	while ((e = glGetError()) != GL_NO_ERROR)
		std::cerr << "OpenGL错误" << e << "，位于" << desc << std::endl;
}

GLuint Shader::createShader(GLenum type, const char* filename) //读入一个shader
{
    std::ifstream t;
    t.open(filename);      //打开文本文件
    if (!t.is_open())
    {
        std::cerr << "Shader错误：打开shader文件 \"" << filename << "\" 失败。" << std::endl;
        return 0;
    }
    std::string shaderSource((std::istreambuf_iterator<char>(t)),	//将文本文件整个读入string当中
                             std::istreambuf_iterator<char>());
    const char* shaderContent = shaderSource.c_str();	//得到char数组
    const GLint shaderLength = (GLuint)shaderSource.length();	//得到数组长度
    
    //创建shader
    GLuint shaderID;
    shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &shaderContent, &shaderLength);
    glCompileShader(shaderID);	//编译
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);	//获取编译状态
    if (!success)
    {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        std::cerr << "Shader错误：Shader \"" << filename << "\" 编译失败：\n" << infoLog << std::endl;
        return 0;
    }
    return shaderID;
}

bool Shader::init()
{
    program = glCreateProgram();
    return true;
}

bool Shader::attach(const GLenum type, const char* filename)
{
    GLuint shader;
    shader = createShader(type, filename);
    if (shader == 0)	//shader编译失败
    {
        errorFlags |= 0x01;	//编译标志位
        return false;
    }
    else
        errorFlags &= ~0x01;
    glAttachShader(program, shader);
    if (type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER)
        errorFlags ^= 0x04;	//vertex/fragment标志位，异或此位可将其取反
    return true;
}

bool Shader::link()
{
    if ((errorFlags & 0x01) != 0)	//编译错误
    {
        std::cerr << "Shader错误：有Shader编译失败。" << std::endl;
        return false;
    }
    if ((errorFlags & 0x04) == 0)	//vertex/fragment只编译了其中一个
    {
        std::cerr << "Shader错误：Vertex和Fragment shader必须同时挂接。" << std::endl;
        return false;
    }
    glLinkProgram(program);
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);	//获取编译状态
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader错误：Program链接失败：\n" << infoLog << std::endl;
        errorFlags |= 0x02;	//link标志位
        return false;
    }
    linkFlag = true;
    return true;
}

