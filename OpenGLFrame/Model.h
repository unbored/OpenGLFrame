//
//  Model.h
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/18.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "GL/glew.h"
#include "FreeImage.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

struct Texture
{
	GLuint id;
	std::string path;
};

struct Animation
{
	std::string name;
	double duration;
	double ticksPerSec;
    double totalTicks;
	std::vector<NodeAnim> channels;
};

class Model
{
public:
    Model();
    Model(const char* filename);
    ~Model();
    
private:
	Node rootNode;
	glm::mat4 globalInverseTransform;
	std::vector<Mesh> meshes;
	std::string directory;
	std::string modelName;
	std::vector<Material> materialLoaded;
	std::vector<Texture> textureLoaded;
    bool isHeightTex;
    Animation anim;
    bool hasAnimation;

private:
	//处理节点数据
    void processNode(aiNode* node, Node &newNode, const aiScene* scene);
	//读取网格数据
	Mesh loadMesh(aiMesh* mesh, const aiScene* scene);
	//读取材质
    Material loadMaterial(aiMaterial* mat);
	//读取材质当中的纹理
    void loadMatTex(aiMaterial* mat, Material& targetMat, aiTextureType matType);
	//从文件读取纹理
    GLuint textureFromFile(const char* filename);
    Animation loadAnimation(aiAnimation* anim);
    //从动画中读取一帧的矩阵
    glm::mat4 getAnimMatrix(Animation &anim, std::string nodeName, double sec);
	//绘制节点的递归函数
//	void drawNode(GLuint program, Node &node, glm::mat4 transMat);
    void drawNode(GLuint program, Node &node, glm::mat4 transMat, double time = 0.0);
    
    static glm::mat4 aiMatToGlmMat(aiMatrix4x4 mat)
    {
        glm::mat4 result;
        for (int x = 0; x < 4; x++)
            for (int y = 0; y < 4; y++)
                result[x][y] = mat[y][x];
        return result;
    }
    
    static glm::mat3 aiMatToGlmMat(aiMatrix3x3 mat)
    {
        glm::mat3 result;
        for (int x = 0; x < 3; x++)
            for (int y = 0; y < 3; y++)
                result[x][y] = mat[y][x];
        return result;
    }
    
    static glm::vec3 aiVecToGlmVec(aiVector3D vec)
    {
        glm::vec3 result;
        for (int i = 0; i < 3; i++)
            result[i] = vec[i];
        return result;
    }

public:
	//加载模型
    GLboolean loadModel(const char* filename, bool isHeightMap = false);
	//处理GL数据
	void process();
	//销毁GL数据
	void destroy();
	//绘制模型
//	void draw(GLuint program, glm::mat4 transform = glm::mat4(1.0));
    void draw(GLuint program, glm::mat4 transform = glm::mat4(1.0), double time = 0.0);
};