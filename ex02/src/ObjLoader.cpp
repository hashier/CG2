#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>

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
    return *mMeshMap.find(ID);
  // TODO: parse the given .obj file and create a new MeshObj from the imported data //
  MeshObj* tmp_mesh = createMeshObj(std::stirng fileName);
  // TODO: insert this MeshObj into the mMeshMap using the given identifier string ID //
  mMeshMap.insert(std::pair<std::string, MeshObj*>(ID, tmp_mesh));  

  return tmp_mesh;
}

MeshObj* ObjLoader::getMeshObj(std::string ID) {
  // TODO: return the object using ID //
  // return NULL if an object for the given ID doesn't exist //
  if (mMeshMap.find(ID) != mMeshMap.end())
    return *mMeshMap.find(ID);
  else
    return NULL;
}
