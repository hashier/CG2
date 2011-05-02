#ifndef __OBJ_LOADER__
#define __OBJ_LOADER__

#include <map>
#include <string>

#include "MeshObj.h"

class ObjLoader {
  public:
    ObjLoader();
    ~ObjLoader();
    MeshObj* loadObjFile(std::string fileName, std::string ID = "");
    MeshObj* getMeshObj(std::string ID);
  private:
    std::map<std::string, MeshObj*> mMeshMap;

    MeshObj * createMeshObj(std::string fileName);
    void loadObjFile(const char *, std::vector<Vertex>&, std::vector<unsigned int>&);
    void parseOBJLine(const std::string&, std::vector<Vertex>&, std::vector<unsigned int>&);
    Vertex parseVertex(std::istringstream&);
    unsigned int parseFace(std::istringstream&, std::vector<unsigned int>&);
};

#endif
