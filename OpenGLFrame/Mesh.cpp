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
    //颜色
//    glEnableVertexAttribArray(3);
//    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
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
    //标注漫射、散射和反射是否有纹理
    glUniform3i(glGetUniformLocation(program, "material.textured"),
                material.textured.x, material.textured.y, material.textured.z);
    //漫射贴图
    if (material.textured.x)
    {
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(program, "material.ambientTex"), 0);
        glBindTexture(GL_TEXTURE_2D, material.ambientTex);
    }
    else
        glUniform3f(glGetUniformLocation(program, "material.ambientColor"),
                    material.ambientColor.r, material.ambientColor.g, material.ambientColor.b);
    //散射贴图
    if (material.textured.y)
    {
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(program, "material.diffuseTex"), 1);
        glBindTexture(GL_TEXTURE_2D, material.diffuseTex);
    }
    else
        glUniform3f(glGetUniformLocation(program, "material.diffuseColor"),
                    material.diffuseColor.r, material.diffuseColor.g, material.diffuseColor.b);
    //反射贴图
    if (material.textured.z)
    {
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(program, "material.specularTex"), 2);
        glBindTexture(GL_TEXTURE_2D, material.specularTex);
    }
    else
        glUniform3f(glGetUniformLocation(program, "material.specularColor"),
                    material.specularColor.r, material.specularColor.g, material.specularColor.b);
    //高光大小
    glUniform1f(glGetUniformLocation(program, "material.shininess"), material.shininess);
    
    glUniformMatrix4fv(glGetUniformLocation(program, "mesh"), 1, GL_FALSE, glm::value_ptr(transform));
    //绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}