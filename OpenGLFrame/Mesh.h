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
//    glm::vec4 color;
};

struct Material
{
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    GLfloat shininess;
    glm::bvec3 textured;
    GLuint ambientTex;
    GLuint diffuseTex;
    GLuint specularTex;
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