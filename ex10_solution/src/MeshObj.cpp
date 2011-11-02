#include "MeshObj.h"
#include <iostream>
#include <limits>
#include <cmath>

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
  for (size_t i = 0; i < vertexData.size(); ++i) {
    for (int j = 0; j < 3; ++j) {
      if (vertexData[i].position[j] < mMinBounds[j]) mMinBounds[j] = vertexData[i].position[j];
      if (vertexData[i].position[j] > mMaxBounds[j]) mMaxBounds[j] = vertexData[i].position[j];
    }
  }
  
  // save local copy of vertex data (needed for shadow volume computation) //
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

void MeshObj::setMaterial(Material *material) {
  mMaterial = material;
}

void MeshObj::render(void) {
  if (mMaterial != NULL) {
    mMaterial->enable();
  }
  if (mVBO != 0) {
    // init vertex attribute arrays //
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    
    GLint vertexLoc = glGetAttribLocation(mMaterial->getShaderProgram()->getProgramID(), "vertex_OS");
    GLint texCoordLoc = glGetAttribLocation(mMaterial->getShaderProgram()->getProgramID(), "texCoord_OS");
    GLint normalLoc = glGetAttribLocation(mMaterial->getShaderProgram()->getProgramID(), "normal_OS");
    GLint tangentLoc = glGetAttribLocation(mMaterial->getShaderProgram()->getProgramID(), "tangent_OS");
    GLint bitangentLoc = glGetAttribLocation(mMaterial->getShaderProgram()->getProgramID(), "bitangent_OS");
    
    glEnableVertexAttribArray(vertexLoc);
    glVertexAttribPointer(vertexLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(0));
    glEnableVertexAttribArray(normalLoc);
    glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(texCoordLoc);
    glVertexAttribPointer(texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(tangentLoc);
    glVertexAttribPointer(tangentLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(bitangentLoc);
    glVertexAttribPointer(bitangentLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(11 * sizeof(GLfloat)));
  
    // bind the index buffer object mIBO here //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    
    // render VBO as triangles //
    glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, (void*)0);
    
    // unbind the element render buffer //
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // unbind the vertex array buffer //
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  if (mMaterial != NULL) {
    mMaterial->disable();
  }
}

#define INF 10000

void MeshObj::initShadowVolume(GLfloat lightPos[4]) {
  // clone all vertices and add their projected versions //
  std::vector<Vertex> shadowVertexData;
  shadowVertexData.assign(mVertexData.begin(), mVertexData.end());
  for (std::vector<Vertex>::iterator vertexIter = mVertexData.begin(); vertexIter != mVertexData.end(); ++vertexIter) {
    GLfloat projDir[3];
    if (lightPos[3] != 0) {
      // this is a non-directional light source at a distinct position //
      for (unsigned int i = 0; i < 3; ++i) {
        projDir[i] = vertexIter->position[i] - lightPos[i];
      }
    } else {
      // this is a directional light source at infinity -> use directional vector directly //
      for (unsigned int i = 0; i < 3; ++i) {
        projDir[i] = lightPos[i];
      }
    }
    // normalize projection direction //
    GLfloat norm = sqrt(projDir[0] * projDir[0] + projDir[1] * projDir[1] + projDir[2] * projDir[2]);
    if (norm != 0) {
      for (unsigned int i = 0; i < 3; ++i) {
        projDir[i] /= norm;
      }
    }
    // create new vertex from projection to 'infinity' -> large enough scaling factor //
    Vertex vertex;
    for (unsigned int i = 0; i < 3; ++i) {
      vertex.position[i] = vertexIter->position[i] + INF * projDir[i];
    }
    shadowVertexData.push_back(vertex);
  }
  
  // project 8 (6 sides + 2 caps) shadow triangles for each mesh triangle //
  unsigned int indexOffset = mVertexData.size();
  // new index array for shadow volume geometry //
  std::vector<unsigned int> shadowIndexData;
  for (unsigned int vertexIdxIter = 0; vertexIdxIter < (mIndexData.size() - 3); vertexIdxIter += 3) {
    unsigned int vIdx[3];
    Vertex *v[3];
    for (unsigned int i = 0; i < 3; ++i) {
      vIdx[i] = mIndexData.at(vertexIdxIter + i);
      v[i] = &mVertexData.at(vIdx[i]);
    }

    // check face normal against light dir for correct orientation //
    // simple approach -> compute face center and check face normal orientation at this point //
    //       if face normal points away from light source -> face orientation must be flipped //
    GLfloat centerPoint[3];
    for (unsigned int i = 0; i < 3; ++i) {
      centerPoint[i]  = v[0]->position[i];
      centerPoint[i] += v[1]->position[i];
      centerPoint[i] += v[2]->position[i];
      centerPoint[i] /= 3.0f;
    }
    GLfloat lightDir[3];
    GLfloat norm = 0;
    for (unsigned int i = 0; i < 3; ++i) {
      lightDir[i] = lightPos[i] - centerPoint[i];
      norm += lightDir[i] * lightDir[i];
    }
    norm = sqrt(norm);
    lightDir[0] /= norm;
    lightDir[1] /= norm;
    lightDir[2] /= norm;
    
    GLfloat e0[3] = {v[1]->position[0] - v[0]->position[0],
                     v[1]->position[1] - v[0]->position[1],
                     v[1]->position[2] - v[0]->position[2]};
    GLfloat e1[3] = {v[2]->position[0] - v[0]->position[0],
                     v[2]->position[1] - v[0]->position[1],
                     v[2]->position[2] - v[0]->position[2]};
    // face normal by cross product //
    GLfloat faceNormal[3] = {e0[1] * e1[2] - e0[2] * e1[1],
                             e0[2] * e1[0] - e0[0] * e1[2],
                             e0[0] * e1[1] - e0[1] * e1[0]};
    norm = 0;
    for (unsigned int i = 0; i < 3; ++i) {
      norm += faceNormal[i] * faceNormal[i];
    }
    norm = sqrt(norm);
    faceNormal[0] /= norm;
    faceNormal[1] /= norm;
    faceNormal[2] /= norm;
    
    GLfloat dot = lightDir[0] * faceNormal[0]
                + lightDir[1] * faceNormal[1]
                + lightDir[2] * faceNormal[2];
    
    if (fabs(dot) == 0) {
      // silhouette to tiny -> do not project any face ... //
      continue;
    }
    
    // dot product negative -> normal points away from light source //
    if (dot < 0) {
      // swap vertex 1 and 2 -> reverse order flips face orientation //
      unsigned int tmp = vIdx[2];
      vIdx[2] = vIdx[1];
      vIdx[1] = tmp;
    }
    
    // for each edge create 2 projected triangles //
    // v_i---v_(i+1)
    //  | \   |
    //  |  \  |
    //  |   \ |
    // d_i---d_(i+1)
    for (unsigned int i = 0; i < 3; ++i) {
      unsigned int i0 = i;
      unsigned int i1 = (i + 1) % 3;
      // face 0 //
      shadowIndexData.push_back(vIdx[i0]);
      shadowIndexData.push_back(vIdx[i0] + indexOffset);
      shadowIndexData.push_back(vIdx[i1] + indexOffset);
      // face 1 //
      shadowIndexData.push_back(vIdx[i0]);
      shadowIndexData.push_back(vIdx[i1] + indexOffset);
      shadowIndexData.push_back(vIdx[i1]);
    }
    // cap top //
    shadowIndexData.push_back(vIdx[0]);
    shadowIndexData.push_back(vIdx[1]);
    shadowIndexData.push_back(vIdx[2]);
    // cap back //
    shadowIndexData.push_back(vIdx[0] + indexOffset);
    shadowIndexData.push_back(vIdx[2] + indexOffset);
    shadowIndexData.push_back(vIdx[1] + indexOffset);
  }
  mShadowIndexCount = shadowIndexData.size();
  
  // setup VBOs //
  // init and bind a VBO (vertex buffer object) //
  if (mShadowVBO == 0) {
    glGenBuffers(1, &mShadowVBO);
  }
  glBindBuffer(GL_ARRAY_BUFFER, mShadowVBO);
  // copy data into the VBO //
  glBufferData(GL_ARRAY_BUFFER, shadowVertexData.size() * sizeof(Vertex), &shadowVertexData[0], GL_STATIC_DRAW);
  
  // init and bind a IBO (index buffer object) //
  if (mShadowIBO == 0) {
    glGenBuffers(1, &mShadowIBO);
  }
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mShadowIBO);
  // copy data into the IBO //
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, shadowIndexData.size() * sizeof(GLuint), &shadowIndexData[0], GL_STATIC_DRAW);

  // unbind buffers //
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshObj::renderShadowVolume() {
  if (mShadowVBO != 0) {
    // init vertex attribute arrays //
    glBindBuffer(GL_ARRAY_BUFFER, mShadowVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET(0));
    
    // bind the index buffer object mIBO here //
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
  return mMaxBounds[0];
}

float MeshObj::getHeight(void) {
  return mMaxBounds[1] - mMinBounds[1]; 
}

float MeshObj::getDepth(void) {
  return mMaxBounds[2] - mMinBounds[2]; 
}