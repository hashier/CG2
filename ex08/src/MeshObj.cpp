#include "MeshObj.h"
#include <iostream>
#include <limits>
#include <cmath>

#include "ObjLoader.h"

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
  
  // copy given vertex data to local storage //
  mVertexData.assign(vertexData.begin(), vertexData.end());
  mIndexData.assign(indexData.begin(), indexData.end());
  mIndexCount = indexData.size();
  
  // init and bind a VBO (vertex buffer object) //
  if (mVBO == 0) {
    glGenBuffers(1, &mVBO);
  }
  glBindBuffer(GL_ARRAY_BUFFER, mVBO);
  // copy data into the VBO //
  glBufferData(GL_ARRAY_BUFFER, mVertexData.size() * sizeof(Vertex), &mVertexData[0], GL_STATIC_DRAW);
  
  // init and bind a IBO (index buffer object) //
  if (mIBO == 0) {
    glGenBuffers(1, &mIBO);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
  // copy data into the IBO //
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount * sizeof(GLint), &mIndexData[0], GL_STATIC_DRAW);
  
  // unbind buffers //
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObj::render(void) {
  if (mVBO != 0) {
    // init vertex attribute arrays //
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    
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

void MeshObj::initShadowVolume(GLfloat lightPos[4]) {
  const unsigned int very_long_distance = 20;
  // init vince custom vector for convenience
  Point3D light = Point3D(lightPos);

  // TODO: init the shadow volume for a given light source position 'lightPos' //
  // not sure what to do here
  
  // TODO: clone all vertices and compute their projections //
  //       -> store as NEW local array of 'vertex'          //
  Vertex vertices[mVertexData.size()*2];
  for(int i=0; i<mVertexData.size(); i++) {
    vertices[i].position[0] = mVertexData[i].position[0];
    vertices[i].position[1] = mVertexData[i].position[1];
    vertices[i].position[2] = mVertexData[i].position[2];
  }
  for(int i=mVertexData.size(); i<mVertexData.size()*2; i++) {
    Point3D oldVert = Point3D(mVertexData[i - mVertexData.size()]);
    Point3D newVert = Point3D();
    newVert = oldVert + (oldVert - light).normalize() * very_long_distance;
    vertices[i].position[0] = newVert.data[0];
    vertices[i].position[1] = newVert.data[1];
    vertices[i].position[2] = newVert.data[2];
  }
  
  // TODO: compute shadow volume faces (6 resp. 8 triangles depending on technique) //
  //       -> check correct face orientation                                        //
  //       -> create new faces by adding indices to a NEW index array               //
  // TODO: store the index count for indexed vertex buffer rendering to 'mShadowIndexCount' //
  const unsigned int step = 6;
  unsigned int indizes[mShadowIndexCount = step*mIndexCount];
  for (unsigned int i = 0; i < mIndexCount; i+=3) {
    // punkte holen
    unsigned int p0 = mIndexData[i+0];
    unsigned int p1 = mIndexData[i+1];
    unsigned int p2 = mIndexData[i+2];
    // projektionen der punkte holen
    unsigned int p0_strich = mIndexData[i+0] + mVertexData.size();
    unsigned int p1_strich = mIndexData[i+1] + mVertexData.size();
    unsigned int p2_strich = mIndexData[i+2] + mVertexData.size();

    Point3D p0_p3d(mVertexData[p0]);
    Point3D p1_p3d(mVertexData[p1]);
    Point3D p2_p3d(mVertexData[p2]);
    Point3D triangle_to_light = light - p0_p3d;

    Point3D p01 = (p1_p3d - p0_p3d).normalize();
    Point3D p02 = (p2_p3d - p0_p3d).normalize();
    Point3D triangle_normal = p01.cross(p02);

    bool reverse_orientation = triangle_normal * triangle_to_light > 0;

    // volume erstellen
    // rechteck bei p0 und p1
    indizes[i*step + 0] = p0;
    indizes[i*step + 1 + reverse_orientation] = p1_strich;
    indizes[i*step + 2 - reverse_orientation] = p0_strich;

    indizes[i*step + 3] = p0;
    indizes[i*step + 4 + reverse_orientation] = p1;
    indizes[i*step + 5 - reverse_orientation] = p1_strich;

    // rechteck bei p0 und p2
    indizes[i*step + 6] = p0;
    indizes[i*step + 7 + reverse_orientation] = p0_strich;
    indizes[i*step + 8 - reverse_orientation] = p2_strich;

    indizes[i*step + 9] = p2;
    indizes[i*step + 10 + reverse_orientation] = p0;
    indizes[i*step + 11 - reverse_orientation] = p2_strich;

    // reckteck bei p1 und p2
    indizes[i*step + 12] = p1;
    indizes[i*step + 13 + reverse_orientation] = p2_strich;
    indizes[i*step + 14 - reverse_orientation] = p1_strich;

    indizes[i*step + 15] = p2;
    indizes[i*step + 16 + reverse_orientation] = p2_strich;
    indizes[i*step + 17 - reverse_orientation] = p1;

    if (step == 8) {
      // Vorne und hinten
      indizes[i*step + 18] = p0;
      indizes[i*step + 19] = p1;
      indizes[i*step + 20] = p2;

      indizes[i*step + 21] = p0_strich;
      indizes[i*step + 22] = p2_strich;
      indizes[i*step + 23] = p1_strich;
    }
  }
  
  
  // TODO: setup VBO ('mShadowVBO') and IBO ('mShadowIBO') for the computed data //
  if (mShadowVBO == 0) {
    glGenBuffers(1, &mShadowVBO);
  }
  glBindBuffer(GL_ARRAY_BUFFER, mShadowVBO);
  // copy data into the VBO //
  glBufferData(GL_ARRAY_BUFFER, mVertexData.size() * 2 * sizeof(Vertex), vertices, GL_STATIC_DRAW);
  
  // init and bind a IBO (index buffer object) //
  if (mShadowIBO == 0) {
    glGenBuffers(1, &mShadowIBO);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mShadowIBO);
  // copy data into the IBO //
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mShadowIndexCount * sizeof(GLint), indizes, GL_STATIC_DRAW);
  
  // unbind buffers //
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObj::renderShadowVolume() {
  // TODO: render the shadow volume //
  if (mShadowVBO != 0) {
    // init vertex attribute arrays //
    glBindBuffer(GL_ARRAY_BUFFER, mShadowVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    
    // bind the index buffer object mShadowIBO here //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mShadowIBO);
    
    // render VBO as triangles //
    glDrawElements(GL_TRIANGLES, mShadowIndexCount, GL_UNSIGNED_INT, (void*)0);
    
    // unbind the element render buffer //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // unbind the vertex array buffer //
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

float MeshObj::getWidth(void) {
  return mMaxBounds[0] - mMinBounds[0];
}

float MeshObj::getHeight(void) {
  return mMaxBounds[1] - mMinBounds[1]; 
}

float MeshObj::getDepth(void) {
  return mMaxBounds[2] - mMinBounds[2]; 
}
