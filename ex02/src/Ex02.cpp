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

#define LETTER_SPACING 0.1f
#define EMPTY_LETTER_WIDTH 0.6f
#define LINE_HEIGHT 1.3f

void initGL();
void resizeGL(int w, int h);
void updateGL();
void idle();

// TODO: complete these callback functions at the end of this file //
void keyboardEvent(unsigned char key, int x, int y);
void mouseEvent(int button, int state, int x, int y);
void mouseMoveEvent(int x, int y);

// TODO: implement this method at the end of this file for rendering a text file //
void renderTextFile(const char *fileName);

// initialization of used tools //
Trackball trackball;
ObjLoader objLoader;
const char *fileName;

int main (int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Please specify the text file to be loaded..." << std::endl;
    return 0;
  }
  fileName = argv[1];
  
  // init window //
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (512, 512);
  glutInitWindowPosition (100, 100);
  glutCreateWindow("Exercise 02");
  
  // setup callback functions //
  glutReshapeFunc(resizeGL);
  glutDisplayFunc(updateGL);
  glutIdleFunc(idle);
  // TODO: connect callback functions for keyboard and mouse events //
  
  initGL();
  
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  
  // load needed .obj files here (do not change this) //
  std::string availableSymbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZ!?.:,;_+-'()[]{}<>=*#%&|/\\\"0123456789";
  for (unsigned int i = 0; i < availableSymbols.length(); ++i) {
    std::stringstream sstr;
    sstr << "./meshes/_";
    if (availableSymbols[i] == '/') {
      sstr << "slash";
    } else if (availableSymbols[i] == '*') {
      sstr << "star";
    } else {
      sstr << availableSymbols[i];
    }
    sstr << ".obj";
    // load the file corresponding to the current character from disk as new MeshObj //
    objLoader.loadObjFile(sstr.str(), availableSymbols.substr(i, 1));
  }
  
  glutMainLoop();
  
  return 0;
}

void initGL() {
  // setup clear color to plain black //
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // tell OpenGL to use depth testing while rendering //
  glEnable(GL_DEPTH_TEST);
  // set projection matrix //
  glMatrixMode(GL_PROJECTION);
  gluPerspective(45.0f, 1.0, 0.01f, 100.0f);
  // switch to model view mode used for transforming geometry //
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void updateGL() {
  // clear render buffer //
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // reset all transformations //
  glLoadIdentity();
  
  // TODO: use your trackball to rotate the view here, i.e. call the rotateView() method of your trackball //
  
  // TODO: draw your text here //
  
  // swap render buffer and screen buffer //
  glutSwapBuffers();
}

void idle() {
  glutPostRedisplay();
}

void resizeGL(int w, int h) {
  // resize OpenGL viewport to new window dimensions //
  glViewport(0, 0, (GLint)w, (GLint)h);
  // set perspective transformation to new aspect ratio //
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)w/(GLfloat)h, 0.1f, 100.0f);
  // switch to model view mode used for transforming geometry //
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboardEvent(unsigned char key, int x, int y) {
  switch (key) {
    case 'x':
    case 27 : {
      exit(0);
      break;
    }
    // TODO: add code processing intended movement (forward, backward, left, right) and update the trackball accordingly //
  }
  // tell OpenGL to redraw everything since the viewport has changed //
  glutPostRedisplay();
}

void mouseEvent(int button, int state, int x, int y) {
  // this method is triggered whenever a mouse button is pressed or released //
  // TODO: forward events to the trackball //
}

void mouseMoveEvent(int x, int y) {
  // this method is triggered whenever the mouse is moved //
  // TODO: forward movement to the trackball to control viewing changes //
}

void renderTextFile(const char *fileName) {
  // TODO: read in and render a text file here //
  /* - use your .obj loader to get the mesh corresponding a given character and render it
     - translate sideways by the last characters width
     - whenever a linebreak occurs, discard all previous translations
       and translate on the y-axis to begin a new line (hint: glPushMatrix(), glPopMatrix()) */
}
