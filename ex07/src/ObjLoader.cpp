#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

ObjLoader::ObjLoader() {
}

ObjLoader::~ObjLoader() {
  // thanks to Vincent for pointing out the broken destructor ... erasing pointers without deleting the referenced object is baaaaad //
  for (std::map<std::string, MeshObj*>::iterator iter = mMeshMap.begin(); iter != mMeshMap.end(); ++iter) {
    delete iter->second;
    iter->second = NULL;
  }
  mMeshMap.clear();
}

MeshObj* ObjLoader::loadObjFile(std::string fileName, std::string ID, float scale) {
  // sanity check for identfier -> must not be empty //
  if (ID.length() == 0) {
    return NULL;
  }
  // try to load the MeshObj for ID //
  MeshObj* obj = getMeshObj(ID);
  if (obj != NULL) {
    // if found, return it instead of loading a new one from file //
    return obj;
  }
  // ID is not known yet -> try to load mesh from file //
  
  // setup variables used for parsing //
  std::string key;
  float x, y, z;
  int vi[4];
  int ni[4];
  int ti[4];

  // setup local lists //
  std::vector<Point3D> localVertexList;
  std::vector<Point3D> localNormalList;
  std::vector<Point3D> localTexCoordList;
  std::vector<Face> localFaceList;

  // setup tools for parsing a line correctly //
  std::string line;
  std::stringstream sstr;

  // open file //
  std::ifstream file(fileName.c_str());
  unsigned int lineNumber = 0;
  if (file.is_open()) {
    while (file.good()) {
      key = "";
      getline(file, line);
      sstr.clear();
      sstr.str(line);
      sstr >> key;
      if (!key.compare("v")) {
        // read in vertex //
        sstr >> x >> y >> z;
        localVertexList.push_back(Point3D(x * scale, y * scale, z * scale));
      }
      if (!key.compare("vn")) {
        // read in normal //
        sstr >> x >> y >> z;
        localNormalList.push_back(Point3D(x, y, z));
      }
      if (!key.compare("vt")) {
        // read in texture coordinate //
        sstr >> x >> y;
        localTexCoordList.push_back(Point3D(x, y, 0));
      }
      if (!key.compare("f")) {
        // read in vertex indices for a face //
        unsigned int vCount = 0;
        sstr.peek();
        while (sstr.good() && vCount < 4) {
          sstr >> vi[vCount];
          if (!sstr.good()) {
            // import of vertex index failed -> end of line reached //
            vi[vCount] = 0;
          } else {
            // vertex index import successful -> try to load texture and normal indices //
            if (sstr.peek() == '/') {
              sstr.get(); // skip '/' symbol //
              if (sstr.peek() != '/') {
                // there is a texture coordinate //
                sstr >> ti[vCount];
              } else {
                ti[vCount] = 0;
              }
              sstr.get(); // skip '/' symbol //
              sstr >> ni[vCount];
            } else {
              ti[vCount] = 0;
              ni[vCount] = 0;
            }
            ++vCount;
          }
        }
        
        // insert index data into face //
        if (vCount < 3) {
          std::cout << "(ObjLoader::loadObjFile) - WARNING: Malformed face in line " << lineNumber << std::endl;
          continue; // not a real face //
        } else if (vCount > 3) {
          // quad face loaded -> split into two triangles (0,1,2) and (0,2,3) //
          Face face0, face1;
          for (unsigned int v = 0; v < vCount; ++v) {
            if (v != 3) {
              face0.vIndex[v] = vi[v];
              face0.tIndex[v] = ti[v];
              face0.nIndex[v] = ni[v];
            }
            if (v != 1) {
              unsigned int _v = (v > 1) ? v - 1 : v;
              face1.vIndex[_v] = vi[v];
              face1.tIndex[_v] = ti[v];
              face1.nIndex[_v] = ni[v];
            }
          }
          localFaceList.push_back(face0);
          localFaceList.push_back(face1);
        } else {
          Face face;
          for (unsigned int v = 0; v < vCount; ++v) {
            face.vIndex[v] = vi[v];
            face.tIndex[v] = ti[v];
            face.nIndex[v] = ni[v];
          }
          localFaceList.push_back(face);
        }
      }
      
      ++lineNumber;
    }
    file.close();
    std::cout << "Imported " << localFaceList.size() << " faces from \"" << fileName << "\" using " << localVertexList.size() << " vertices" << std::endl;
    
    // create vertex list for vertex buffer object //
    // one vertex definition per index-triplet (vertex index, texture index, normal index) //
    std::map<std::string, unsigned int> vertexMap;
    std::vector<Vertex> vertexList;
    std::vector<unsigned int> indexList;
    for (std::vector<Face>::iterator faceIter = localFaceList.begin(); faceIter != localFaceList.end(); ++faceIter) {
      std::string vertexId("");
      const char* idPattern = "%08d|%08d|%08d";
      char idStr[27];
      // iterate over face vertices //
      for (unsigned int i = 0; i < 3; ++i) {
        sprintf(idStr, idPattern, faceIter->vIndex[i], faceIter->tIndex[i], faceIter->nIndex[i]);
        vertexId = std::string(idStr);
        std::map<std::string, unsigned int>::iterator vertexIter = vertexMap.find(vertexId);
        if (vertexIter == vertexMap.end()) {
          // vertex not known yet -> insert new one //
          Vertex vertex;
          
          Point3D position = localVertexList[faceIter->vIndex[i] - 1];
          vertex.position[0] = position.data[0];
          vertex.position[1] = position.data[1];
          vertex.position[2] = position.data[2];
          
          if (localNormalList.size() > (faceIter->nIndex[i] - 1)) {
            Point3D normal = localNormalList[faceIter->nIndex[i] - 1];
            vertex.normal[0] = normal.data[0];
            vertex.normal[1] = normal.data[1];
            vertex.normal[2] = normal.data[2];
          }
          
          if (localTexCoordList.size() > (faceIter->tIndex[i] - 1)) {
            Point3D texCoord = localTexCoordList[faceIter->tIndex[i] - 1];
            vertex.texcoord[0] = texCoord.data[0];
            vertex.texcoord[1] = texCoord.data[1];
          }
          
          unsigned int vertexIdx = vertexList.size();
          vertexMap.insert(std::make_pair(vertexId, vertexIdx));
          vertexList.push_back(vertex);
          indexList.push_back(vertexIdx);
        } else {
          // vertex already defined -> use index and add to indexList //
          indexList.push_back(vertexIter->second);
        }
      }
    }
    
    // reconstruct normals from given vertex data //
    if (localNormalList.size() == 0) reconstructNormals(vertexList, indexList);
    
    // calculate tangent space for object //
    if (localTexCoordList.size() > 0) computeTangentSpace(vertexList, indexList);
    
    // create new MeshObj and set imported geoemtry data //
    obj = new MeshObj();
    obj->setData(vertexList, indexList);
    
    std::cout << "Created " << indexList.size() / 3 << " faces using " << vertexList.size() << " vertices" << std::endl;
    
    // insert MeshObj into map //
    mMeshMap.insert(std::make_pair(ID, obj));
    
    // return newly created MeshObj //
    return obj;
  } else {
    std::cout << "(ObjLoader::loadObjFile) : Could not open file: \"" << fileName << "\"" << std::endl;
    return NULL;
  }
}

void ObjLoader::reconstructNormals(std::vector<Vertex> &vertexList, const std::vector<unsigned int> &indexList) {
  // iterator over faces (given by index triplets) and calculate normals for each incident vertex //
  for (unsigned int i = 0; i < (indexList.size() - 3); i += 3) {
    // face edges incident with vertex 0 //
    GLfloat edge0[3] = {vertexList[indexList[i+1]].position[0] - vertexList[indexList[i]].position[0],
                        vertexList[indexList[i+1]].position[1] - vertexList[indexList[i]].position[1],
                        vertexList[indexList[i+1]].position[2] - vertexList[indexList[i]].position[2]};
    GLfloat edge1[3] = {vertexList[indexList[i+2]].position[0] - vertexList[indexList[i]].position[0],
                        vertexList[indexList[i+2]].position[1] - vertexList[indexList[i]].position[1],
                        vertexList[indexList[i+2]].position[2] - vertexList[indexList[i]].position[2]};
    normalizeVector(edge0);
    normalizeVector(edge1);
    // compute normal using cross product //
    GLfloat normal[3] = {edge0[1] * edge1[2] - edge0[2] * edge1[1],
                         edge0[2] * edge1[0] - edge0[0] * edge1[2],
                         edge0[0] * edge1[1] - edge0[1] * edge1[0]};
    normalizeVector(normal);
    
    // add this normal to all face-vertices //
    for (int j = 0; j < 3; ++j) {
      vertexList[indexList[i]].normal[j] += normal[j];
      vertexList[indexList[i+1]].normal[j] += normal[j];
      vertexList[indexList[i+2]].normal[j] += normal[j];
    }
  }

  // normalize all normals //
  for (unsigned int i = 0; i < vertexList.size(); ++i) {
    normalizeVector(vertexList[i].normal);
  }
}

void ObjLoader::computeTangentSpace(std::vector<Vertex> &vertexList, const std::vector<unsigned int> &indexList) {
  // DONE: iterator over faces (given by index triplets) and calculate tangents and bitangents for each vertex //
  for(unsigned int i=0; i<indexList.size(); i++) {
    Vertex &v0 = vertexList[indexList[i++]];
    Vertex &v1 = vertexList[indexList[i++]];
    Vertex &v2 = vertexList[indexList[i]];

  //       - compute triangle edges e1, e2
    Vertex e1, e2;
    e1.position[0] = v1.position[0] - v0.position[0];
    e1.position[1] = v1.position[1] - v0.position[1];
    e1.position[2] = v1.position[2] - v0.position[2];

    e2.position[0] = v2.position[0] - v0.position[0];
    e2.position[1] = v2.position[1] - v0.position[1];
    e2.position[2] = v2.position[2] - v0.position[2];

  //       - compute uv-distances dU1, dU2, dV1, dV2
    GLfloat dU1 = v1.texcoord[0] - v0.texcoord[0];
    GLfloat dU2 = v2.texcoord[0] - v0.texcoord[0];
    GLfloat dV1 = v1.texcoord[1] - v0.texcoord[1];
    GLfloat dV2 = v2.texcoord[1] - v0.texcoord[1];

  //       - compute determinant det(A)
    GLfloat det_a = dU1*dV2 - dU2*dV1;

  //       - compute tangent and bitangent from uv-mapping and triangle edges
    GLfloat dU1_inv = dV2/det_a;
    GLfloat dU2_inv = -dU2/det_a;
    GLfloat dV1_inv = -dV1/det_a;
    GLfloat dV2_inv = dU1/det_a;
    Vertex t, b;

    t.position[0] = dU1_inv*e1.position[0] + dV1_inv*e2.position[0];
    t.position[1] = dU1_inv*e1.position[1] + dV1_inv*e2.position[1];
    t.position[2] = dU1_inv*e1.position[2] + dV1_inv*e2.position[2];
    
    b.position[0] = dU2_inv*e1.position[0] + dV2_inv*e2.position[0];
    b.position[1] = dU2_inv*e1.position[1] + dV2_inv*e2.position[1];
    b.position[2] = dU2_inv*e1.position[2] + dV2_inv*e2.position[2];

  //       - accumulate tangent and bitangent to face-vertices
    v0.tangent[0] += t.position[0];
    v0.tangent[1] += t.position[1];
    v0.tangent[2] += t.position[2];
    v0.bitangent[0] += b.position[0];
    v0.bitangent[1] += b.position[1];
    v0.bitangent[2] += b.position[2];

    v1.tangent[0] += t.position[0];
    v1.tangent[1] += t.position[1];
    v1.tangent[2] += t.position[2];
    v1.bitangent[0] += b.position[0];
    v1.bitangent[1] += b.position[1];
    v1.bitangent[2] += b.position[2];

    v2.tangent[0] += t.position[0];
    v2.tangent[1] += t.position[1];
    v2.tangent[2] += t.position[2];
    v2.bitangent[0] += b.position[0];
    v2.bitangent[1] += b.position[1];
    v2.bitangent[2] += b.position[2];
  }
  
  // DONE: normalize accumulated tangents and bitangents //
  for(unsigned int i=0; i<vertexList.size(); i++) {
    normalizeVector(vertexList[i].tangent, 3);
    normalizeVector(vertexList[i].bitangent, 3);
  }
  
  // DONE: use gram-schmidt approach to reorthogonalize the current vectors //
  //       - compute remapped tangent: T' = T - (N*T)*N
  //       - normalize T'
  //       - compute cross-product to recover B' from T' and N
  for(unsigned int i=0; i<vertexList.size(); i++) {
    GLfloat NdotT = vertexList[i].normal[0]*vertexList[i].tangent[0] + vertexList[i].normal[1]*vertexList[i].tangent[1] + vertexList[i].normal[2]*vertexList[i].tangent[2]; 
    vertexList[i].tangent[0] = vertexList[i].tangent[0] - NdotT*vertexList[i].normal[0];
    vertexList[i].tangent[1] = vertexList[i].tangent[1] - NdotT*vertexList[i].normal[1];
    vertexList[i].tangent[2] = vertexList[i].tangent[2] - NdotT*vertexList[i].normal[2];

    normalizeVector(vertexList[i].tangent, 3);

    vertexList[i].bitangent[0] = vertexList[i].tangent[1]*vertexList[i].normal[2] - vertexList[i].tangent[2]*vertexList[i].normal[1];
    vertexList[i].bitangent[1] = vertexList[i].tangent[2]*vertexList[i].normal[0] - vertexList[i].tangent[0]*vertexList[i].normal[2];
    vertexList[i].bitangent[2] = vertexList[i].tangent[0]*vertexList[i].normal[1] - vertexList[i].tangent[1]*vertexList[i].normal[0];
  }
  
  // DONE: store computed vectors as vertex attributes //
  // ???
}

MeshObj* ObjLoader::getMeshObj(std::string ID) {
  // sanity check for ID //
  if (ID.length() > 0) {
    std::map<std::string, MeshObj*>::iterator mapLocation = mMeshMap.find(ID);
    if (mapLocation != mMeshMap.end()) {
      // mesh with given ID already exists in meshMap -> return this mesh //
      return mapLocation->second;
    }
  }
  // no MeshObj found for ID -> return NULL //
  return NULL;
}

void ObjLoader::normalizeVector(float *vector, int dim) {
  float length = 0.0f;
  for (int i = 0; i < dim; ++i) {
    length += powf(vector[i],2);
  }
  length = sqrt(length);
  
  if (length != 0) {
    for (int i = 0; i < dim; ++i) {
      vector[i] /= length;
    }
  }
}
