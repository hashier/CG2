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
  
  // TODO: parse the given .obj file and create a new MeshObj from the imported data //
  
  // TODO: insert this MeshObj into the mMeshMap using the given identifier string ID //
}

MeshObj* ObjLoader::getMeshObj(std::string ID) {
  // TODO: return the object using ID //
  // return NULL if an object for the given ID doesn't exist //
}