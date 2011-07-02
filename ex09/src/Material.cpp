#include "Material.h"

Material::Material() {
  mShaderProgram = NULL;
  setDiffuseColor();
  setAmbientColor();
  setSpecularColor();
  setShininess();
  mUniformsInitialized = false;
}

Material::~Material() {

}

void Material::setDiffuseColor(GLfloat r, GLfloat g, GLfloat b) {
  mDiffuseColor.active = (r >= 0);
  mDiffuseColor.color[0] = r;
  mDiffuseColor.color[1] = g;
  mDiffuseColor.color[2] = b;
  mUniformsInitialized = false;
}

void Material::setAmbientColor(GLfloat r, GLfloat g, GLfloat b) {
  mAmbientColor.active = (r >= 0);
  mAmbientColor.color[0] = r;
  mAmbientColor.color[1] = g;
  mAmbientColor.color[2] = b;
  mUniformsInitialized = false;
}

void Material::setEmissiveColor(GLfloat r, GLfloat g, GLfloat b) {
  mEmissiveColor.active = (r >= 0);
  mEmissiveColor.color[0] = r;
  mEmissiveColor.color[1] = g;
  mEmissiveColor.color[2] = b;
  mUniformsInitialized = false;
}

void Material::setSpecularColor(GLfloat r, GLfloat g, GLfloat b) {
  mSpecularColor.active = (r >= 0);
  mSpecularColor.color[0] = r;
  mSpecularColor.color[1] = g;
  mSpecularColor.color[2] = b;
  mUniformsInitialized = false;
}

void Material::setShininess(GLfloat s) {
  mSpecularColor.active = (s >= 0);
  mShininess.value = s;
  mUniformsInitialized = false;
}

void Material::setDiffuseTexture(const char *filename) {
  loadTextureData(filename, mTexture[DIFFUSE]);
}

void Material::setSpecularTexture(const char *filename) {
  loadTextureData(filename, mTexture[SPECULAR]);
}

void Material::setEmissiveTexture(const char *filename) {
  loadTextureData(filename, mTexture[EMISSIVE]);
}

void Material::setNormalTexture(const char *filename) {
  loadTextureData(filename, mTexture[NORMAL]);
}


void Material::setShaderProgram(Shader *shaderProgram) {
  mShaderProgram = shaderProgram;
  std::cerr << "(Material::setShaderProgram) - Set new shaderprogram (ID: " << mShaderProgram->getProgramID() << ")" << std::endl;
}

Shader *Material::getShaderProgram() {
  return mShaderProgram;
}

void Material::initUniforms() {
  if (mShaderProgram) {
    // init texture uniforms if needed //
    const char* textureUniformNames[LAYER_COUNT] = {"diffuseTex", "specularTex", "emissiveTex", "normalTex"};
    for (unsigned int layer = 0; layer < LAYER_COUNT; ++layer) {
      if (mTexture[layer].active) {
        mTexture[layer].uniformLocation = glGetUniformLocation(mShaderProgram->getProgramID(), textureUniformNames[layer]);
        if (mTexture[layer].uniformLocation < 0) {
          std::cerr << "(Material::initUniforms) - Could not access uniform location \"" << textureUniformNames[layer] << "\"" <<  std::endl;
        }
      } 
    }
    
    // init material-color uniforms (Phong) //
    if (mDiffuseColor.active) {
      mDiffuseColor.uniformLocation = glGetUniformLocation(mShaderProgram->getProgramID(), "diffuseColor");
    }
    if (mAmbientColor.active) {
      mAmbientColor.uniformLocation = glGetUniformLocation(mShaderProgram->getProgramID(), "ambientColor");
    }
    if (mEmissiveColor.active) {
      mEmissiveColor.uniformLocation = glGetUniformLocation(mShaderProgram->getProgramID(), "emissiveColor");
    }
    if (mSpecularColor.active) {
      mSpecularColor.uniformLocation = glGetUniformLocation(mShaderProgram->getProgramID(), "specularColor");
    }
    if (mShininess.active) {
      mShininess.uniformLocation = glGetUniformLocation(mShaderProgram->getProgramID(), "shininessCoeff");
    }
    
    mUniformsInitialized = true;
  }
}

void Material::enable() {
  if (mShaderProgram != NULL && mShaderProgram->ready()) {
    mShaderProgram->enable();
    if (!mUniformsInitialized) {
      initUniforms();
    }
  }
  for (unsigned int layer = 0; layer < LAYER_COUNT; ++layer) {
    if (mTexture[layer].active) {
      glActiveTexture(GL_TEXTURE0 + layer);
      glBindTexture(GL_TEXTURE_2D, mTexture[layer].glTextureLocation);
      glUniform1i(mTexture[layer].uniformLocation, layer);
    }
  }
}

void Material::disable() {
  if (mShaderProgram != NULL) {
    mShaderProgram->disable();
  }
  for (unsigned int layer = 0; layer < LAYER_COUNT; ++layer) {
    glActiveTexture(GL_TEXTURE0 + layer);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

void Material::loadTextureData(const char *textureFile, Texture &texture) {
  IplImage *image = cvLoadImage(textureFile, CV_LOAD_IMAGE_COLOR);
  if (image != NULL) {
    texture.width = image->width;
    texture.height = image->height;
    texture.data = new unsigned char[image->imageSize];
    if (image->origin == 0) {
      // flip rows of the image from top to bottom //
      cvFlip(image);
    }
    memcpy(texture.data, image->imageData, image->imageSize);
    std::cout << "(Material::loadTextureData) - imported \"" << textureFile << "\" (" << texture.width << ", " << texture.height << ")" << std::endl;
    
    glEnable(GL_TEXTURE_2D);
    
    // init openGL texture //
    if (texture.glTextureLocation <= 0) {
      // texture has not been generated yet //
      glGenTextures(1, &texture.glTextureLocation);
    }
    glBindTexture(GL_TEXTURE_2D, texture.glTextureLocation);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, texture.width, texture.height, GL_BGR, GL_UNSIGNED_BYTE, texture.data);
    
    glDisable(GL_TEXTURE_2D);
    
    texture.active = true;
    
    mUniformsInitialized = false;
  }
  cvReleaseImage(&image);
}
