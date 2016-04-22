//
//  Mesh.h
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
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
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
	GLuint	  normalTexed;
	GLuint	  heightTexed;
	GLuint	  alphaTexed;
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
    Mesh(vector<Vertex> vertices, vector<GLuint> indices, Material material, glm::mat4 transform);
    ~Mesh();
    
private:
    GLuint VAO, VBO, EBO;
    vector<Vertex> vertices;
    vector<GLuint> indices;
    Material material;
    glm::mat4 transform;
    
public:
    void setupVertices(vector<Vertex> vertices, vector<GLuint> indices, Material material);
    void setTransform(glm::mat4 transform) { this->transform = transform; };
    GLboolean setupMesh();
    void draw(GLuint program);
};