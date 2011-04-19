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

MeshObj::~MeshObj() {
  mVertexData.clear();
  mIndexData.clear();
}

void MeshObj::setData(const std::vector<Vertex> &vertexData, const std::vector<unsigned int> &indexData) {
  // TODO: compute the loaded meshes bounds that may be used later on and store them in mMinBounds, mMaxBounds resp. //
  
  // TODO: COPY the data from vertexData and indexData in own data vectors mVertexData and mIndexData //
}

void MeshObj::render(void) {
  // render the data stored in this object //
  // - use glBegin(GL_TRIANGLES) ... glEnd() to render every triangle indexed by the mIndexData list //
}

float MeshObj::getWidth(void) {
  // TODO: return the width of the current mesh here //
}

float MeshObj::getHeight(void) {
  // TODO: return the height of the current mesh here //
}

float MeshObj::getDepth(void) {
  // TODO: return the depth of the current mesh here //
}