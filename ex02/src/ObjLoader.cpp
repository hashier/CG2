#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

ObjLoader::ObjLoader() {
  mMeshMap.clear();
}

ObjLoader::~ObjLoader() {
  mMeshMap.clear();
}

MeshObj* ObjLoader::loadObjFile(std::string fileName, std::string ID) {
  // TODO: check for already existing objects for the given ID string //
  //       if ID is already known, return the corresponding MeshObj instead of creating a new one //
  if (mMeshMap.find(ID) != mMeshMap.end())
    return mMeshMap.find(ID)->second;
  // TODO: parse the given .obj file and create a new MeshObj from the imported data //
  MeshObj* tmp_mesh = createMeshObj(fileName);
  // TODO: insert this MeshObj into the mMeshMap using the given identifier string ID //
  mMeshMap.insert(std::pair<std::string, MeshObj*>(ID, tmp_mesh));  

  return tmp_mesh;
}

MeshObj* ObjLoader::getMeshObj(std::string ID) {
  // TODO: return the object using ID //
  // return NULL if an object for the given ID doesn't exist //
  if (mMeshMap.find(ID) != mMeshMap.end())
    return mMeshMap.find(ID)->second;
  else
    return NULL;
}


MeshObj * ObjLoader::createMeshObj(std::string fileName) {
  std::vector<Vertex> vertexList;
  std::vector<unsigned int> indexList;
  loadObjFile(fileName.c_str(), vertexList, indexList);
  return new MeshObj(vertexList, indexList);
}

void ObjLoader::loadObjFile(const char *fileName, std::vector<Vertex> &vertexList, std::vector<unsigned int> &indexList) {
  // see 'http://en.wikipedia.org/wiki/Wavefront_.obj_file' for specifications
  vertexList.clear();
  indexList.clear();

  // now open file
  std::fstream in;
  in.open(fileName, std::ios::in);
  if(in.bad() || in.fail())  {
    std::cerr << "Could not open file " << fileName << std::endl;
    return;
  }

  // read lines
  std::string line;
  while(!in.eof()) {
    std::getline(in, line);
    parseOBJLine(line, vertexList, indexList);
  }

  // finished parsing file -> close fileStream
  in.close();

//  std::cerr << "Finished parsing." << std::endl;
}

void ObjLoader::parseOBJLine(const std::string& line, std::vector<Vertex> &vertexList, std::vector<unsigned int> &indexList)
{
  std::istringstream iss(line);
  std::string key;
  iss >> key;
  if (key == "v") {
    // parse vertex //
    Vertex v = parseVertex(iss);
    vertexList.push_back(v);
  } else if (key == "f") {
    // parse face //
    unsigned int max_index = parseFace(iss, indexList);
    assert(max_index <= vertexList.size());
  }
}

Vertex ObjLoader::parseVertex(std::istringstream& iss) {
  float x, y ,z;
  iss >> x >> y >> z;
  return Vertex(x, y, z);
}


unsigned int ObjLoader::parseFace(std::istringstream& iss, std::vector<unsigned int>& indexList) {
  unsigned int index = 0;
  unsigned int max_index = 0;
  for (unsigned int i = 0; i < 3; i++) {
    iss >> index;
    assert(index > 0);
    // eigentlich nur für das debuggen
    if (index > max_index)
      max_index = index;
    indexList.push_back(index-1);
  }
  return max_index;
}
