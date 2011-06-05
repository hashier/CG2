#include "MeshObj.h"
#include <iostream>
#include <limits>

#define OFFSET(i) ((char*)NULL + (i))

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
  for (int i = 0; i < vertexData.size(); ++i) {
    for (int j = 0; j < 3; ++j) {
      if (vertexData[i].position[j] < mMinBounds[j]) mMinBounds[j] = vertexData[i].position[j];
      if (vertexData[i].position[j] > mMaxBounds[j]) mMaxBounds[j] = vertexData[i].position[j];
    }
  }
  
  int numVertices = vertexData.size();
  Vertex vertexArray[numVertices];
  std::copy(vertexData.begin(), vertexData.end(), vertexArray);
  mIndexCount = indexData.size();
  GLuint indexArray[mIndexCount];
  std::copy(indexData.begin(), indexData.end(), indexArray);
  
  // init and bind a VBO (vertex buffer object) //
  if (mVBO == 0) {
    glGenBuffers(1, &mVBO);
  }
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  // copy data into the VBO //
  glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertexArray, GL_STATIC_DRAW);
  
  // init and bind a IBO (index buffer object) //
  if (mIBO == 0) {
    glGenBuffers(1, &mIBO);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
  // copy data into the IBO //
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(GLint), indexArray, GL_STATIC_DRAW);
  
  // unbind buffers //
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObj::render(GLint t_attr, GLint b_attr) {
  if (mVBO != 0) {
    // init vertex attribute arrays //
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(0));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(6 * sizeof(GLfloat)));
    
    // DONE: upload tangent and bitangent attributes (by using texCoord1 and 2 for example) //
    glEnableVertexAttribArray(t_attr);
    glVertexAttribPointer(t_attr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(b_attr);
    glVertexAttribPointer(b_attr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(11 * sizeof(GLfloat)));
    
    // bind the index buffer object mIBO here //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    
    // render VBO as triangles //
    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, (void*)0);
    
    // unbind the element render buffer //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // unbind the vertex array buffer //
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
