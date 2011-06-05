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

void initTextures();

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

Trackball trackball(0, 0.6, 1);
ObjLoader objLoader;

bool shadersInitialized = false;
GLuint shaderProgram;
GLint b_attr, t_attr;

struct Texture {
  unsigned char *data;
  unsigned int width, height;
  GLuint glTextureLocation;
  GLint uniformLocation;
};
enum TextureLayer {DIFFUSE = 0, NORMAL_MAP, LAYER_COUNT};
Texture texture[LAYER_COUNT];

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
  glutCreateWindow("Exercise 07");
  
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
  objLoader.loadObjFile("./meshes/sphere.obj", "planet");
  
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
  
  GLuint vertexShader = loadShaderFile("shader/normalMapShader.vert", GL_VERTEX_SHADER);
  if (vertexShader == 0) {
    std::cout << "(initShader) - Could not create vertex shader." << std::endl;
    glDeleteProgram(shaderProgram);
    return;
  }
  GLuint fragmentShader = loadShaderFile("shader/normalMapShader.frag", GL_FRAGMENT_SHADER);
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
  
  // DONE: assign the used texture uniforms here //
  b_attr = glGetAttribLocation(shaderProgram, "bitangent");
  t_attr = glGetAttribLocation(shaderProgram, "tangent");

  texture[DIFFUSE].uniformLocation = glGetUniformLocation(shaderProgram, "diffuseTex");
  texture[NORMAL_MAP].uniformLocation = glGetUniformLocation(shaderProgram, "normalTex");
  
  disableShader();
}

void initTextures (void) {
  // DONE: init your textures here //
  loadTextureData("textures/mars.png", texture[DIFFUSE]);
  loadTextureData("textures/mars_normal.png", texture[NORMAL_MAP]);

  for (unsigned int layer = DIFFUSE; layer < LAYER_COUNT; ++layer) {
    glGenTextures(1, &texture[layer].glTextureLocation);
    glBindTexture(GL_TEXTURE_2D, texture[layer].glTextureLocation);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture[layer].width, texture[layer].height, 0, GL_BGR, GL_UNSIGNED_BYTE, texture[layer].data);
  }
}

void loadTextureData(const char *textureFile, Texture &texture) {
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
    std::cout << "(loadTextureData) - imported \"" << textureFile << "\" (" << texture.width << ", " << texture.width << ")" << std::endl;
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
    
  // render planet //
  enableShader();
  
  // DONE: enable and upload textures //
  for (unsigned int layer = DIFFUSE; layer < LAYER_COUNT; ++layer) {
    glActiveTexture(GL_TEXTURE0 + layer);
    glBindTexture(GL_TEXTURE_2D, texture[layer].glTextureLocation);
    glUniform1i(texture[layer].uniformLocation, layer);
  }

  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex3i( 0, -10, -4);
  glTexCoord2d(1.0, 0.0); glVertex3i(10, -10, -4);
  glTexCoord2d(1.0, 1.0); glVertex3i(10, 0, -4);
  glTexCoord2d(0.0, 1.0); glVertex3i( 0, 0, -4);
  glEnd();
  
  // render object //
  objLoader.getMeshObj("planet")->render(t_attr, b_attr);
  
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
