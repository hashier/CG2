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

#include "Shader.h"
#include "MeshObj.h"
#include "ObjLoader.h"
#include "Trackball.h"
#include "Path.h"
#include "Timer.h"

void initGL();
void initScene();
void initFBO();

void resizeGL(int w, int h);
void updateGL();
void idle();
void keyboardEvent(unsigned char key, int x, int y);
void mouseEvent(int button, int state, int x, int y);
void mouseMoveEvent(int x, int y);

void renderScene();
void renderScreenFillingQuad();

GLint windowWidth, windowHeight;
GLfloat zNear, zFar;
GLfloat fov;

Trackball trackball(0, 0.4, 2+5);
ObjLoader objLoader;

// shaders //
Shader *mNormalMapShader;
// TODO: add other necessary shaders //

// FBO stuff //
// TODO: add OpenGL textures to be attached to your FBO //
GLuint fbo;

// light //
GLfloat lightPos[4] = {0.0, 0.0, 0.0, 1.0};

// Path //
Path mPath;

// Timer //
Timer mTimer;

int main (int argc, char **argv) {
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  
  windowWidth = 512;
  windowHeight = 512;
  
  zNear = 0.1f;
  zFar = 1000.0f;
  fov = 45.0f;

  glutInitWindowSize (windowWidth, windowHeight);
  glutInitWindowPosition (100, 100);
  glutCreateWindow("Exercise 10");
  
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
  
  initGL();
  initFBO();
  initScene();
  
  // this path defines a looped circular curve //
  std::vector<ControlPoint> p;
  p.push_back(ControlPoint( 1.0, 0.0, 0.0, 0.0));
  p.push_back(ControlPoint( 0.7, 0.0, 0.7, 0.5));
  p.push_back(ControlPoint( 0.0, 0.0, 1.0, 1.0));
  p.push_back(ControlPoint(-0.7, 0.0, 0.7, 1.5));
  p.push_back(ControlPoint(-1.0, 0.0, 0.0, 2.0));
  p.push_back(ControlPoint(-0.7, 0.0,-0.7, 2.5));
  p.push_back(ControlPoint( 0.0, 0.0,-1.0, 3.0));
  p.push_back(ControlPoint( 0.7, 0.0,-0.7, 3.5));
  p.push_back(ControlPoint( 1.0, 0.0, 0.0, 4.0));
  
  mPath.setFirstControlPoint(p[p.size() - 2]);
  mPath.setLastControlPoint(p[1]);
  for (unsigned int i = 0; i < p.size(); ++i) {
    mPath.addIntermediateControlPoint(p[i]);
  }
  mPath.setLooped(true);
  // ----------------------------------------- // 

  std::cout << mPath.getPositionForTime(3.999) << std::endl;
  std::cout << mPath.getPositionForTime(4.001) << std::endl;
//  return 0;
  
  glutMainLoop();
  
  delete mNormalMapShader;
  
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
  
  mTimer.start();
}

void initScene() {
  mNormalMapShader = new Shader("shader/universalShader.vert", "shader/universalShader.frag");
  
  Material *mat;
  
  objLoader.loadObjFile("./meshes/sphere.obj", "sun", 2.0f);
  mat = new Material();
  mat->setShaderProgram(mNormalMapShader);
  mat->setEmissiveTexture("textures/sun.jpg");
  objLoader.getMeshObj("sun")->setMaterial(mat);
  
  objLoader.loadObjFile("./meshes/sphere.obj", "mars", 0.5f);
  mat = new Material();
  mat->setShaderProgram(mNormalMapShader);
  mat->setDiffuseTexture("textures/mars.png");
  mat->setNormalTexture("textures/mars_normal.png");
  mat->setAmbientColor(0.15, 0.15, 0.15);
  objLoader.getMeshObj("mars")->setMaterial(mat);
  
  objLoader.loadObjFile("./meshes/sphere.obj", "moon", 0.2f);
  mat = new Material();
  mat->setShaderProgram(mNormalMapShader);
  mat->setDiffuseTexture("textures/moon.png");
  mat->setNormalTexture("textures/moon_normal.png");
  mat->setAmbientColor(0.15, 0.15, 0.15);
  objLoader.getMeshObj("moon")->setMaterial(mat);
  
  
  // TODO: init you additional shaders here (you might want to use simple.vert as vertex shader) //
}

void initFBO() {
  // TODO: init your FBO here //
  
  // unbind FBO until it's needed //
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateGL() {
  GLfloat aspectRatio = (GLfloat)windowWidth / windowHeight;

  // clear renderbuffer //
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glViewport(0, 0, windowWidth, windowHeight);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fov, aspectRatio, zNear, zFar);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  trackball.rotateView();
  
  // render  //
  renderScene();
  
  // swap render and screen buffer //
  glutSwapBuffers();
}

void renderScene() {
  // light source //
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  
  // TODO: render your scene using the material shaders to your FBO into the first color attachment //
  //       only render the sun a depth values before rendering the other planets fully colored      //
  //       use mPath and mTimer to compute the planet's positions                                   //
  // Matrix zurück setzen, dann evtl Rotation und Sonne
//  glutSolidSphere(1,10,10);
//  glutWireSphere(1,10,10);
  objLoader.getMeshObj("sun")->render();
  // Rotation löschen, dafür Translation und Rotation um Sonne für Mars dann noch Mars rotieren
  double time = mTimer.getTime();
  std::cout << "Mars" << std::endl;
  ControlPoint cp = mPath.getPositionForTime(time/2);
  const float radius_mars = 2;
  glTranslatef(cp.pos[0] * radius_mars, cp.pos[1] * radius_mars, cp.pos[2] * radius_mars);
  objLoader.getMeshObj("mars")->render();
  // Letzte Rotation löschen und Translation und Rotation für Mond dann noch Mond rotieren
  std::cout << "Marsmond" << std::endl;
  cp = mPath.getPositionForTime(1.4*time/2);
  const float radius_moon = 0.3;
  glTranslatef(cp.pos[0] * radius_moon, cp.pos[1] * radius_moon, cp.pos[2] * radius_moon);
  objLoader.getMeshObj("moon")->render();

#if 0
#endif
  
  // TODO: keep the current depth map and render the visible parts of sun to the second color attachment //
  
  // TODO: render blooming effect using a technique like in ex09 //
  
  // TODO: composite the final image by adding the first rendering and the bloom-filtered rendering //
  
  glDisable(GL_LIGHT0);
}

void renderScreenFillingQuad() {
  // save current matrices and setup orthogonal view frustum from 0 to 1 //
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, 0, 1);
  // disable depth testing -> draw over everything //
  glDisable(GL_DEPTH_TEST);
  // draw one black quad filling view frustum //
  glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
  glBegin(GL_QUADS);
      glTexCoord2f(0, 0);
      glVertex2f(0, 0);
      glTexCoord2f(1, 0);
      glVertex2f(1, 0);
      glTexCoord2f(1, 1);
      glVertex2f(1, 1);
      glTexCoord2f(0, 1);
      glVertex2f(0, 1);
  glEnd();
  // re-enable depth testing for further rendering //
  glEnable(GL_DEPTH_TEST);
  // reload old matrices back to their stacks //
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
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
