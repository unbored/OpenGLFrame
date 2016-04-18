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
#include "SOIL/SOIL.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

using namespace std;

class Model
{
public:
    Model();
    Model(const char* filename);
    ~Model();
    
private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textureLoaded;
    
public:
    GLboolean loadModel(const char* filename);
    
private:
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType texType);
    GLuint textureFromFile(const char* filename);
    
};