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
GLboolean Model::loadModel(const char* filename, bool isHeightMap)
{
    string path = filename;    //包含文件名的路径
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate   //三角化
                                             | aiProcess_GenNormals    //创建法线
                                             | aiProcess_FixInfacingNormals //翻转朝内的法线
                                             | aiProcess_CalcTangentSpace   //计算切线空间（法线贴图用）
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

    isHeightTex = isHeightMap;
    processNode(scene->mRootNode, scene, glm::mat4(1.0));   //处理节点
    
    return GL_TRUE;
}

//处理节点的递归函数
void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 transform)
{
    glm::mat4 transmat(node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
                       node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
                       node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
                       node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4);
    
    transmat = transmat * transform;

    //当前节点
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene, transmat));
    }
    //子节点
	for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, transmat);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 transform)
{
    vector<Vertex> vertices;
    vector<GLuint> indices;

    //处理顶点
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
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
        //只关心第一个纹理坐标
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 textemp;
            textemp.x = mesh->mTextureCoords[0][i].x;
            textemp.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = textemp;
			//有纹理坐标才能算切线
			temp.x = mesh->mTangents[i].x;
			temp.y = mesh->mTangents[i].y;
			temp.z = mesh->mTangents[i].z;
			vertex.tangent = temp;
			temp.x = mesh->mBitangents[i].x;
			temp.y = mesh->mBitangents[i].y;
			temp.z = mesh->mBitangents[i].z;
			vertex.bitangent = temp;
        }
		else
		{
			vertex.texCoord = glm::vec2(0.0);
			vertex.tangent = glm::vec3(1.0, 0.0, 0.0);
			vertex.bitangent = glm::vec3(0.0, 1.0, 0.0);
		}
        
        vertices.push_back(vertex);
    }
    
    //索引
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    
    //材质
	Material mat;
	bool loadFlag = false;
	for (unsigned int i = 0; i < materialLoaded.size(); i++)
	{
		if (mesh->mMaterialIndex == materialLoaded[i].id)	//材质已经读取过
		{
			mat = materialLoaded[i];
			loadFlag = true;
			break;
		}
	}
	if (!loadFlag)	//材质没有读取过
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		mat = loadMaterial(material);
		mat.id = mesh->mMaterialIndex;
		materialLoaded.push_back(mat);
	}
    
    return Mesh(vertices, indices, mat, transform);
}

Material Model::loadMaterial(aiMaterial* mat)
{
    Material material;
    
    loadMatTex(mat, material, aiTextureType_AMBIENT);
    loadMatTex(mat, material, aiTextureType_DIFFUSE);
    loadMatTex(mat, material, aiTextureType_SPECULAR);
    loadMatTex(mat, material, aiTextureType_HEIGHT);
    loadMatTex(mat, material, aiTextureType_NORMALS);
    loadMatTex(mat, material, aiTextureType_OPACITY);
    
    float shininess;//, bumpScale;
    mat->Get(AI_MATKEY_SHININESS, shininess);
    material.shininess = shininess;
    
    if (isHeightTex)    //高度贴图，强制用高度贴图算法
        material.normalTexed = GL_FALSE;
    else if (material.heightTexed)  //否则强制用法线贴图算法
        material.normalTexed = GL_TRUE;

	//新建一个UBO
	GLuint UBO;
	glGenBuffers(1, &UBO);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	//缓冲区尺寸：vec4 * 3 + float * 1 + uint * 6，
	glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, offsetof(Material,id), &material);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	material.UBO = UBO;
	return material;
}

void Model::loadMatTex(aiMaterial* mat, Material& targetMat, aiTextureType matType)
{
    GLuint *matTex;
    glm::vec4 *matColor;
    GLuint *texFlag;
    
    switch (matType)
    {
        case aiTextureType_AMBIENT:
            matTex = &targetMat.ambientTex;
            matColor = &targetMat.ambientColor;
            texFlag = &targetMat.ambientTexed;
            break;
        case aiTextureType_DIFFUSE:
            matTex = &targetMat.diffuseTex;
            matColor = &targetMat.diffuseColor;
            texFlag = &targetMat.diffuseTexed;
            break;
        case aiTextureType_SPECULAR:
            matTex = &targetMat.specularTex;
            matColor = &targetMat.specularColor;
            texFlag = &targetMat.specularTexed;
            break;
        case aiTextureType_NORMALS:
            matTex = &targetMat.bumpTex;
            matColor = &targetMat.diffuseColor;
            texFlag = &targetMat.normalTexed;
            break;
        case aiTextureType_HEIGHT:
            matTex = &targetMat.bumpTex;
            matColor = &targetMat.diffuseColor;
            texFlag = &targetMat.heightTexed;
            break;
        case aiTextureType_OPACITY:
            matTex = &targetMat.alphaTex;
            matColor = &targetMat.diffuseColor;
            texFlag = &targetMat.alphaTexed;
            break;
        default:
            matTex = &targetMat.diffuseTex;
            matColor = &targetMat.diffuseColor;
            texFlag = &targetMat.diffuseTexed;
            break;
    }

    int texCount = mat->GetTextureCount(matType);  //纹理数量
    if (texCount > 0)    //有纹理
    {
        aiString str;
        mat->GetTexture(matType, 0, &str);
        string texPath = str.C_Str();
        //检查纹理是否已经读取过
        bool loadFlag = false;
		for (unsigned int j = 0; j < textureLoaded.size(); j++)
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
        *texFlag = GL_TRUE;
    }
    else    //无纹理，取颜色
    {
        aiColor3D color;
        switch (matType)
        {
            case aiTextureType_AMBIENT:
                mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
                break;
            case aiTextureType_DIFFUSE:
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                break;
            case aiTextureType_SPECULAR:
                mat->Get(AI_MATKEY_COLOR_SPECULAR, color);
                break;
            default:
                mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
                break;
        }
        matColor->r = color.r;
        matColor->g = color.g;
        matColor->b = color.b;
		matColor->a = 1.0f;
        *texFlag = GL_FALSE;
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
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    FreeImage_Unload(bmp);
    return tex;
}

void Model::draw(GLuint program)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].draw(program);
}