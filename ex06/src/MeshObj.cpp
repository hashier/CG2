#include "MeshObj.h"
#include <iostream>
#include <limits>

MeshObj::MeshObj() {
  mVBO = 0;
  mIBO = 0;
  mIndexCount = 0;
  for (int i = 0; i < 3; ++i) {
    mMinBounds[i] = std::numeric_limits<float>::max();
    mMaxBounds[i] = std::numeric_limits<float>::min();
  }
}

MeshObj::~MeshObj() {
  glDeleteBuffers(1, &mIBO);
  glDeleteBuffers(1, &mVBO);
}

void MeshObj::setData(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData) {
  // compute bounds //
  for (int i = 0; i < 3; ++i) {
    mMinBounds[i] = std::numeric_limits<float>::max();
    mMaxBounds[i] = std::numeric_limits<float>::min();
  }
  for (unsigned int i = 0; i < vertexData.size(); ++i) {
    for (int j = 0; j < 3; ++j) {
      if (vertexData[i].position[j] < mMinBounds[j]) mMinBounds[j] = vertexData[i].position[j];
      if (vertexData[i].position[j] > mMaxBounds[j]) mMaxBounds[j] = vertexData[i].position[j];
    }
  }
  
  // TODO: copy your data into an vertex and index array (Vertex[], GLuint[]) //
  Vertex vertexArray[vertexData.size()];
  GLuint indexArray[indexData.size()];
  for(unsigned int i=0; i<vertexData.size(); i++) {
    vertexArray[i] = vertexData[i];
  }
  for(unsigned int i=0; i<indexData.size(); i++) {
    indexArray[i] = indexData[i];
  }
  mIndexCount = vertexData.size();
  
  // TODO: init and bind a VBO (vertex buffer object) //
  glGenBuffers(1, &mVBO);
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  
  // TODO: copy data into the VBO //
  glBufferData(GL_ARRAY_BUFFER, vertexData.size()*sizeof(Vertex), vertexArray, GL_STATIC_DRAW);
  
  // TODO: init and bind a IBO (index buffer object) //
  glGenBuffers(1, &mIBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
  
  // TODO: copy data into the IBO //
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size()*sizeof(unsigned int), indexArray, GL_STATIC_DRAW);
  
  // unbind buffers //
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObj::render(void) {
  const unsigned int vertexInterval = sizeof(Vertex);

  if (mVBO != 0) {
    // TODO: init vertex attribute arrays for vertex position, normal vector and texture coordinate //
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, vertexInterval, NULL);
    glNormalPointer(GL_FLOAT, vertexInterval, NULL);
    glTexCoordPointer(2, GL_FLOAT, vertexInterval, NULL);
    glColorPointer(4, GL_FLOAT, vertexInterval, NULL);
    
    // TODO: bind the index buffer object mIBO here to tell OpenGL to use the indices in that array for indexing elements in our vertex buffer //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    
    // TODO: render VBO as triangles //
    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, NULL);
    
    // unbind the buffers //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

float MeshObj::getWidth(void) {
  return mMaxBounds[0];
}

float MeshObj::getHeight(void) {
  return mMaxBounds[1] - mMinBounds[1]; 
}

float MeshObj::getDepth(void) {
  return mMaxBounds[2] - mMinBounds[2]; 
}
