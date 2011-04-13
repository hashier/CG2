#include <sstream>
#include <fstream>


#include "Scene.hxx"
#include "PerspectiveCamera.hxx"
#include "EyeLightShader.hxx"
#include "Triangle.hxx"
#include "SmoothTriangle.hxx"
#include "TexturedSmoothTriangle.hxx"

Scene::Scene()
  : m_camera(new PerspectiveCamera(Vec3f(0,0,8),
				   Vec3f(0,0,-1),
				   Vec3f(0,1,0),
				   60,
				   640,
				   480)
	     ),
    m_bgColor(Vec3f(0,0,0)),
    m_shader(0),
    m_scene_box(Box()),
    m_tree(0)
{
}

Scene::~Scene()
{
  delete m_tree;
  delete m_camera;
  if(m_shader != 0)
    delete m_shader;

  for(unsigned int i = 0; i < m_primitives.size(); ++i)
    {
      delete m_primitives[i];
    }
  m_primitives.clear();
  for(unsigned int i = 0; i < m_textures.size(); ++i)
    {
      delete m_textures[i];
    }
  m_textures.clear();
}

Scene::Scene(const Scene& s)
{
  operator=(s);
}

Scene&
Scene::operator=(const Scene& s)
{
  return *this;
}

void
Scene::Add(Primitive* p)
{
  m_primitives.push_back(p);
}

void
Scene::Add(Light* l)
{
  m_lights.push_back(l);
}

void
Scene::Add(Texture* t)
{
  m_textures.push_back(t);
}

bool
Scene::Intersect(Ray& ray)
{
  if(!m_tree)
    {
      bool intersect = false;

      for(unsigned int i = 0; i < m_primitives.size(); ++i)
	{
	  intersect |= m_primitives[i]->Intersect(ray);
	}
      return intersect;
    }
  else
    {
      return m_tree->Intersect(ray);
    }
}

bool
Scene::Occluded(Ray& ray)
{
  return this->Intersect(ray);
}

Vec3f
Scene::RayTrace(Ray& ray)
{
  bool intersect = this->Intersect(ray);
  return (intersect) ? ray.hit()->shader()->Shade(ray) : m_bgColor;
}

const Camera*
Scene::camera() const
{
  return m_camera;
}

std::vector<Light*>
Scene::lights() const
{
  return m_lights;
}

std::vector<Texture*>
Scene::textures() const
{
  return m_textures;
}

void
Scene::setCamera(const Camera* camera)
{
  if(m_camera != 0)
    delete m_camera;
  m_camera = const_cast<Camera*>(camera);
}

void
Scene::ParseOBJ(const std::string& file, float scale)
{
  std::cerr << "(Scene): Parsing OBJ file : " << file << std::endl;

  // clear old buffers
  m_vertices.clear();
  m_normals.clear();
  m_texcoords.clear();
  m_faces.clear();
  m_normal_indices.clear();
  m_tex_indices.clear();

  // for the moment, we will attach a white eyelight shader to each object
  // in the future, you will extend your parser to also read in material definitiions
  if(m_shader == 0) // not yet defined
    m_shader = new EyeLightShader(this, Vec3f(1.0,1.0,1.0));


  // now open file
  std::fstream in;
  in.open(file.c_str(), std::ios::in);
  if(in.bad() || in.fail())
    {
      std::cerr << "(Scene): Could not open file " << file << std::endl;
      return;
    }

  // read lines
  std::string line;
  while(!in.eof())
    {
      std::getline(in, line);
      this->parseOBJLine(line);
    }

  // finished parsing file -> close fileStream
  in.close();

  // build triangle list from parsed vertices
  this->buildTriangleList(scale);

  std::cerr << "(Scene): Finished parsing." << std::endl;
  std::cerr << "(Scene): Building acceleration structure...";
  this->buildkDTree();
  std::cerr << "done." << std::endl;

}


void
Scene::parseOBJLine(const std::string& line)
{
  std::istringstream iss(line);
  std::string key;
  iss >> key;
  if (key == "v")
    {
      // parse vertex //
      this->parseVertex(iss);
    }
  else if(key == "vn")
    {
      this->parseNormal(iss);
    }
  else if(key == "vt")
    {
      this->parseTexCoord(iss);
    }
  else if (key == "f")
    {
      // parse face //
      this->parseFace(iss);
    }
}

void
Scene::parseVertex(std::istringstream& iss)
{
  Vec3f v;
  iss >> v[0] >> v[1] >> v[2];

  m_vertices.push_back(v);
  m_centroid += v;
}

void
Scene::parseNormal(std::istringstream& iss)
{
  Vec3f n;
  iss >> n[0] >> n[1] >> n[2];

  m_normals.push_back(n);
}

void
Scene::parseTexCoord(std::istringstream& iss)
{
  Vec3f t;
  iss >> t[0] >> t[1];

  m_texcoords.push_back(t);
}

void
Scene::parseFace(std::istringstream& iss)
{
  Vec3f f, t, n;
  bool tex = false;
  bool norm = false;
  char skip = 0;
  int vertexCount = 0;

  while (!iss.eof() && vertexCount < 3)
    {
      // read face index
      iss >> f[vertexCount];
      if (iss.fail())
	{
	  break;
	}

      // look ahead whether there is texture index or normal index
      if (iss.peek() == '/')
	{
	  iss >> skip;

	  // skip, if no texCoord is specified //
	  if (iss.peek() == '/')
	    {
	      norm = true;
	      iss >> skip >> n[vertexCount];
	      if (iss.fail())
		break;
	    }
	  else
	    {
	      tex = true;
	      iss >> t[vertexCount];
	      if (iss.fail())
		break;
	    }

	  if (iss.peek() == '/')
	    {
	      norm = true;
	      iss >> skip >> n[vertexCount];
	      if (iss.fail())
		break;
	    }
	}
      vertexCount++;
    }

  if (vertexCount < 3)
    {
      // this is not a valid face, since it has less than 3 vertices
      std::cerr << "(Scene::parseFace): WARNING: Malformed face! Not enough vertices defined." << std::endl;
      return;
    }

  m_faces.push_back(f);
  if(tex)
    {
      m_tex_indices.push_back(t);
    }
  if(norm)
    {
      m_normal_indices.push_back(n);
    }
}

void
Scene::buildTriangleList(float fac)
{
  bool vertex_normals = (m_normal_indices.size() > 0);
  bool texture_coords = (m_tex_indices.size() > 0);

  for(unsigned int f = 0; f < m_faces.size(); ++f)
    {
      // stores indices of triangle into vertex list
      // remember: indices start at 1!!
      if(!vertex_normals)
	{
	  Vec3f face_idx = m_faces[f];
	  this->Add(new Triangle(m_vertices[ static_cast<int>(face_idx[0])-1 ] * fac,
				 m_vertices[ static_cast<int>(face_idx[1])-1 ] * fac,
				 m_vertices[ static_cast<int>(face_idx[2])-1 ] * fac,
				 m_shader));
	}
      else if(vertex_normals && !texture_coords)
	{
	  Vec3f face_idx = m_faces[f];
	  Vec3f norm_idx = m_normal_indices[f];
	  this->Add(new SmoothTriangle(m_vertices[ static_cast<int>(face_idx[0])-1 ] * fac,
				       m_vertices[ static_cast<int>(face_idx[1])-1 ] * fac,
				       m_vertices[ static_cast<int>(face_idx[2])-1 ] * fac,
				       m_normals[ static_cast<int>(norm_idx[0])-1 ],
				       m_normals[ static_cast<int>(norm_idx[1])-1 ],
				       m_normals[ static_cast<int>(norm_idx[2])-1 ],
				       m_shader));
	}
      else
	{
	  Vec3f face_idx = m_faces[f];
	  Vec3f norm_idx = m_normal_indices[f];
	  Vec3f tex_idx = m_tex_indices[f];
	  this->Add(new TexturedSmoothTriangle(m_vertices[ static_cast<int>(face_idx[0])-1 ] * fac,
					       m_vertices[ static_cast<int>(face_idx[1])-1 ] * fac,
					       m_vertices[ static_cast<int>(face_idx[2])-1 ] * fac,
					       m_normals[ static_cast<int>(norm_idx[0])-1 ],
					       m_normals[ static_cast<int>(norm_idx[1])-1 ],
					       m_normals[ static_cast<int>(norm_idx[2])-1 ],
					       m_texcoords[ static_cast<int>(tex_idx[0])-1 ],
					       m_texcoords[ static_cast<int>(tex_idx[1])-1 ],
					       m_texcoords[ static_cast<int>(tex_idx[2])-1 ],
					       m_shader));
	}
    }
  m_centroid /= static_cast<float>(m_vertices.size());
  std::cerr << "(Scene): Model centroid = " << m_centroid * fac << std::endl;
  std::cerr << "(Scene): Model contains " << m_faces.size() << " triangles." << std::endl;
}

void
Scene::CalcBounds()
{
  m_scene_box.Clear();

  for(unsigned int i = 0; i< m_primitives.size(); ++i)
    {
      Box box = m_primitives[i]->CalcBounds();
      m_scene_box.Extend(box);
    }
}

const Box&
Scene::GetSceneBox() const
{
  return m_scene_box;
}

void
Scene::buildkDTree()
{
  if(m_tree != 0)
    {
      delete m_tree;
      m_scene_box.Clear();
    }

  this->CalcBounds();
  m_tree = new kDTree(m_scene_box, m_primitives);
}

void
Scene::setShader(const Shader* shader)
{
  for(unsigned int i = 0; i < m_primitives.size(); ++i)
    {
      m_primitives[i]->setShader(shader);
    }
}
