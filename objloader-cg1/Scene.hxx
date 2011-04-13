#ifndef SCENE_HXX
#define SCENE_HXX

#include <vector>
#include <string>
#include <fstream>


#include "Ray.hxx"
#include "Camera.hxx"
#include "Light.hxx"
#include "Primitive.hxx"
#include "kDTree.hxx"
#include "Texture.hxx"

class Scene
{
public:
  Scene();
  virtual ~Scene();

  // add another primitive to the scene
  void Add(Primitive* p);
  // add another light source to the scene
  void Add(Light* l);
  // add another texture
  void Add(Texture* t);

  // intersect the ray with all objects in the scene
  virtual bool Intersect(Ray& ray);
  // find occluder
  virtual bool Occluded(Ray& ray);

  // trace the given ray and shade it, returnt the color of the shaded ray
  Vec3f RayTrace(Ray& ray);

  // acces to camera and light sources
  const Camera* camera() const;
  std::vector<Light*> lights() const;
  std::vector<Texture*> textures() const;

  // set new camera
  void setCamera(const Camera* camera);
  // set new default shader for all primitives
  void setShader(const Shader* shader);

  // reads in a scene description
  void ParseOBJ(const std::string& file, float factor);
  // calculate scene bounding box
  void CalcBounds();

  const Box& GetSceneBox() const;

  void buildkDTree();
private:
  Scene(const Scene& );
  Scene& operator=(const Scene& );

  void parseOBJLine(const std::string& line);
  void parseVertex(std::istringstream& iss);
  void parseNormal(std::istringstream& iss);
  void parseTexCoord(std::istringstream& iss);
  void parseFace(std::istringstream& iss);
  void buildTriangleList(float scale);

  Camera* m_camera;
  // background color
  Vec3f m_bgColor;

  // primitives
  std::vector<Primitive*> m_primitives;
  // lights
  std::vector<Light*> m_lights;
  // textures
  std::vector<Texture*> m_textures;

  // shader used by loading routine
  Shader* m_shader;

  // storage for vertices and face indices
  std::vector<Vec3f> m_vertices, m_normals, m_texcoords, m_faces, m_normal_indices, m_tex_indices;
  Vec3f m_centroid;

  // scene bounding box
  Box m_scene_box;
  // kD-tree
  kDTree* m_tree;
};
#endif
