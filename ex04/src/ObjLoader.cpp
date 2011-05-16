#include "ObjLoader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

#include <list>

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

MeshObj* ObjLoader::loadObjFile(std::string fileName, std::string ID) {
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
    int i, j, k;

    // setup local lists //
    std::vector<Vertex> vertexList;
    std::vector<unsigned int> indexList;

    // setup tools for parsing a line correctly //
    std::string line;
    std::stringstream sstr;

    // open file //
    std::ifstream file(fileName.c_str());
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
                vertexList.push_back(Vertex(x, y, z));
            }
            if (!key.compare("f")) {
                // read in vertex indices for a face //
                sstr >> i >> j >> k;
                indexList.push_back(i - 1);
                indexList.push_back(j - 1);
                indexList.push_back(k - 1);
            }
        }
        file.close();
        std::cout << "Imported " << indexList.size() / 3 << " faces from \"" << fileName << "\"" << std::endl;

        // reconstruct normals from given vertex data //
        reconstructNormals(vertexList, indexList);

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

struct Vector {
    Vector(GLfloat x = 0.0f, GLfloat y = 0.0f, GLfloat z = 0.0f) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    GLfloat x, y, z;

    Vector operator+(Vector& other) {
        return Vector(this->x + other.x, this->y + other.y, this->z + other.z);
    }

};

/** Calculates the cross product of two vectors */
Vector crossProduct(const Vector a, const Vector b) {

    // a2 * b3 - a3 * b2
    // a3 * b1 - a1 * b3
    // a1 * b2 - a2 * b1

    Vector norm;

    norm.x = a.y * b.z - a.z * b.y;
    norm.y = a.z * b.x - a.x * b.z;
    norm.z = a.x * b.y - a.y * b.x;

    return norm;

}

void normalizeVector(Vector* v) {
    GLfloat len = sqrt( v->x * v->x + v->y * v->y + v->z * v->z );
    v->x /= len;
    v->y /= len;
    v->z /= len;

}

void ObjLoader::reconstructNormals(std::vector<Vertex> &vertexList, const std::vector<unsigned int> &indexList) {
    // XXX:  iterate over all faces defined by the indexList vector (index triplets define one face)   //
    //       compute a normal for every face
    //       accumulate these face normals for every incident vertex to a particular face
    //       finally normalize all vertex normals
    std::vector<Vector> faceNormals;
    faceNormals.resize(indexList.size() / 3);

    for(int i = 0; i < indexList.size(); i += 3) {

        // Get three points
        Vertex vex1 = vertexList[indexList[i+0]];
        Vertex vex2 = vertexList[indexList[i+1]];
        Vertex vex3 = vertexList[indexList[i+2]];

        // Vectors between the points
        Vector v1 = Vector( vex2.position[0] - vex1.position[0], vex2.position[1] - vex1.position[1], vex2.position[2] - vex1.position[2] );
        Vector v2 = Vector( vex3.position[0] - vex1.position[0], vex3.position[1] - vex1.position[1], vex3.position[2] - vex1.position[2] );

        // save face normal = Kreuzprodukt between vectors normal of the face)
        faceNormals[i/3] = crossProduct(v1, v2);

    }

    std::vector< std::list<unsigned int> > relevantFaces;
    relevantFaces.resize(vertexList.size());
    for(int i = 0; i < indexList.size(); i += 3) {
        relevantFaces[indexList[i+0]].push_back(i/3);
        relevantFaces[indexList[i+1]].push_back(i/3);
        relevantFaces[indexList[i+2]].push_back(i/3);
    }

    for(int i = 0; i < vertexList.size(); i++) {
        Vector v = Vector();
        for(std::list<unsigned int>::iterator it = relevantFaces[i].begin(); it != relevantFaces[i].end(); ++it) {
            Vector norm = faceNormals[*it];
            v = v + norm;
        }
        normalizeVector(&v);
        vertexList[i].normal[0] = v.x;
        vertexList[i].normal[1] = v.y;
        vertexList[i].normal[2] = v.z;
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
