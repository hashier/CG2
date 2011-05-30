#include <GL/glew.h>
#include <GL/glut.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "MeshObj.h"
#include "ObjLoader.h"
#include "Trackball.h"

void initGL();
void initShader();
void initUniforms();
void enableShader();
void disableShader();

void resizeGL(int w, int h);
void updateGL();
void idle();
void keyboardEvent(unsigned char key, int x, int y);
void mouseEvent(int button, int state, int x, int y);
void mouseMoveEvent(int x, int y);

void invertRotTransMat(float *mat, float *inv);
void invertPerspectiveMat(float *mat, float *inv);

void renderScene();

GLint windowWidth, windowHeight;
GLfloat zNear, zFar;
GLfloat fov;

Trackball trackball(0, 0, 1);
ObjLoader objLoader;

bool shadersInitialized = false;
GLuint shaderProgram;

// this is a container for texture data, OpenGL and GLSL locations //
struct Texture {
  unsigned char *data;
  unsigned int width, height;
  GLuint glTextureLocation;
  GLint uniformLocation;
};
// storage for your local textures //
enum TextureLayer {DIFFUSE = 0, EMISSIVE, SKY_ALPHA, SKY_COLOR, LAYER_COUNT};
Texture texture[LAYER_COUNT];

void initTextures();
void loadTextureData(const char *fileName, Texture &texture);

int main (int argc, char **argv) {
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  windowWidth = 512;
  windowHeight = 512;
  
  zNear = 0.1f;
  zFar = 10000.0f;
  fov = 45.0f;

  glutInitWindowSize (windowWidth, windowHeight);
  glutInitWindowPosition (100, 100);
  glutCreateWindow("Exercise 06");
  
  glutReshapeFunc(resizeGL);
  glutDisplayFunc(updateGL);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboardEvent);
  glutMouseFunc(mouseEvent);
  glutMotionFunc(mouseMoveEvent);
  
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  
  // load obj file here //
  objLoader.loadObjFile("./meshes/sphere.obj", "earth", 1);
  //objLoader.loadObjFile("./meshes/triangle.obj", "earth", 1);
  
  initGL();
  initShader();
  
  initTextures();
  
  glutMainLoop();
  
  return 0;
}

void initGL() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  
  // set projectionmatrix
  glMatrixMode(GL_PROJECTION);
  gluPerspective(fov, 1.0, zNear, zFar);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

char* loadShaderSource(const char* fileName) {
  std::ifstream file(fileName, std::ios::in);
  if (file.good()) {
    unsigned long srcLength = 0;
    file.tellg();
    file.seekg(0, std::ios::end);
    srcLength = file.tellg();
    file.seekg(0, std::ios::beg);
    
    char *srcData = new char[srcLength + 1];
    srcData[srcLength] = 0;
    
    unsigned long i = 0;
    while (file.good()) {
      srcData[i] = file.get();
      if (!file.eof()) {
        ++i;
      }
    }
    file.close();
    srcData[i] = 0;
    
    // return shader source //
    return srcData;
  } else {
    std::cout << "(loadShaderSource) - Could not open file \"" << fileName << "\"." << std::endl;
    return NULL;
  }
}

GLuint loadShaderFile(const char* fileName, GLenum shaderType) {
  GLuint shader = glCreateShader(shaderType);
  if (shader == 0) {
    return 0;
  }
  
  const char* shaderSrc = loadShaderSource(fileName);
  if (shaderSrc == NULL) return 0;
  glShaderSource(shader, 1, (const char**)&shaderSrc, NULL);
  
  glCompileShader(shader);
  
  int logMaxLength;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logMaxLength);
  char log[logMaxLength];
  int logLength = 0;
  glGetShaderInfoLog(shader, logMaxLength, &logLength, log);
  if (logLength > 0) {
    std::cout << "(loadShaderFile) - Compiler log:\n------------------\n" << log << "\n------------------" << std::endl;
  }
  
  return shader;
}

void initShader() {
  shadersInitialized = false;
  
  shaderProgram = glCreateProgram();
  
  GLuint vertexShader = loadShaderFile("shader/textureShader.vert", GL_VERTEX_SHADER);
  if (vertexShader == 0) {
    std::cout << "(initShader) - Could not create vertex shader." << std::endl;
    glDeleteProgram(shaderProgram);
    return;
  }
  GLuint fragmentShader = loadShaderFile("shader/textureShader.frag", GL_FRAGMENT_SHADER);
  if (fragmentShader == 0) {
    std::cout << "(initShader) - Could not create vertex shader." << std::endl;
    glDeleteProgram(shaderProgram);
    return;
  }
  
  // loaded shaders seem to work -> attach them to program //
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  
  // link final program //
  glLinkProgram(shaderProgram);
  
  // get log //
  int logMaxLength;
  glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logMaxLength);
  char log[logMaxLength];
  int logLength = 0;
  glGetShaderInfoLog(shaderProgram, logMaxLength, &logLength, log);
  if (logLength > 0) {
    std::cout << "(initShader) - Linker log:\n------------------\n" << log << "\n------------------" << std::endl;
  }
  
  initUniforms();
}

void initUniforms(void) {
  enableShader();
  
  // TODO: init your texture uniforms here //
  texture[0].uniformLocation = glGetUniformLocation(shaderProgram, "earthmap");
  texture[1].uniformLocation = glGetUniformLocation(shaderProgram, "earthspec");
  texture[2].uniformLocation = glGetUniformLocation(shaderProgram, "earthcloudmap");
  texture[3].uniformLocation = glGetUniformLocation(shaderProgram, "earthcloudmaptrans");
  texture[4].uniformLocation = glGetUniformLocation(shaderProgram, "earthlights");
  
//  texture[0].uniformLocation = glGetUniformLocation(shaderProgram, "earth_day");
//  texture[1].uniformLocation = glGetUniformLocation(shaderProgram, "specular");
//  texture[2].uniformLocation = glGetUniformLocation(shaderProgram, "clouds");
//  texture[3].uniformLocation = glGetUniformLocation(shaderProgram, "clouds_alpha");
//  texture[4].uniformLocation = glGetUniformLocation(shaderProgram, "earth_night");
  
  disableShader();
}

void initTextures (void) {
  // TODO: load the textures for the needed material layers from the files in './textures/ into your local texture objects //
  loadTextureData("textures/earthmap1k.jpg", texture[0]);
  loadTextureData("textures/earthspec1k.jpg", texture[1]);
  loadTextureData("textures/earthcloudmap.jpg", texture[2]);
  loadTextureData("textures/earthcloudmaptrans.jpg", texture[3]);
  loadTextureData("textures/earthlights1k.jpg", texture[4]);
  
  // TODO: initialize OpenGL textures for each taxture layer //
  glGenTextures(1, &texture[0].glTextureLocation);
  glGenTextures(1, &texture[1].glTextureLocation);
  glGenTextures(1, &texture[2].glTextureLocation);
  glGenTextures(1, &texture[3].glTextureLocation);
  glGenTextures(1, &texture[4].glTextureLocation);

  glBindTexture(GL_TEXTURE_2D, texture[0].glTextureLocation);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[0].width, texture[0].height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[0].data);

  glBindTexture(GL_TEXTURE_2D, texture[1].glTextureLocation);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[1].width, texture[1].height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[1].data);

  glBindTexture(GL_TEXTURE_2D, texture[2].glTextureLocation);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[2].width, texture[2].height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[2].data);

  glBindTexture(GL_TEXTURE_2D, texture[3].glTextureLocation);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[3].width, texture[3].height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[3].data);

  glBindTexture(GL_TEXTURE_2D, texture[4].glTextureLocation);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[4].width, texture[4].height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[4].data);
}

void loadTextureData(const char *textureFile, Texture &texture) {
  IplImage *image = cvLoadImage(textureFile, CV_LOAD_IMAGE_COLOR);
  if (image != NULL) {
    texture.width = image->width;
    texture.height = image->height;
    texture.data = new unsigned char[image->imageSize];
    if (image->origin == 0) {
      // flip rows of the image from top to bottom //
      for (unsigned int r = 0; r < texture.height; ++r) {
        for (unsigned int c = 0; c < texture.width; ++c) {
          texture.data[3 * ((texture.height - (r + 1)) * texture.width + c) + 0] = image->imageData[3 * (r * texture.width + c) + 0];
          texture.data[3 * ((texture.height - (r + 1)) * texture.width + c) + 1] = image->imageData[3 * (r * texture.width + c) + 1];
          texture.data[3 * ((texture.height - (r + 1)) * texture.width + c) + 2] = image->imageData[3 * (r * texture.width + c) + 2];
        }
      }
    } else {
      memcpy(texture.data, image->imageData, image->imageSize);
    }
  }
  cvReleaseImage(&image);
}

void enableShader() {
  glUseProgram(shaderProgram);
}

void disableShader() {
  glUseProgram(0);
}

void updateGL() {
  GLfloat aspectRatio = (GLfloat)windowWidth / windowHeight;
  
  // clear renderbuffer //
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glViewport(0, 0, windowWidth, windowHeight);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, aspectRatio, zNear, zFar);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  trackball.rotateView();
    
  // render earth //
  enableShader();
  
  // TODO: setup your sun-light //
  glEnable(GL_LIGHTING);
  GLfloat white_camera_spec_ptr[] = {1, 1, 1, 1};
  GLfloat position_camera_light[] = {1, 1, 1, 1};
  glLightfv(GL_LIGHT0, GL_POSITION, position_camera_light);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white_camera_spec_ptr);
  glLightfv(GL_LIGHT0, GL_AMBIENT, white_camera_spec_ptr);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white_camera_spec_ptr);
  glEnable(GL_LIGHT0);
  
  // TODO: enable texture units, bind textures and pass them to your shader //
  glEnable(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture[0].glTextureLocation);
  glUniform1i(texture[0].uniformLocation, 0);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture[1].glTextureLocation);
  glUniform1i(texture[1].uniformLocation, 1);
  
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, texture[2].glTextureLocation);
  glUniform1i(texture[2].uniformLocation, 2);
  
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, texture[3].glTextureLocation);
  glUniform1i(texture[3].uniformLocation, 3);
  
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, texture[4].glTextureLocation);
  glUniform1i(texture[4].uniformLocation, 4);
  
  objLoader.getMeshObj("earth")->render();
  
  disableShader();
  
  // swap render and screen buffer //
  glutSwapBuffers();
}

void idle() {
  glutPostRedisplay();
}

void resizeGL(int w, int h) {
  windowWidth = w;
  windowHeight = h;
  glutPostRedisplay();
}

void keyboardEvent(unsigned char key, int x, int y) {
  switch (key) {
    case 'x':
    case 27 : {
      exit(0);
      break;
    }
    case 'w': {
      // move forward //
      trackball.updateOffset(Trackball::MOVE_FORWARD);
      break;
    }
    case 's': {
      // move backward //
      trackball.updateOffset(Trackball::MOVE_BACKWARD);
      break;
    }
    case 'a': {
      // move left //
      trackball.updateOffset(Trackball::MOVE_LEFT);
      break;
    }
    case 'd': {
      // move right //
      trackball.updateOffset(Trackball::MOVE_RIGHT);
      break;
    }
    default : {
      break;
    }
  }
  glutPostRedisplay();
}

void mouseEvent(int button, int state, int x, int y) {
 Trackball::MouseState mouseState;
  if (state == GLUT_DOWN) {
    switch (button) {
      case GLUT_LEFT_BUTTON : {
        mouseState = Trackball::LEFT_BTN;
        break;
      }
      case GLUT_RIGHT_BUTTON : {
        mouseState = Trackball::RIGHT_BTN;
        break;
      }
      default : break;
    }
  } else {
    mouseState = Trackball::NO_BTN;
  }
  trackball.updateMouseBtn(mouseState, x, y);
}

void mouseMoveEvent(int x, int y) {
  trackball.updateMousePos(x, y);
}
