#ifndef __OBJ_LOADER__
#define __OBJ_LOADER__

#include <fstream>
#include <sstream>
#include <vector>

#include "OpenGLTools.h"

void loadObjFile(const char *fileName, std::vector<Vertex> &vertexList, std::vector<int> &indexList);

#endif
