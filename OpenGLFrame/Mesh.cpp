//
//  Mesh.cpp
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/18.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#include "Mesh.h"

Mesh::Mesh()
{
    
}

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, Material material, glm::mat4 transform)
{
    setupVertices(vertices, indices, material);
    this->transform = transform;
}

Mesh::~Mesh()
{
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
//    glDeleteVertexArrays(1, &VAO);
}

void Mesh::setupVertices(vector<Vertex> vertices, vector<GLuint> indices, Material material)
{
    this->vertices = vertices;
    this->indices = indices;
    this->material = material;
    setupMesh();
}

GLboolean Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
    
    //顶点
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    //法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
    //纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
    //切线坐标
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tangent));
    //副切线坐标
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitengent));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    GLenum e;
    while ((e = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error " << e << " at " << "VAOSetup" << std::endl;
    }
    return GL_TRUE;
}

void Mesh::draw(GLuint program)
{
    //标注漫射、散射和反射等是否有纹理
    glUniform1i(glGetUniformLocation(program, "material.ambientTexed"), material.ambientTexed);
    glUniform1i(glGetUniformLocation(program, "material.diffuseTexed"), material.diffuseTexed);
    glUniform1i(glGetUniformLocation(program, "material.specularTexed"), material.specularTexed);
    glUniform1i(glGetUniformLocation(program, "material.heightTexed"), material.heightTexed);
    glUniform1i(glGetUniformLocation(program, "material.normalTexed"), material.normalTexed);
    glUniform1i(glGetUniformLocation(program, "material.alphaTexed"), material.alphaTexed);
    //漫射贴图
    if (material.ambientTexed)
    {
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(program, "material.ambientTex"), 0);
        glBindTexture(GL_TEXTURE_2D, material.ambientTex);
    }
    else
        glUniform3f(glGetUniformLocation(program, "material.ambientColor"),
                    material.ambientColor.r, material.ambientColor.g, material.ambientColor.b);
    //散射贴图
    if (material.diffuseTexed)
    {
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(program, "material.diffuseTex"), 1);
        glBindTexture(GL_TEXTURE_2D, material.diffuseTex);
    }
    else
        glUniform3f(glGetUniformLocation(program, "material.diffuseColor"),
                    material.diffuseColor.r, material.diffuseColor.g, material.diffuseColor.b);
    //反射贴图
    if (material.specularTexed)
    {
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(program, "material.specularTex"), 2);
        glBindTexture(GL_TEXTURE_2D, material.specularTex);
    }
    else
        glUniform3f(glGetUniformLocation(program, "material.specularColor"),
                    material.specularColor.r, material.specularColor.g, material.specularColor.b);
    //凹凸贴图
    if (material.heightTexed || material.normalTexed)
    {
        glActiveTexture(GL_TEXTURE3);
        glUniform1i(glGetUniformLocation(program, "material.bumpTex"), 3);
        glBindTexture(GL_TEXTURE_2D, material.bumpTex);
    }
    //透明贴图
    if (material.alphaTexed)
    {
        glActiveTexture(GL_TEXTURE4);
        glUniform1i(glGetUniformLocation(program, "material.alphaTex"), 4);
        glBindTexture(GL_TEXTURE_2D, material.alphaTex);
    }
    //高光大小
    glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);
    //法线贴图缩放
//    glUniform1f(glGetUniformLocation(program, "material.bumpScale"), material.bumpScale);
    //内部变换矩阵
    glUniformMatrix4fv(glGetUniformLocation(program, "mesh"), 1, GL_FALSE, glm::value_ptr(transform));
    //绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}