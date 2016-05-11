//
//  Mesh.cpp
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/18.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#include "Mesh.h"

size_t Mesh::MaxBones = 0;

Mesh::Mesh()
{
    
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Bone> bones, unsigned int matIndex)
{
    //setupVertices(vertices, indices);
	this->vertices = vertices;
	this->indices = indices;
	this->bones = bones;
	this->matID = matIndex;
}

Mesh::~Mesh()
{
//    glDeleteBuffers(1, &VBO);
//    glDeleteBuffers(1, &EBO);
//    glDeleteVertexArrays(1, &VAO);
}

//void Mesh::setupVertices(std::vector<Vertex> vertices, std::vector<GLuint> indices)
//{
//    this->vertices = vertices;
//    this->indices = indices;
//    //setupMesh();
//}

void Mesh::getNodeMatrix(Node &node, glm::mat4 parentMat)
{
    glm::mat4 transMat = parentMat * node.transform;
    offsetMat = transMat;
    for (int i = 0; i < bones.size(); i++)
    {
        if (node.name == bones[i].name)
        {
            bones[i].nodeMat = transMat;
            break;
        }
    }
    for (int i = 0; i < node.childs.size(); i++)
    {
        getNodeMatrix(node.childs[i], transMat);
    }
}

void Mesh::getAnimMatrix(std::vector<NodeAnim> &channel)
{
    for (int boneIndex = 0; boneIndex < bones.size(); boneIndex++)
    {
        for (int channelIndex = 0; channelIndex < channel.size(); channelIndex++)
        {
            if (bones[boneIndex].name == channel[channelIndex].name)
            {
                bones[boneIndex].anim = channel[channelIndex];
                break;
            }
        }
    }
}

void Mesh::updateAnimMatrix(double tick)
{
    std::vector<glm::mat4> resultMatrix;
    for (int boneIndex = 0; boneIndex < bones.size(); boneIndex++)
    {
        NodeAnim &node = bones[boneIndex].anim;
        
        glm::mat4 nodeMat = bones[boneIndex].nodeMat;
        glm::mat4 offset = bones[boneIndex].offsetMat;
        glm::mat4 trans(1.0);
        glm::mat4 rotate(1.0);
        glm::mat4 scale(1.0);
        
        for (int i = 0; i < node.position.size(); i++)
        {
            if (tick <= node.position[i].time)
            {
                trans = node.position[i].transMat;
                break;
            }
        }
        for (int i = 0; i < node.rotate.size(); i++)
        {
            if (tick <= node.rotate[i].time)
            {
                rotate = node.rotate[i].transMat;
                break;
            }
        }
        for (int i = 0; i < node.scale.size(); i++)
        {
            if (tick <= node.scale[i].time)
            {
                scale = node.scale[i].transMat;
                break;
            }
        }
        glm:: mat4 result = nodeMat * offset * (trans * rotate * scale);
        resultMatrix.push_back(result);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, boneUBO);
    //缓冲区尺寸：每个mat4占64字节
    glBufferSubData(GL_UNIFORM_BUFFER, 0, resultMatrix.size() * 64, &resultMatrix[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
	//骨骼编号，注意是IPointer
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, boneIndices)));
	//骨骼权重
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(offsetof(Vertex, weights)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    if (MaxBones < bones.size())
        MaxBones = bones.size();
    
    if (MaxBones > MAX_BONES)
        std::cerr << "错误：实际读取骨骼数 " << MaxBones << " 大于设定上限 " << MAX_BONES << std::endl;
//    
//    for (size_t i = 0; i < bones.size(); i++)
//    {
//        offsetMats.push_back(bones[i].offsetMat);
//    }
    
    //新建一个存放骨骼偏移矩阵的UBO
    glGenBuffers(1, &boneUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, boneUBO);
    //缓冲区尺寸：每个mat4占64字节
    glBufferData(GL_UNIFORM_BUFFER, MAX_BONES * 64, NULL, GL_STATIC_DRAW);
//    glBufferSubData(GL_UNIFORM_BUFFER, 0, offsetMats.size() * 64, &offsetMats[0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    GLenum e;
    while ((e = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL错误: " << e << " 位于 " << "VAOSetup" << std::endl;
    }
    return GL_TRUE;
}

void Mesh::destroy()
{
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
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
	glUniformBlockBinding(program, index, 0);	//0号位置
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, material.UBO);
    
    //取得boneUBO位置
    GLuint index1 = glGetUniformBlockIndex(program, "Bone");
    glUniformBlockBinding(program, index1, 1);	//0号位置
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, boneUBO);
//    glUniformMatrix4fv(glGetUniformLocation(program, "bones"), MAX_BONES, GL_FALSE, glm::value_ptr(offsetMats[0]));
    
    //绘制
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}