#include "Ex01.h"
#include "ObjLoader.h"
#include <iostream>

void initGL();
void resizeGL(int w, int h);
void updateGL();
void idle();
void keyboardEvent(unsigned char key, int x, int y);

std::vector<Vertex> vertexList;
std::vector<int> indexList;

int main (int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (512, 512);
  glutInitWindowPosition (100, 100);
  glutCreateWindow("Exercise 01");
  
  glutReshapeFunc(resizeGL);
  glutDisplayFunc(updateGL);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboardEvent);
  
  initGL();
  
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  
  // load "HelloOpenGL.obj" file here using loadObjFile(...) //
  loadObjFile("HelloOpenGL.obj", vertexList, indexList);
  
  glutMainLoop();
  
  return 0;
}

void initGL() {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glMatrixMode(GL_PROJECTION);
  gluPerspective(45.0f, 1.0, 0.01f, 100.0f);
  glMatrixMode(GL_MODELVIEW); 
}

void updateGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);

  // now render your imported object using renderVertexArray(...) //
  renderVertexArray(vertexList, indexList);
  
  glutSwapBuffers();
}

void idle() {
  glutPostRedisplay();
}

void resizeGL(int w, int h) {
  glViewport(0, 0, (GLint)w, (GLint)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)w/(GLfloat)h, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboardEvent(unsigned char key, int x, int y) {
  if (key == 27) {
    exit(0);
  }
  glutPostRedisplay();
}
