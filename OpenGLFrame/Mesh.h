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
    glm::vec4 color;
};

enum TextureType { Ambient, Diffuse, Specular };

struct Texture
{
    GLuint id;
    TextureType type;
    string path;
};

class Mesh
{
public:
    Mesh();
    Mesh(vector<Vertex> vertices, vector<Texture> textures, vector<GLuint> indices);
    ~Mesh();
    
private:
    GLuint VAO, VBO, EBO;
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;
    
public:
    void setupVertices(vector<Vertex> vertices, vector<Texture> textures, vector<GLuint> indices);
    GLboolean setupMesh();
    void draw(GLuint program);
};