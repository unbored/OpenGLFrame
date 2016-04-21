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
#include "GL/glew.h"
#include "FreeImage.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

using namespace std;

struct Texture
{
    GLuint id;
//    TextureType type;
    string path;
};

class Model
{
public:
    Model();
    Model(const char* filename);
    ~Model();
    
private:
    vector<Mesh> meshes;
    string directory;
	vector<Material> materialLoaded;
    vector<Texture> textureLoaded;
    bool isHeightTex;
    
public:
    GLboolean loadModel(const char* filename, bool isHeightMap = false);
    void draw(GLuint program);
    
private:
    void processNode(aiNode* node, const aiScene* scene, glm::mat4 transform);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transform);
    Material loadMaterial(aiMaterial* mat);
    void loadMatTex(aiMaterial* mat, Material& targetMat, aiTextureType matType);
    GLuint textureFromFile(const char* filename);
};