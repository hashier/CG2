#include <GL/glew.h>
#include <GL/glut.h>
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

void initTextures();
void loadTextureData(const char *fileName);

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

Trackball trackball(0, 0, 5);
ObjLoader objLoader;

bool shadersInitialized = false;
GLuint shaderProgram;
unsigned int textureIndex;
GLuint texture[3];
unsigned char *textureData;
unsigned int textureWidth, textureHeight;
GLint uniform_texture;

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
  glutCreateWindow("Exercise 05");
  
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
  objLoader.loadObjFile("./meshes/trashbin.obj", "trashbin");
  
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
  // load your shaders and link them to a program //
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
  glUseProgram(shaderProgram);
  
  // DONE: get your textute-uniform location here //

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture[0]);
  uniform_texture = glGetUniformLocation(shaderProgram, "tex0");
  glUniform1i(uniform_texture, 0);

}

void initTextures (void) {
  // load the texture for our object //
  loadTextureData("textures/trashbin.png");
  
  // DONE: generate three textures                                       //
  //       one for GL_NEAREST, one for GL_LINEAR and one MIPMAP texture  //
  //       pass the loaded texture data to each of these texture objects //
  glGenTextures(3, texture);

  glBindTexture(GL_TEXTURE_2D, texture[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

  glBindTexture(GL_TEXTURE_2D, texture[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

  glBindTexture(GL_TEXTURE_2D, texture[2]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
  glGenerateMipmap(GL_TEXTURE_2D);
//  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, textureWidth, textureHeight, GL_RGB, GL_UNSIGNED_BYTE, textureData);

  // init textureIndex to 0 //
  textureIndex = 0;
}

void loadTextureData(const char *textureFile) {
  // DONE: load the file given by 'textureFile' to the global array 'textureData' //
  //       hint: you can use 'cvLoadImage()' of OpenCV to import your file        //
  IplImage * image = cvLoadImage(textureFile);
  textureWidth = image->width;
  textureHeight = image->height;
  textureData = new unsigned char[textureWidth * textureHeight * 3];
  for (unsigned int i = 0; i < textureWidth * textureHeight * 3; i++)
    textureData[i] = image->imageData[i];
  cvReleaseImage(&image);
}

void updateGL() {
  // DONE: enable your shader //
  glUseProgram(shaderProgram);
  
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
  glBegin(GL_QUADS);
  glTexCoord2d(0.0, 0.0); glVertex3i( 0,  0, 4);
  glTexCoord2d(1.0, 0.0); glVertex3i(1,  0, 4);
  glTexCoord2d(1.0, 1.0); glVertex3i(1, 1, 4);
  glTexCoord2d(0.0, 1.0); glVertex3i( 0, 1, 4);
  glEnd();
  
  // TODO: enable a texture unit, bind your texture and upload it as uniform to your shader //
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture[textureIndex]);
//  uniform_texture = glGetUniformLocation(shaderProgram, "tex0");
  glUniform1i(uniform_texture, 0);
  
  // render scene //
  objLoader.getMeshObj("trashbin")->render();
  
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
    case '1': {
      textureIndex = 0;
      break;
    }
    case '2': {
      textureIndex = 1;
      break;
    }
    case '3': {
      textureIndex = 2;
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
