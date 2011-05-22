#include "MeshObj.h"
#include <iostream>
#include <limits>

MeshObj::MeshObj() {
  mVertexData.clear();
  mIndexData.clear();
  
  for (int i = 0; i < 3; ++i) {
    mMinBounds[i] =  std::numeric_limits<float>::max();
    mMaxBounds[i] = -std::numeric_limits<float>::max();
  }
}

MeshObj::~MeshObj() {
  mVertexData.clear();
  mIndexData.clear();
}

void MeshObj::setData(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData) {
  // compute bounds //
  for (int i = 0; i < 3; ++i) {
    mMinBounds[i] =  std::numeric_limits<float>::max();
    mMaxBounds[i] = -std::numeric_limits<float>::max();
  }
  for (int i = 0; i < vertexData.size(); ++i) {
    for (int j = 0; j < 3; ++j) {
      mMinBounds[j] = std::min(vertexData[i].position[j], mMinBounds[j]);
      mMaxBounds[j] = std::max(vertexData[i].position[j], mMaxBounds[j]);
    }
  }
  
  mVertexData.assign(vertexData.begin(), vertexData.end());
  mIndexData.assign(indexData.begin(), indexData.end());
}

void MeshObj::render(void) {
  // set OpenGL to rendering triangles from all upcoming position values //
  glBegin(GL_TRIANGLES);
  // iterate over index list //
  for (std::vector<unsigned int>::iterator indexIter = mIndexData.begin(); indexIter != mIndexData.end(); ++indexIter) {
    // render indexed vertex //
    // DONE: pass texture coordinates as vertex attribute //
    glTexCoord2f(mVertexData[*indexIter].texcoord[0], mVertexData[*indexIter].texcoord[1]);
    glNormal3f(mVertexData[*indexIter].normal[0], mVertexData[*indexIter].normal[1], mVertexData[*indexIter].normal[2]);
    glVertex3f(mVertexData[*indexIter].position[0], mVertexData[*indexIter].position[1], mVertexData[*indexIter].position[2]);
  }
  // stop rendering geometry //
  glEnd();
}

float MeshObj::getWidth(void) {
  // return width //
  return mMaxBounds[0] - mMinBounds[0];
}

float MeshObj::getHeight(void) {
  // return height //
  return mMaxBounds[1] - mMinBounds[1]; 
}

float MeshObj::getDepth(void) {
  // return depth //
  return mMaxBounds[2] - mMinBounds[2]; 
}
