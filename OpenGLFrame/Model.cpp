//
//  Model.cpp
//  OpenGLFrame
//
//  Created by 钟擎 on 16/4/18.
//  Copyright © 2016年 钟擎. All rights reserved.
//

#include "Model.h"
using namespace std;

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
	std::size_t found = path.find_last_of("/\\");

	if (found == path.npos)
	{
		modelName = filename;
		directory = "";
	}
	else
	{
		modelName = path.substr(found + 1);
		directory = path.substr(0, found + 1); //纯路径
	}
	cerr << "当前目录：" << directory << endl;
	cerr << "开始读取模型：" << modelName << endl;
    Assimp::Importer importer;
    //importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS);
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate   //三角化
                                             | aiProcess_GenNormals    //创建法线
                                             | aiProcess_FixInfacingNormals //翻转朝内的法线
                                             | aiProcess_CalcTangentSpace   //计算切线空间（法线贴图用）
                                             | aiProcess_JoinIdenticalVertices  //合并顶点
                                             //| aiProcess_PreTransformVertices   //坐标预变换（无动画）
                                             | aiProcess_GenUVCoords    //生成UV坐标
                                             | aiProcess_TransformUVCoords //转换UV坐标
//                                             | aiProcess_OptimizeGraph  //优化无动画部分
//                                             | aiProcess_OptimizeMeshes //减少网格数
                                             );

    if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cerr << "错误::ASSIMP::" << importer.GetErrorString() << endl;
        return GL_FALSE;
    }
    
	cerr << "处理模型节点..." << endl;
    isHeightTex = isHeightMap;

    processNode(scene->mRootNode, rootNode, scene);   //处理节点

    globalInverseTransform = aiMatToGlmMat(scene->mRootNode->mTransformation);
	globalInverseTransform = glm::inverse(globalInverseTransform);

	cerr << "读取模型网格..." << endl;
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		meshes.push_back(loadMesh(scene->mMeshes[i], scene));
	}

	cerr << "读取模型材质..." << endl;
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		materialLoaded.push_back(loadMaterial(scene->mMaterials[i]));
	}

    if (scene->HasAnimations())
    {
        hasAnimation = true;
        cerr << "读取动画..." << endl;
        anim = loadAnimation(scene->mAnimations[0]);
        for (int i = 0; i < meshes.size(); i++)
        {
            meshes[i].getAnimMatrix(anim.channels);
            meshes[i].getNodeMatrix(rootNode, glm::mat4(1.0));
        }
    }
    else
        hasAnimation = false;
    
	std::cerr << "共读取：网格数：" << meshes.size()
		<< "；材质数：" << materialLoaded.size()
		<< "；纹理数：" << textureLoaded.size() << std::endl;

	
    return GL_TRUE;
}

//处理节点的递归函数
void Model::processNode(aiNode* node, Node &newNode, const aiScene* scene)
{
    newNode.name = node->mName.C_Str();
    
    glm::mat4 transmat = aiMatToGlmMat(node->mTransformation);

	newNode.transform = transmat;
    //当前节点
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
		newNode.meshIndices.push_back(node->mMeshes[i]);
        //meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene, transmat));
    }
    //子节点
	for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
		Node childNode;
		processNode(node->mChildren[i], childNode, scene);
        newNode.childs.push_back(childNode);
    }
}

//读取网格，包含顶点、索引和材质
Mesh Model::loadMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Bone> bones;

	//处理顶点
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position = aiVecToGlmVec(mesh->mVertices[i]);
		vertex.normal = aiVecToGlmVec(mesh->mNormals[i]);
		//只关心第一个纹理坐标
		if (mesh->mTextureCoords[0])
		{
            vertex.texCoord = glm::vec2(aiVecToGlmVec(mesh->mTextureCoords[0][i]));
			//有纹理坐标才能算切线
			vertex.tangent = aiVecToGlmVec(mesh->mTangents[i]);
			vertex.bitangent = aiVecToGlmVec(mesh->mBitangents[i]);
		}
		else
		{
			vertex.texCoord = glm::vec2(0.0);
			vertex.tangent = glm::vec3(1.0, 0.0, 0.0);
			vertex.bitangent = glm::vec3(0.0, 1.0, 0.0);
		}

		//遍历骨骼取权重
		int boneCount = 0;
		for (unsigned int iBone = 0; iBone < mesh->mNumBones; iBone++)
		for (unsigned int iWeight = 0; iWeight < mesh->mBones[iBone]->mNumWeights; iWeight++)
		if (mesh->mBones[iBone]->mWeights[iWeight].mVertexId == i)	//是要找的顶点
		{
			if (boneCount >= 4)
				std::cerr << "警告：每个顶点关联的骨骼数超出 4 个的上限" << std::endl;

			vertex.boneIndices[boneCount] = iBone;
			vertex.weights[boneCount] = mesh->mBones[iBone]->mWeights[iWeight].mWeight;
			boneCount++;
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

	//骨骼
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		Bone bone;
		bone.name = mesh->mBones[i]->mName.C_Str();	//骨骼名
		//复制矩阵
        bone.offsetMat = aiMatToGlmMat(mesh->mBones[i]->mOffsetMatrix);
        bone.nodeMat = glm::mat4(1.0);
		bones.push_back(bone);
	}
	
	unsigned int matIndex = mesh->mMaterialIndex;

	return Mesh(vertices, indices, bones, matIndex);
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
	//GLuint UBO;
	//glGenBuffers(1, &UBO);
	//glBindBuffer(GL_UNIFORM_BUFFER, UBO);
	////缓冲区尺寸：vec4 * 3 + float * 1 + uint * 6，
	//glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_STATIC_DRAW);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, offsetof(Material,id), &material);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//material.UBO = UBO;
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
		texPath = directory + texPath;
        //检查纹理是否已经读取过
        bool loadFlag = false;
		for (unsigned int j = 0; j < textureLoaded.size(); j++)
        {
            if (texPath == textureLoaded[j].path)
            {
                *matTex = j;
                loadFlag = true;
                break;
            }
        }
        if (!loadFlag)  //新建纹理
        {
			Texture texture;
            //texture.id = textureFromFile(fullpath.c_str());
//            texture.type = Diffuse;
            texture.path = texPath;
            textureLoaded.push_back(texture);
            *matTex = (GLuint)textureLoaded.size() - 1;
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
		*matTex = 0;
        *texFlag = GL_FALSE;
    }
}

Animation Model::loadAnimation(aiAnimation *aiAnim)
{
    Animation anim;
    anim.name = aiAnim->mName.C_Str();
    anim.duration = aiAnim->mDuration;
    anim.ticksPerSec = aiAnim->mTicksPerSecond;
    for (int i = 0; i < aiAnim->mNumChannels; i++)
    {
        aiNodeAnim *source = aiAnim->mChannels[i];
        
        NodeAnim nodeAnim;
        nodeAnim.name = source->mNodeName.C_Str();
        
        for (int i = 0; i < source->mNumPositionKeys; i++)
        {
            TransKey key;
            key.time = source->mPositionKeys[i].mTime;
            key.transMat = glm::translate(glm::mat4(1.0), aiVecToGlmVec(source->mPositionKeys[i].mValue));
            nodeAnim.position.push_back(key);
        }
        for (int i = 0; i < source->mNumRotationKeys; i++)
        {
            TransKey key;
            key.time = source->mRotationKeys[i].mTime;
            key.transMat = glm::mat4(aiMatToGlmMat(source->mRotationKeys[i].mValue.GetMatrix()));
            nodeAnim.rotate.push_back(key);
        }
        for (int i = 0; i < source->mNumScalingKeys; i++)
        {
            TransKey key;
            key.time = source->mScalingKeys[i].mTime;
            key.transMat = glm::scale(glm::mat4(1.0), aiVecToGlmVec(source->mScalingKeys[i].mValue));
            nodeAnim.scale.push_back(key);
        }
        
        anim.channels.push_back(nodeAnim);
    }
    anim.totalTicks = 0;
    return anim;
}

glm::mat4 Model::getAnimMatrix(Animation &anim, std::string nodeName, double sec)
{
    bool searchNodeName = false;
    int channelIndex;
    for (channelIndex = 0; channelIndex < anim.channels.size(); channelIndex++)
    {
        if (nodeName == anim.channels[channelIndex].name)
        {
            searchNodeName = true;
            break;
        }
    }
    if (!searchNodeName)
        return glm::mat4(1.0);
    
    NodeAnim &node = anim.channels[channelIndex];
    double currentTick = anim.ticksPerSec * sec;
    anim.totalTicks = std::fmod(anim.totalTicks + currentTick, anim.duration);  //求得不大于duration的结果
    
    glm::mat4 result(1.0);
    double tempTicks = anim.totalTicks;
    for (int i = 0; i < node.position.size(); i++)
    {
        if (tempTicks < node.position[i].time)
        {
            result *= node.position[i].transMat;
            break;
        }
//        tempTicks -= node.position[i].time;
    }
    tempTicks = anim.totalTicks;
    for (int i = 0; i < node.rotate.size(); i++)
    {
        if (tempTicks < node.rotate[i].time)
        {
            result *= node.rotate[i].transMat;
            break;
        }
//        tempTicks -= node.rotate[i].time;
    }
    tempTicks = anim.totalTicks;
    for (int i = 0; i < node.scale.size(); i++)
    {
        if (tempTicks < node.scale[i].time)
        {
            result *= node.scale[i].transMat;
            break;
        }
//        tempTicks -= node.scale[i].time;
    }
    
    return result;
}

void Model::process()
{
	//加载纹理
	vector<Texture>::iterator itTex;
	for (itTex = textureLoaded.begin(); itTex != textureLoaded.end(); itTex++)
	{
		itTex->id = textureFromFile(itTex->path.c_str());
	}
	//加载材质，UBO
	vector<Material>::iterator itMat;
	for (itMat = materialLoaded.begin(); itMat != materialLoaded.end(); itMat++)
	{
		//新建一个UBO
		GLuint UBO;
		glGenBuffers(1, &UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, UBO);
		//缓冲区尺寸：vec4 * 3 + float * 1 + uint * 6，
		glBufferData(GL_UNIFORM_BUFFER, 80, NULL, GL_STATIC_DRAW);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, offsetof(Material,id), &(*itMat));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		itMat->UBO = UBO;
		//将纹理置换为实际id
		if (itMat->ambientTexed)
			itMat->ambientTex = textureLoaded[itMat->ambientTex].id;
		if (itMat->diffuseTexed)
			itMat->diffuseTex = textureLoaded[itMat->diffuseTex].id;
		if (itMat->specularTexed)
			itMat->specularTex = textureLoaded[itMat->specularTex].id;
		if (itMat->heightTexed || itMat->normalTexed)
			itMat->bumpTex = textureLoaded[itMat->bumpTex].id;
		if (itMat->alphaTexed)
			itMat->alphaTex = textureLoaded[itMat->alphaTex].id;
	}
	//将材质传给mesh
	vector<Mesh>::iterator itMesh;
	for (itMesh = meshes.begin(); itMesh != meshes.end(); itMesh++)
	{
		itMesh->setMaterial(materialLoaded[itMesh->getMatID()]);
		itMesh->setupMesh();
	}
}

void Model::destroy()
{
	vector<Mesh>::iterator itMesh;
	for (itMesh = meshes.begin(); itMesh != meshes.end(); itMesh++)
	{
		itMesh->destroy();
	}
	vector<Material>::iterator itMat;
	for (itMat = materialLoaded.begin(); itMat != materialLoaded.end(); itMat++)
	{
		glDeleteBuffers(1, &itMat->UBO);
	}
	vector<Texture>::iterator itTex;
	for (itTex = textureLoaded.begin(); itTex != textureLoaded.end(); itTex++)
	{
		glDeleteTextures(1, &itTex->id);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, bits);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    //SOIL_free_image_data(data);
    FreeImage_Unload(bmp);
    return tex;
}

//void Model::drawNode(GLuint program, Node &node, glm::mat4 transMat)
//{
//	glm::mat4 transform = transMat * node.transform;
//	for (unsigned int i = 0; i < node.meshIndices.size(); i++)
//	{
//		//内部变换矩阵
//		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(transform));
//		meshes[node.meshIndices[i]].draw(program);
//	}
//	for (unsigned int i = 0; i < node.childs.size(); i++)
//	{
//		drawNode(program, node.childs[i], transform);
//	}
//}

void Model::drawNode(GLuint program, Node &node, glm::mat4 transMat, double tick)
{
    glm::mat4 transform(1.0);
    if (!hasAnimation)
        transform = transMat * node.transform;
    
    for (unsigned int i = 0; i < node.meshIndices.size(); i++)
    {
        //内部变换矩阵
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(transform));
        //传入是否有动画的标志
        glUniform1i(glGetUniformLocation(program, "hasAnimation"), (GLint)hasAnimation);
        if(hasAnimation)    //更新动画矩阵
            meshes[node.meshIndices[i]].updateAnimMatrix(tick);
        meshes[node.meshIndices[i]].draw(program);
    }
    for (unsigned int i = 0; i < node.childs.size(); i++)
    {
        drawNode(program, node.childs[i], transform, tick);
    }
}

//void Model::draw(GLuint program, glm::mat4 transform)
//{
//	//跳过根节点的变换
//	drawNode(program, rootNode, transform * globalInverseTransform);
//}

void Model::draw(GLuint program, glm::mat4 transform, double time)
{
    //有动画矩阵
    if (hasAnimation)
    {
        double currentTick = anim.ticksPerSec * time;
        anim.totalTicks = std::fmod(anim.totalTicks + currentTick, anim.duration);  //求得不大于duration的结果
    }
    else
        anim.totalTicks = 0;
    //跳过根节点的变换
    drawNode(program, rootNode, transform * globalInverseTransform, anim.totalTicks);
}