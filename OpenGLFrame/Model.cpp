//
//  Model.cpp
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/18.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#include "Model.h"

Model::Model()
{
    
}

Model::Model(const char* filename)
{
    loadModel(filename);
}

Model::~Model()
{
    
}

//读取模型
GLboolean Model::loadModel(const char* filename)
{
    string path = filename;    //包含文件名的路径
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);   //三角化，翻转纹理坐标，创建法线
    
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cerr << "错误::ASSIMP::" << importer.GetErrorString() << endl;
        return GL_FALSE;
    }
    
    directory = path.substr(0, path.find_last_of('/')); //纯路径

    processNode(scene->mRootNode, scene);   //处理节点
    
    return GL_TRUE;
}

//处理节点的递归函数
void Model::processNode(aiNode* node, const aiScene* scene)
{
    //当前节点
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
    }
    //子节点
    for (int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    //处理顶点
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 temp;
        temp.x = mesh->mVertices[i].x;
        temp.y = mesh->mVertices[i].y;
        temp.z = mesh->mVertices[i].z;
        vertex.position = temp;
        temp.x = mesh->mNormals[i].x;
        temp.y = mesh->mNormals[i].y;
        temp.z = mesh->mNormals[i].z;
        vertex.normal = temp;
        //颜色
        if (mesh->mColors[0])
        {
            glm::vec4 color;
            color.r = mesh->mColors[0][i].r;
            color.g = mesh->mColors[0][i].g;
            color.b = mesh->mColors[0][i].b;
            color.a = mesh->mColors[0][i].a;
            vertex.color = color;
        }
        else
            vertex.color = glm::vec4(1.0);  //白色
        //只关心第一个纹理坐标
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 textemp;
            textemp.x = mesh->mTextureCoords[0][i].x;
            textemp.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = textemp;
        }
        else
            vertex.texCoord = glm::vec2(0.0);
        
        vertices.push_back(vertex);
    }
    
    //索引
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    //纹理
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, Diffuse);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, Specular);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    
    return Mesh(vertices, textures, indices);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType texType)
{
    vector<Texture> textures;
    for(int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        string texPath = str.C_Str();
        
        //检查纹理是否已经读取过
        bool loadFlag = false;
        for (int j = 0; j < textureLoaded.size(); j++)
        {
            if (texPath == textureLoaded[j].path)
            {
                loadFlag = true;
                break;
            }
        }
        if (!loadFlag)
        {
            Texture texture;
            string fullpath = directory + texPath;
            texture.id = textureFromFile(fullpath.c_str());
            texture.type = texType;
            texture.path = texPath;
            textures.push_back(texture);
        }
    }
    return textures;
}

GLuint Model::textureFromFile(const char* filename)
{
    GLuint tex;
    int width, height;
    unsigned char *data;
    data = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
    
    //翻转图像（来自SOIL源码）
//    int i, j;
//    for (j = 0; j * 2 < height; ++j)
//    {
//        int index1 = j * width * 4;
//        int index2 = (height - 1 - j) * width * 4;
//        for (i = width * 4; i > 0; --i)
//        {
//            unsigned char temp = data[index1];
//            data[index1] = data[index2];
//            data[index2] = temp;
//            ++index1;
//            ++index2;
//        }
//    }
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    SOIL_free_image_data(data);
    return tex;
}