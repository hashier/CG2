#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <cassert>

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
  
  // TODO: extend your objLoader to load vertices, vertex normals and texture coordinates
  //       note: faces using normals and texture coordinates are defines as "f vi0/ti0/ni0 ... viN/tiN/niN"
  //       vi0 .. viN : vertex index of vertex 0..N
  //       ti0 .. tiN : texture coordinate index of vertex 0..N
  //       ni0 .. niN : vertex normal index of vertex 0..N
  //       faces without normals and texCoords are defined as  "f vi0// ... viN//"
  //       faces without texCoords are defined as              "f vi0//ni0 ... viN//niN"
  //       make your parser robust against ALL possible combinations
  //       also allow to import QUADS as faces. directly split them up into two triangles!
  
  // setup variables used for parsing //
  std::string key;
  float x, y, z;
  int vi[4];
  int ni[4];
  int ti[4];

  // setup local lists                        //
  // import all data in temporary lists first //
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
      // DONE: implement parsing of vertex normals and texture coordinates //
      if (key == "vn") {
        sstr >> x >> y >> z;
        localNormalList.push_back(Point3D(x, y, z));
      }
      if (key == "vt") {
        sstr >> x >> y;
        localTexCoordList.push_back(Point3D(x, y));
      }
      
      if (!key.compare("f")) {
        // DONE: implement a robust method to load a face definition               //
        //       allowing vertex normals, tecture coordinates, triangles and quads //
        // read in vertex indices for a face //
        bool texture_coords_present = false;
        bool vertex_normals_present = false;
        unsigned int i = 0;
        for (; i < 4 && sstr.good(); i++) {
          // der facepoint sollte jetzt so aussehen: "v/vt/vn"
          // dann facepoint bei '/' auftrennen um die einzelnen indizes zu erhalten
          std::string facepoint;
          sstr >> facepoint;
          std::stringstream point_stream(facepoint);
          std::string token;
          getline(point_stream, token, '/');
          std::stringstream index_converter(token);
          vi[i] = 0;
          index_converter >> vi[i];
          if (vi[i] == 0) {
            std::cout << "fehler beim einlesen in Zeile: " << lineNumber << std::endl;
            assert(vi[i] > 0);
          }
          if (getline(point_stream, token, '/') && token.size() > 0) {
            index_converter.str(token);
            index_converter.clear();
            ti[i] = 0;
            index_converter >> ti[i];
            assert(ti[i] > 0);
            texture_coords_present = true;
          }
          if (getline(point_stream, token) && token.size() > 0) {
            index_converter.str(token);
            index_converter.clear();
            ni[i] = 0;
            index_converter >> ni[i];
            assert(ni[i] > 0);
            vertex_normals_present = true;
          }
        }
        
        assert(i == 3 || i == 4);
        
        // DONE: directly split up polygons using 4 vertices into two triangles //
        //       add all imported faces to 'localFaceList'                      //
        struct Face face1;
        for (unsigned int j = 0; j < 3; j++) {
          assert(vi[j] > 0);
          face1.vIndex[j] = vi[j];
          if (vertex_normals_present)
            face1.nIndex[j] = ni[j];
          else
            face1.nIndex[j] = 0;
          if (texture_coords_present)
            face1.tIndex[j] = ti[j];
          else
            face1.tIndex[j] = 0;
        }
        localFaceList.push_back(face1);
        if (i == 4) {
          struct Face face2;
          unsigned int face_index = 0;
          for (unsigned int j = 0; j < 4; j++, face_index++) {
            if (j == 1)
              j++;
            assert(vi[j] > 0);
            face1.vIndex[face_index] = vi[j];
            if (vertex_normals_present)
              face1.nIndex[face_index] = ni[j];
            else
              face1.nIndex[face_index] = 0;
            if (texture_coords_present)
              face1.tIndex[face_index] = ti[j];
            else
              face1.tIndex[face_index] = 0;
          }
          localFaceList.push_back(face2);
        }
      }
      lineNumber++;
    }
    file.close();
    std::cout << "Imported " << localFaceList.size() << " faces from \"" << fileName << "\"" << std::endl;
    
    // DONE: every face is able to use a different set of vertex normals and texture coordinates
    //       when using a single vertex for multiple faces, however, this conflicts multiple normals
    //       rearrange and complete the imported data in the following way:
    //        - if a vertex uses multiple normals and/or texture coordinates, create copies of that vertex
    //        - every triplet (vertex-index, textureCoord-index, normal-index) is unique and indexed by indexList
    // create vertex list for vertex buffer object //
    // one vertex definition per index-triplet (vertex index, texture index, normal index) //
    std::vector<Vertex> vertexList;
    std::vector<unsigned int> indexList;
    for (std::vector<Face>::iterator faceIter = localFaceList.begin(); faceIter != localFaceList.end(); ++faceIter) {
      Face f = *faceIter;
      for (unsigned int i = 0; i < 3; i++) {
        struct Vertex v;
        // hier alles mit -1 verringern, da obj bei 1 anfängt zu zählen
        // punkt übernehmen
        struct Point3D p = localVertexList[f.vIndex[i]-1];
        for (unsigned int j = 0; j < 3; j++)
          v.position[j] = p.data[j];
        // normale übernehmen
        if (f.nIndex[i] > 0) {
          p = localNormalList[f.nIndex[i]-1];
          for (unsigned int j = 0; j < 3; j++)
            v.normal[j] = p.data[j];
        }
        // textur übernehmen
        if (f.tIndex[i] > 0) {
          p = localTexCoordList[f.tIndex[i]-1];
          for (unsigned int j = 0; j < 2; j++)
            v.texcoord[j] = p.data[j];
        }
        vertexList.push_back(v);
        indexList.push_back(vertexList.size() - 1);
      }
    }
    
    // TODO: rearrange and complete data, when conflicting combinations of vertex and vertex attributes occur //
    
    // reconstruct normals from given vertex data (only if no normals have been imported) //
    if (localVertexList.size() == 0) reconstructNormals(vertexList, indexList);
    
    // create new MeshObj and set imported geoemtry data //
    obj = new MeshObj();
    obj->setData(vertexList, indexList);
    
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
  for (unsigned int i = 0; i < indexList.size(); ++i) {
    normalizeVector(vertexList[indexList[i]].normal);
  }
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
