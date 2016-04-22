#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 18) out;

in vec3 gNormal[];

out int normalIndex;

const float MAGNITUDE = 0.05f;



void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(0.0f, gNormal[index].x, 0.0f, 0.0f) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(1.0f, 0.0f, 0.0f, 0.0f) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // First vertex normal
    GenerateLine(1); // Second vertex normal
    GenerateLine(2); // Third vertex normal
}