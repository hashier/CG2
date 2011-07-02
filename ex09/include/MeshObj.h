#ifndef __MESH_OBJ__
#define __MESH_OBJ__

#include <GL/glew.h>
#include <GL/glut.h>

#include <vector>

#include "Material.h"

struct Vertex {
  Vertex(float x = 0, float y = 0, float z = 0, float nx = 1, float ny = 0, float nz = 0, float tx = 0, float ty = 0) {
    position[0] = (GLfloat)x;
    position[1] = (GLfloat)y;
    position[2] = (GLfloat)z;
    normal[0] = (GLfloat)nx;
    normal[1] = (GLfloat)ny;
    normal[2] = (GLfloat)nz;
    texcoord[0] = (GLfloat)tx;
    texcoord[1] = (GLfloat)ty;
    tangent[0] = (GLfloat)0;
    tangent[1] = (GLfloat)0;
    tangent[2] = (GLfloat)0;
    bitangent[0] = (GLfloat)0;
    bitangent[1] = (GLfloat)0;
    bitangent[2] = (GLfloat)0;
  }
  GLfloat position[3];
  GLfloat normal[3];
  GLfloat texcoord[2];
  GLfloat tangent[3];
  GLfloat bitangent[3];
};

class MeshObj {
  public:
    MeshObj();
    ~MeshObj();
    
    void setData(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData);
    void setMaterial(Material *material);
    
    void render(void);
    
    void initShadowVolume(GLfloat lightPos[3]);
    void renderShadowVolume(void);
    
    float getWidth(void);
    float getHeight(void);
    float getDepth(void);
  private:
    std::vector<Vertex> mVertexData;
    std::vector<unsigned int> mIndexData;
    Material *mMaterial;
    
    GLuint mVBO;
    GLuint mIBO;
    GLuint mIndexCount;
    
    GLuint mShadowVBO;
    GLuint mShadowIBO;
    GLuint mShadowIndexCount;
    
    float mMinBounds[3];
    float mMaxBounds[3];
};

#endif
