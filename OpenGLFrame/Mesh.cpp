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
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, bitangent));
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
	//绑定所有纹理
	//漫射贴图
	if (material.ambientTexed)
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program, "ambientTex"), 0);
		glBindTexture(GL_TEXTURE_2D, material.ambientTex);
	}
	//散射贴图
	if (material.diffuseTexed)
	{
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(glGetUniformLocation(program, "diffuseTex"), 1);
		glBindTexture(GL_TEXTURE_2D, material.diffuseTex);
	}
	//反射贴图
	if (material.specularTexed)
	{
		glActiveTexture(GL_TEXTURE2);
		glUniform1i(glGetUniformLocation(program, "specularTex"), 2);
		glBindTexture(GL_TEXTURE_2D, material.specularTex);
	}
	//凹凸贴图
	if (material.heightTexed || material.normalTexed)
	{
		glActiveTexture(GL_TEXTURE3);
		glUniform1i(glGetUniformLocation(program, "bumpTex"), 3);
		glBindTexture(GL_TEXTURE_2D, material.bumpTex);
	}
	//透明贴图
	if (material.alphaTexed)
	{
		glActiveTexture(GL_TEXTURE4);
		glUniform1i(glGetUniformLocation(program, "alphaTex"), 4);
		glBindTexture(GL_TEXTURE_2D, material.alphaTex);
	}
	//取得UBO位置
	GLuint index = glGetUniformBlockIndex(program, "Material");
	glBindBufferBase(GL_UNIFORM_BUFFER, index, material.UBO);

    //内部变换矩阵
    glUniformMatrix4fv(glGetUniformLocation(program, "mesh"), 1, GL_FALSE, glm::value_ptr(transform));
    //绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}