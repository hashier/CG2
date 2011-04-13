#ifndef __OPENGL_TOOLS__
#define __OPENGL_TOOLS__

#include <GL/glew.h>
#include <GL/glut.h>

#include <vector>
#include <stddef.h>

struct Vertex {
  Vertex(float x = 0, float y = 0, float z = 0) {
    position[0] = (GLfloat)x;
    position[1] = (GLfloat)y;
    position[2] = (GLfloat)z;
  }
  GLfloat position[3];
  GLfloat texCoord[2];
};

void renderVertexArray(const std::vector<Vertex> &vertexArray, const std::vector<int> &indexList);

#endif