#ifndef __OBJ_LOADER__
#define __OBJ_LOADER__

#include <map>
#include <vector>
#include <string>
#include <cmath>

#include "MeshObj.h"

// copied Vincent's 3D-Vector-struct
struct Point3D {
  Point3D(float x = 0, float y = 0, float z = 0) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }
  Point3D(float const* const d) {
    data[0] = d[0];
    data[1] = d[1];
    data[2] = d[2];
  }
  Point3D(Vertex const &v) {
    data[0] = v.position[0];
    data[1] = v.position[1];
    data[2] = v.position[2];
  }
  float data[3];

  float operator*(Point3D const &a) {
      float r = 0.0f;
      r += this->data[0] * a.data[0];
      r += this->data[1] * a.data[1];
      r += this->data[2] * a.data[2];
      return r;
  }

  Point3D operator*(float const &a) {
      Point3D p(this->data);
      p.data[0] *= a;
      p.data[1] *= a;
      p.data[2] *= a;
      return p;
  }

  Point3D operator+(Point3D const &a) {
      Point3D p(this->data);
      p.data[0] += a.data[0];
      p.data[1] += a.data[1];
      p.data[2] += a.data[2];
      return p;
  }

  Point3D operator-(Point3D const &a) {
      Point3D p(this->data);
      p.data[0] -= a.data[0];
      p.data[1] -= a.data[1];
      p.data[2] -= a.data[2];
      return p;
  }

  Point3D cross(Point3D const &a) {
      Point3D p;
      p.data[0] = this->data[1] * a.data[2] - this->data[2] * a.data[1];
      p.data[1] = this->data[2] * a.data[0] - this->data[0] * a.data[2];
      p.data[2] = this->data[0] * a.data[1] - this->data[1] * a.data[0];
      return p;
  }

  float length() {
    return sqrt(data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
  }

  Point3D normalize() {
      return Point3D(*this) * (1/length());
  }

};

Point3D operator*(float const &a, Point3D const &b);

#if 0
struct Point3D {
  Point3D(float x = 0, float y = 0, float z = 0) {
    data[0] = x;
    data[1] = y;
    data[2] = z;
  }
  float data[3];
};
#endif

struct Face {
  unsigned int vIndex[3];
  unsigned int nIndex[3];
  unsigned int tIndex[3];
};

class ObjLoader {
  public:
    ObjLoader();
    ~ObjLoader();
    MeshObj* loadObjFile(std::string fileName, std::string ID = "", float scale = 1.0f);
    MeshObj* getMeshObj(std::string ID);
  private:
    void reconstructNormals(std::vector<Vertex> &vertexList, const std::vector<unsigned int> &indexList);
    void computeTangentSpace(std::vector<Vertex> &vertexList, const std::vector<unsigned int> &indexList);
    void normalizeVector(float *vector, int dim = 3);
    
    std::map<std::string, MeshObj*> mMeshMap;
};

#endif
