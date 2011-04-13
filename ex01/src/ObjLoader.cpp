#include "ObjLoader.h"

#include <iostream>
#include <sstream>

void parseOBJLine(const std::string&, std::vector<Vertex>&, std::vector<int>&);
Vertex parseVertex(std::istringstream&);

void loadObjFile(const char *fileName, std::vector<Vertex> &vertexList, std::vector<int> &indexList) {
  // TODO: implement .obj parser here
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

  std::cerr << "Finished parsing." << std::endl;
}

void parseOBJLine(const std::string& line, std::vector<Vertex> &vertexList, std::vector<int> &indexList)
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
    unsigned int index;
    iss >> index;
    indexList.push_back(index);
    iss >> index;
    indexList.push_back(index);
    iss >> index;
    indexList.push_back(index);
  }
}

Vertex parseVertex(std::istringstream& iss) {
  float x, y ,z;
  iss >> x >> y >> z;
  return Vertex( x-1, y-1, z-1);
}
