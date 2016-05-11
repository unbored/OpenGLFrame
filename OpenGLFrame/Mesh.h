//
//  Mesh.h
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/18.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#pragma once

#define MAX_BONES 100

#include <iostream>
#include <vector>
#include <string>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

struct Node
{
    std::string name;
    std::vector<unsigned int> meshIndices;
    glm::mat4 transform;
    std::vector<Node> childs;
};

struct TransKey
{
    double time;
    glm::mat4 transMat;
};

struct NodeAnim
{
    std::string name;
    std::vector<TransKey> position;
    std::vector<TransKey> rotate;
    std::vector<TransKey> scale;
};

struct Bone
{
    std::string name;
    glm::mat4 nodeMat;
    glm::mat4 offsetMat;
    NodeAnim anim;
};

//struct BoneWeight
//{
//	GLuint boneIndex;
//	GLfloat weight;
//	BoneWeight() { boneIndex = 0; weight = 0; };
//};

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
	//记录所关联的所有骨骼和权重
	glm::ivec4 boneIndices;
	glm::vec4 weights;
};

struct Material
{
    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    GLfloat   shininess;
    GLuint	  ambientTexed;
	GLuint	  diffuseTexed;
	GLuint	  specularTexed;
	GLuint	  heightTexed;
	GLuint	  normalTexed;
	GLuint	  alphaTexed;
    //GLfloat   bumpScale;
	//以下用于记录，不传进shader
	unsigned int id;
	GLuint UBO;
	GLuint ambientTex;
	GLuint diffuseTex;
	GLuint specularTex;
	GLuint bumpTex;
	GLuint alphaTex;
};

class Mesh
{
public:
    Mesh();
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Bone> bones, unsigned int matIndex);
    ~Mesh();
    
private:
    GLuint VAO, VBO, EBO;
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Bone> bones;
    static size_t MaxBones;
    Material material;
    glm::mat4 offsetMat;
	GLuint matID;
    GLuint boneUBO;
  
public:
	//void setupVertices(std::vector<Vertex> vertices, std::vector<GLuint> indices);
	//指定材质
	void setMaterial(Material material) { this->material = material; };
	//获取材质ID
	GLuint getMatID() { return matID; };
    //读取节点矩阵
    void getNodeMatrix(Node &node, glm::mat4 parentMat);
    //读取动画矩阵
    void getAnimMatrix(std::vector<NodeAnim> &channel);
    //更新动画矩阵
    void updateAnimMatrix(double tick);
    //创建GL资源
    GLboolean setupMesh();
	//销毁GL资源
	void destroy();
	//绘制
    void draw(GLuint program);
};