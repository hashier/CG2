#include <GL/glew.h>
#include <GL/glut.h>

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

char* loadShaderSource(const char* fileName);

void resizeGL(int w, int h);
void updateGL();
void idle();
void keyboardEvent(unsigned char key, int x, int y);
void mouseEvent(int button, int state, int x, int y);
void mouseMoveEvent(int x, int y);

void printLog(GLuint obj);

GLint windowWidth, windowHeight;
GLfloat zNear, zFar;
GLfloat fov;

Trackball trackball(0, 0, 10);
ObjLoader objLoader;

float innerAngle = 15.0f;
float outerAngle = 20.0f;

GLuint shaderProgram;
GLint uniform_innerSpotAngle;
GLint uniform_outerSpotAngle;

int main (int argc, char **argv) {
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  windowWidth = 512;
  windowHeight = 512;
  
  zNear = 1.0f;
  zFar = 50.0f;
  fov = 45.0f;

  glutInitWindowSize (windowWidth, windowHeight);
  glutInitWindowPosition (100, 100);
  glutCreateWindow("Exercise 03");
  
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
  objLoader.loadObjFile("./meshes/scene.obj", "scene");
  
  initGL();
  initShader();
  
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

void initShader() {
    GLchar * vertex;
    GLchar * fragment;

    // XXX: create a new shader program here and assign it to 'shaderProgram'      //
    shaderProgram = glCreateProgram();
    GLuint prog_vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint prog_fragment = glCreateShader(GL_FRAGMENT_SHADER);

    // XXX: use 'loadShaderSource' to load your vertex and fragment shader sources //
    //       create your shaders and attach them to yout shader program             //
    //       finally link your program to be able to use it whenever you want it    //
    vertex = loadShaderSource("shaders/vertex.s");
    fragment = loadShaderSource("shaders/fragment.s");

    glShaderSource(prog_vertex, 1, (const GLchar**) &vertex, NULL);
    glShaderSource(prog_fragment, 1, (const GLchar**) &fragment, NULL);

    glCompileShader(prog_vertex);
    glCompileShader(prog_fragment);

    glAttachShader(shaderProgram, prog_vertex);
    glAttachShader(shaderProgram, prog_fragment);

    glLinkProgram(shaderProgram);

    // XXX: init your uniform variables used in the shader                         //
    //       bind them to 'uniform_innerSpotAngle' and 'uniform_outerSpotAngle'     //
    //       make sure to use the EXACT same uniform name as in your shader file    //
    glUseProgram(shaderProgram);

    uniform_innerSpotAngle = glGetUniformLocation(shaderProgram, "uni_innerSpotAngle");
    uniform_outerSpotAngle = glGetUniformLocation(shaderProgram, "uni_outerSpotAngle");
    glUniform1f(uniform_innerSpotAngle, innerAngle);
    glUniform1f(uniform_outerSpotAngle, outerAngle);

    printLog(prog_vertex);
    printLog(prog_fragment);
    printLog(shaderProgram);

}

void updateGL() {
  // XXX: enable your shader program if not active yet //
  GLfloat aspectRatio = (GLfloat)windowWidth / windowHeight;
  glUseProgram(shaderProgram);

  // clear renderbuffer //
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glViewport(0, 0, windowWidth, windowHeight);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, aspectRatio, zNear, zFar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  trackball.rotateView();

  // XXX: update your uniform variables //
  glUniform1f(uniform_innerSpotAngle, innerAngle);
  glUniform1f(uniform_outerSpotAngle, outerAngle);

  // XXX: render your scene //
  objLoader.getMeshObj("scene")->render();

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
    case 'q':
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
    case 'r': {
      innerAngle += 0.1f;
      if (innerAngle > outerAngle) outerAngle = innerAngle;
      break;
    }
    case 'f': {
      innerAngle -= 0.1f;
      if (innerAngle < 0.5f) innerAngle = 0.5f;
      break;
    }
    case 't': {
      outerAngle += 0.1f;
      if (outerAngle > 90.0f) outerAngle = 90.0f;
      break;
    }
    case 'g': {
      outerAngle -= 0.1f;
      if (outerAngle < innerAngle) innerAngle = outerAngle;
      if (innerAngle < 0.5f) innerAngle = outerAngle = 0.5f;
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


void printLog(GLuint obj)
{
    int infologLength = 0;
    int maxLength;

    if(glIsShader(obj))
        glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&maxLength);
    else
        glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&maxLength);

    char infoLog[maxLength];

    if (glIsShader(obj))
        glGetShaderInfoLog(obj, maxLength, &infologLength, infoLog);
    else
        glGetProgramInfoLog(obj, maxLength, &infologLength, infoLog);

    if (infologLength > 0)
        printf("%s\n",infoLog);
}
