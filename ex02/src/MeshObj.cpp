#include "MeshObj.h"
#include <iostream>
#include <limits>

MeshObj::MeshObj() {
  mVertexData.clear();
  mIndexData.clear();
  for (int i = 0; i < 3; ++i) {
    mMinBounds[i] = std::numeric_limits<float>::max();
    mMaxBounds[i] = std::numeric_limits<float>::min();
  }
}

MeshObj::MeshObj(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData) {
  for (int i = 0; i < 3; ++i) {
    mMinBounds[i] = std::numeric_limits<float>::max();
    mMaxBounds[i] = std::numeric_limits<float>::min();
  }
  setData(vertexData, indexData);
}

MeshObj::~MeshObj() {
  mVertexData.clear();
  mIndexData.clear();
}

void MeshObj::setData(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData) {
  // TODO: compute the loaded meshes bounds that may be used later on and store them in mMinBounds, mMaxBounds resp. //
  for (std::vector<Vertex>::const_iterator it = vertexData.begin(); it != vertexData.end(); it++) {
    Vertex v = *it;
    for (unsigned int i = 0; i < 3; i++) {
      if (v.position[i] < mMinBounds[i])
        mMinBounds[i] = v.position[i];
      if (v.position[i] > mMaxBounds[i])
        mMaxBounds[i] = v.position[i];
    }
  }
  
  // TODO: COPY the data from vertexData and indexData in own data vectors mVertexData and mIndexData //
  mVertexData.clear();
  mVertexData.resize(vertexData.size());
  std::copy(vertexData.begin(), vertexData.end(), mVertexData.begin());

  mIndexData.clear();
  mIndexData.resize(indexData.size());
  std::copy(indexData.begin(), indexData.end(), mIndexData.begin());
}

void MeshObj::render(void) {
  // render the data stored in this object //
  // - use glBegin(GL_TRIANGLES) ... glEnd() to render every triangle indexed by the mIndexData list //
  glBegin(GL_TRIANGLES);
  for (std::vector<unsigned int>::iterator it = mIndexData.begin(); it != mIndexData.end(); it++) {
      Vertex v = mVertexData[*it];
      glVertex3f(v.position[0], v.position[1], v.position[2]);
//      std::cout << "Vertex ist an Stelle: " << v.position[0] << ", " << v.position[1] << ", " << v.position[2] << std::endl;
  }
  glEnd();
}

float MeshObj::getWidth(void) {
  // TODO: return the width of the current mesh here //
  return mMaxBounds[0] - mMinBounds[0];
}

float MeshObj::getHeight(void) {
  // TODO: return the height of the current mesh here //
  return mMaxBounds[1] - mMinBounds[1];
}

float MeshObj::getDepth(void) {
  // TODO: return the depth of the current mesh here //
  return mMaxBounds[2] - mMinBounds[2];
}
