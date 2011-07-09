#ifndef __MATERIAL__
#define __MATERIAL__

#include <cstddef>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "Shader.h"

struct Texture {
  Texture() {
    active = false;
    data = NULL;
    width = 0;
    height = 0;
    glTextureLocation = 0;
    uniformLocation = -1;
  };
  bool active;
  unsigned char *data;
  unsigned int width, height;
  GLuint glTextureLocation;
  GLint uniformLocation;
};

struct MaterialColor {
  MaterialColor() {
    active = false;
    for (unsigned int i = 0; i < 3; ++i) {
      color[i] = 0;
    }
    uniformLocation = -1;
  }
  bool active;
  GLfloat color[3];
  GLint uniformLocation;
};

struct MaterialParam {
  MaterialParam() {
    active = false;
    value = 0;
    uniformLocation = -1;
  }
  bool active;
  GLfloat value;
  GLint uniformLocation;
};

class Material {
  public:
    Material();
    ~Material();
    
    void setDiffuseColor(GLfloat r = -1, GLfloat g = 0, GLfloat b = 0);
    void setAmbientColor(GLfloat r = -1, GLfloat g = 0, GLfloat b = 0);
    void setEmissiveColor(GLfloat r = -1, GLfloat g = 0, GLfloat b = 0);
    void setSpecularColor(GLfloat r = -1, GLfloat g = 0, GLfloat b = 0);
    void setShininess(GLfloat s = -1);
    
    void setDiffuseTexture(const char *filename);
    void setSpecularTexture(const char *filename);
    void setEmissiveTexture(const char *filename);
    void setNormalTexture(const char *filename);
    
    void setShaderProgram(Shader *shader);
    Shader *getShaderProgram();
    
    void enable();
    void disable();
    
  private:
    void loadTextureData(const char *textureFile, Texture &texture);
    void initUniforms();
    bool mUniformsInitialized;
    
    MaterialColor mDiffuseColor;
    MaterialColor mAmbientColor;
    MaterialColor mEmissiveColor;
    MaterialColor mSpecularColor;
    MaterialParam mShininess;
    
    enum TextureLayer {DIFFUSE = 0, SPECULAR, EMISSIVE, NORMAL, LAYER_COUNT};
    Texture mTexture[4];
    
    Shader *mShaderProgram;
};

#endif
