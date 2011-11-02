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

Trackball trackball(0, 0.4, 2);
ObjLoader objLoader;

// shaders //
Shader *mNormalMapShader;
Shader *mBloomShader;
Shader *mCombinationShader;
GLint blurRadius = 5;

// FBO stuff //
GLuint fboTexture[3];
GLuint fbo;

// light //
GLfloat lightPos[4] = {0.0, 0.0, 0.0, 1.0};
bool lightSourcePosUpdate = true;

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
  mat->setAmbientColor(0.1, 0.1, 0.1);
  objLoader.getMeshObj("mars")->setMaterial(mat);
  
  objLoader.loadObjFile("./meshes/sphere.obj", "moon", 0.2f);
  mat = new Material();
  mat->setShaderProgram(mNormalMapShader);
  mat->setDiffuseTexture("textures/moon.png");
  mat->setNormalTexture("textures/moon_normal.png");
  mat->setAmbientColor(0.1, 0.1, 0.1);
  objLoader.getMeshObj("moon")->setMaterial(mat);
  
  mBloomShader = new Shader("shader/simple.vert", "shader/bloomShader.frag");
  
  mCombinationShader = new Shader("shader/simple.vert", "shader/combination.frag");
}

void initFBO() {
  // init color textures //
  glGenTextures(3, fboTexture);
  for (unsigned int i = 0; i < 3; ++i) {
    glBindTexture(GL_TEXTURE_2D, fboTexture[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  }
  
  // generate FBO and depthBuffer //
  glGenFramebuffers(1, &fbo);
  
  // attach textures to FBO //
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture[0], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fboTexture[1], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, fboTexture[2], 0);
  
  // attach depthbuffer //
  GLuint depthbuffer;
  glGenRenderbuffers(1, &depthbuffer);
  glBindRenderbuffer(GL_RENDERBUFFER, depthbuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbuffer);
  
  GLuint error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  std::cerr << "FBO status(" << error << ")" << std::endl;
  if (error != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "FBO not ready" << std::endl;
  }
  
  // unbind FBO until it's needed //
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void updateGL() {
  GLfloat aspectRatio = (GLfloat)windowWidth / windowHeight;
  
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
  // enable shader //
  // light source //
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  
  // render all EXCEPT sun //
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  // render sun only to the depth buffer //
  objLoader.getMeshObj("sun")->render();
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  
  double t = mTimer.getTime();
  glPushMatrix();
  ControlPoint P = mPath.getPositionForTime(t);
  glTranslatef(P.pos[0], P.pos[1], P.pos[2]);
  objLoader.getMeshObj("mars")->render();
  glPushMatrix();
  P = mPath.getPositionForTime(0.2 * t + 0.5);
  glTranslatef(0.3 * P.pos[0], 0.3 * P.pos[1], 0.3 * P.pos[2]);
  objLoader.getMeshObj("moon")->render();
  glPopMatrix();
  glPopMatrix();
  
  // keep depth map and render visible parts of sun //
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glDepthFunc(GL_LEQUAL);
  objLoader.getMeshObj("sun")->render();
  glPolygonOffset(0, 0);
  glDepthFunc(GL_LESS);
  
  // render bloom effect //
  mBloomShader->enable();
  glUniform1i(mBloomShader->getUniformLocation("blurRadius"), blurRadius);
  glUniform1i(mBloomShader->getUniformLocation("width"), windowWidth);
  
  glDrawBuffer(GL_COLOR_ATTACHMENT2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glUniform1i(mBloomShader->getUniformLocation("blurAxis"), 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fboTexture[1]);
  glUniform1i(mBloomShader->getUniformLocation("inputImage"), 0);
  renderScreenFillingQuad();
  
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glUniform1i(mBloomShader->getUniformLocation("blurAxis"), 1);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fboTexture[2]);
  glUniform1i(mBloomShader->getUniformLocation("inputImage"), 0);
  renderScreenFillingQuad();
  mBloomShader->disable();
  
  // composite final image //
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  mCombinationShader->enable();
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fboTexture[0]);
  glUniform1i(mCombinationShader->getUniformLocation("inputImage0"), 0);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, fboTexture[1]);
  glUniform1i(mCombinationShader->getUniformLocation("inputImage1"), 1);
  
  renderScreenFillingQuad();
  
  mCombinationShader->disable();
  
  for (unsigned int i = 0; i < 2; ++i) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  
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