#ifndef __MESH_OBJ__
#define __MESH_OBJ__

#include <GL/glew.h>
#include <GL/glut.h>

#include <vector>

struct Vertex {
  Vertex(float x = 0, float y = 0, float z = 0, float nx = 1, float ny = 0, float nz = 0, float b = 1, float g = 1, float r = 1, float a = 1) {
    position[0] = (GLfloat)x;
    position[1] = (GLfloat)y;
    position[2] = (GLfloat)z;
    normal[0] = (GLfloat)nx;
    normal[1] = (GLfloat)ny;
    normal[2] = (GLfloat)nz;
    texcoord[0] = (GLfloat)0;
    texcoord[1] = (GLfloat)0;
    color[0] = (GLfloat)b;
    color[1] = (GLfloat)g;
    color[2] = (GLfloat)r;
    color[3] = (GLfloat)a;
  }
  GLfloat position[3];
  GLfloat normal[3];
  GLfloat texcoord[2];
  GLfloat color[4];
};

class MeshObj {
  public:
    MeshObj();
    MeshObj(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData);
    ~MeshObj();
    
    void setData(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData);
    void render(void);
    
    float getWidth(void);
    float getHeight(void);
    float getDepth(void);
  private:
    std::vector<Vertex> mVertexData;
    std::vector<unsigned int> mIndexData;
    
    float mMinBounds[3];
    float mMaxBounds[3];
};

#endif
