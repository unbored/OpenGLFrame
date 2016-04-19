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
    //importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS);
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate   //三角化
                                             | aiProcess_GenNormals    //创建法线
                                             | aiProcess_FixInfacingNormals //翻转朝内的法线
                                             | aiProcess_JoinIdenticalVertices  //合并顶点
                                             //| aiProcess_PreTransformVertices   //坐标预变换（无动画）
                                             | aiProcess_GenUVCoords    //生成UV坐标
                                             | aiProcess_TransformUVCoords //转换UV坐标
                                             | aiProcess_OptimizeGraph  //优化无动画部分
                                             | aiProcess_OptimizeMeshes //减少网格数
                                             );
    
    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cerr << "错误::ASSIMP::" << importer.GetErrorString() << endl;
        return GL_FALSE;
    }
    
    std::size_t found = path.find_last_of("/\\");
    if (found == path.npos)
        directory = "";
    else
        directory = path.substr(0, found + 1); //纯路径

    processNode(scene->mRootNode, scene, glm::mat4(1.0));   //处理节点
    
    return GL_TRUE;
}

//处理节点的递归函数
void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 transform)
{
//    glm::mat4 transmat(node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
//                       node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
//                       node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
//                       node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4);
    glm::mat4 transmat(node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
                       node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
                       node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
                       node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4);
    
    transmat = transmat * transform;
    //transmat = glm::translate(transform, glm::vec3(100.0, 0.0, 0.0));

    //当前节点
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene, transmat));
    }
    //子节点
    for (int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, transmat);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transform)
{
    vector<Vertex> vertices;
    vector<GLuint> indices;
    Material mat;

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
//        if (mesh->mColors[0])
//        {
//            glm::vec4 color;
//            color.r = mesh->mColors[0][i].r;
//            color.g = mesh->mColors[0][i].g;
//            color.b = mesh->mColors[0][i].b;
//            color.a = mesh->mColors[0][i].a;
//            vertex.color = color;
//        }
//        else
//            vertex.color = glm::vec4(1.0);  //白色
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
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    //纹理
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    
    mat = loadMaterial(material);
    
    return Mesh(vertices, indices, mat, transform);
}

Material Model::loadMaterial(aiMaterial* mat)
{
    Material material;
    
    loadMatTex(mat, material, Ambient);
    loadMatTex(mat, material, Diffuse);
    loadMatTex(mat, material, Specular);
    
    float shininess;
    mat->Get(AI_MATKEY_SHININESS, shininess);
    material.shininess = shininess;
    
    return material;
}

void Model::loadMatTex(aiMaterial* mat, Material& targetMat, TextureType matType)
{
    aiTextureType aiType;
    GLuint *matTex;
    glm::vec3 *matColor;
    bool *texFlag;
    
    switch (matType)
    {
        case Ambient:
            aiType = aiTextureType_AMBIENT;
            matTex = &targetMat.ambientTex;
            matColor = &targetMat.ambientColor;
            texFlag = &targetMat.textured.x;
            break;
        case Diffuse:
            aiType = aiTextureType_DIFFUSE;
            matTex = &targetMat.diffuseTex;
            matColor = &targetMat.diffuseColor;
            texFlag = &targetMat.textured.y;
            break;
        case Specular:
            aiType = aiTextureType_SPECULAR;
            matTex = &targetMat.specularTex;
            matColor = &targetMat.specularColor;
            texFlag = &targetMat.textured.z;
            break;
        default:
            aiType = aiTextureType_DIFFUSE;
            matTex = &targetMat.diffuseTex;
            matColor = &targetMat.diffuseColor;
            texFlag = &targetMat.textured.y;
            break;
    }

    int texCount = mat->GetTextureCount(aiType);  //纹理数量
    if (texCount > 0)    //有纹理
    {
        aiString str;
        mat->GetTexture(aiType, 0, &str);
        string texPath = str.C_Str();
        //检查纹理是否已经读取过
        bool loadFlag = false;
        for (int j = 0; j < textureLoaded.size(); j++)
        {
            if (texPath == textureLoaded[j].path)
            {
                *matTex = textureLoaded[j].id;
                loadFlag = true;
                break;
            }
        }
        if (!loadFlag)  //新建纹理
        {
            Texture texture;
            string fullpath = directory + texPath;
            texture.id = textureFromFile(fullpath.c_str());
//            texture.type = Diffuse;
            texture.path = texPath;
            textureLoaded.push_back(texture);
            *matTex = texture.id;
        }
        *texFlag = true;
    }
    else    //无纹理，取颜色
    {
        aiColor3D color;
        switch (matType)
        {
            case Ambient:
                mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
                break;
            case Diffuse:
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                break;
            case Specular:
                mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
                break;
            default:
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                break;
        }
        matColor->r = color.r;
        matColor->g = color.g;
        matColor->b = color.b;
        *texFlag = false;
    }
}

GLuint Model::textureFromFile(const char* filename)
{
    FREE_IMAGE_FORMAT fif;
    FIBITMAP *bmpIn;
    fif = FreeImage_GetFileType(filename);
    if (fif == FIF_UNKNOWN)
        fif = FreeImage_GetFIFFromFilename(filename);
    bmpIn = FreeImage_Load(fif, filename);
    FIBITMAP *bmp = FreeImage_ConvertTo32Bits(bmpIn);
    FreeImage_Unload(bmpIn);
    
    BYTE* bits = FreeImage_GetBits(bmp);
    unsigned int width = FreeImage_GetWidth(bmp);
    unsigned int height = FreeImage_GetHeight(bmp);
    
    GLuint tex;
    float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    //SOIL_free_image_data(data);
    FreeImage_Unload(bmp);
    return tex;
}

void Model::draw(GLuint program)
{
    for (int i = 0; i < meshes.size(); i++)
        meshes[i].draw(program);
}