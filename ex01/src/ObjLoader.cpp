#include "ObjLoader.h"

#include <iostream>
#include <sstream>
#include <cassert>

void parseOBJLine(const std::string&, std::vector<Vertex>&, std::vector<int>&);
Vertex parseVertex(std::istringstream&);
unsigned int parseFace(std::istringstream&, std::vector<int>&);

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
    unsigned int max_index = parseFace(iss, indexList);
    assert(max_index <= vertexList.size());
  }
}

Vertex parseVertex(std::istringstream& iss) {
  float x, y ,z;
  iss >> x >> y >> z;
  return Vertex(x, y, z);
}


unsigned int parseFace(std::istringstream& iss, std::vector<int>& indexList) {
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
