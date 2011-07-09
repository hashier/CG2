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
void renderShadow();
void renderScreenFillingQuad();

GLint windowWidth, windowHeight;
GLfloat zNear, zFar;
GLfloat fov;

Trackball trackball(0, 0.4, 1);
ObjLoader objLoader;

// local shader for blur rendering //
Shader *mBlurShader;

// FBO stuff //
GLuint fboTexture[2];
GLuint fboDepthTexture;
GLuint fbo;

float focusDepth = 0.5f;
float blurStrength = 5.0f;

// light //
GLfloat lightPos[4] = {0.0, 0.0, 0.0, 1.0};
bool lightSourcePosUpdate = true;

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
  glutCreateWindow("Exercise 09");
  
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

void initScene() {
  Shader *normalMapShader = new Shader("shader/normalMappingShader.vert", "shader/normalMappingShader.frag");
  objLoader.loadObjFile("./meshes/sphere.obj", "mars");
  Material *material = new Material();
  material->setShaderProgram(normalMapShader);
  material->setDiffuseTexture("textures/mars.png");
  material->setNormalTexture("textures/mars_normal.png");
  objLoader.getMeshObj("mars")->setMaterial(material);
  
  objLoader.loadObjFile("./meshes/sphere.obj", "moon", 0.8);
  material = new Material();
  material->setShaderProgram(normalMapShader);
  material->setDiffuseTexture("textures/moon.png");
  material->setNormalTexture("textures/moon_normal.png");
  objLoader.getMeshObj("moon")->setMaterial(material);
  
  mBlurShader = new Shader("shader/blurShader.vert", "shader/blurShader.frag");
}

void initFBO() {
  // init color textures //
  glGenTextures(2, fboTexture);
  for (unsigned int i = 0; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, fboTexture[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  }
  // init depth texture //
  glGenTextures(1, &fboDepthTexture);
  glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  
  // generate FBO and depthBuffer //
  glGenFramebuffers(1, &fbo);
  
  // attach textures to FBO //
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture[0], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fboTexture[1], 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepthTexture, 0);
  
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
  // enable shader //
  // light source //
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  
  // render scene into first color attachment of FBO -> use as filter texture later on //
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glDrawBuffer(GL_COLOR_ATTACHMENT0);
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  
  // render actual scene objects //
  for (unsigned int i = 0; i < 20; ++i) {
    glPushMatrix();
    glTranslatef(0.0, 0.0, i * -0.5);
    if (i % 2 == 0) {
      objLoader.getMeshObj("mars")->render();
    } else {
      objLoader.getMeshObj("moon")->render();
    }
    glPopMatrix(); 
  }
  
  // enable blur shader here //
  mBlurShader->enable();
  glDepthMask(GL_FALSE);
  glUniform1f(glGetUniformLocation(mBlurShader->getProgramID(), "blurStrength"), blurStrength);
  glUniform1f(glGetUniformLocation(mBlurShader->getProgramID(), "focusDepth"), focusDepth);
  glUniform1i(glGetUniformLocation(mBlurShader->getProgramID(), "width"), windowWidth);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
  glUniform1i(glGetUniformLocation(mBlurShader->getProgramID(), "depthImage"), 0);
  
  
  // render first (horizontal) blur pass to second color attachment in FBO //
  glDrawBuffer(GL_COLOR_ATTACHMENT1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  // update uniforms //
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, fboTexture[0]);
  glUniform1i(glGetUniformLocation(mBlurShader->getProgramID(), "inputImage"), 1);
  glUniform1i(glGetUniformLocation(mBlurShader->getProgramID(), "blurAxis"), 0);
  renderScreenFillingQuad();
  
  // vertical blur depending on depth -> final rendring to screen //
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  // update uniforms //
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, fboTexture[1]);
  glUniform1i(glGetUniformLocation(mBlurShader->getProgramID(), "inputImage"), 1);
  glUniform1i(glGetUniformLocation(mBlurShader->getProgramID(), "blurAxis"), 1);
  renderScreenFillingQuad();
  
  mBlurShader->disable();
  
  
  glDisable(GL_LIGHT0);
}

void renderShadow() {
  // init shadow volume if light source position has changed //
  if (lightSourcePosUpdate) {
    objLoader.getMeshObj("mars")->initShadowVolume(lightPos);
    lightSourcePosUpdate = false;
  }
  
  // draw nothing  to screen //
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);
  
  // enable stencil test and face culling //
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);
  
  // ## implementation of Carmack's reverse using depth-fail test ## //
  
  // first shadow pass -> render back facing parts                                      //
  // if object is in front of shadow volume back-face (depth test for that face fails), //
  // it might be in shadow -> increase stencil buffer                                   //
  glStencilFunc(GL_ALWAYS, 0x0, 0xFFFFFF);
  glStencilOp(GL_KEEP, GL_INCR, GL_KEEP);
  glFrontFace(GL_CW);
  objLoader.getMeshObj("mars")->renderShadowVolume();
  
  // second shadow pass -> render front facing parts                                      //
  // if depth test fails for front-face of shadow volume, the shadow volume is            //
  // completely behind the current object -> thus is is not in shadow -> decrease stencil //
  glStencilFunc(GL_ALWAYS, 0x0, 0xFFFFFF);
  glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);
  glFrontFace(GL_CCW);
  objLoader.getMeshObj("mars")->renderShadowVolume();
  
  // final pass -> render screen quad with current stencil buffer         //
  // disable face culling and re-enable writing to color and depth buffer //
  glDisable(GL_CULL_FACE);
  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  // set stencil operation to only execute, when stencil buffer is not equal to zero //
  glStencilFunc(GL_NOTEQUAL, 0x0, 0xFFFFFF);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  // enable blend function to prevent shadows from being pitch black      //
  // (uses alpha of color defined when rendering the screen filling quad) //
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  renderScreenFillingQuad();
  glDisable(GL_BLEND);
  // disable stencil testing for further rendering //
  glDisable(GL_STENCIL_TEST);
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
    case '+': {
      focusDepth += 0.01f;
      if (focusDepth > zFar) focusDepth = zFar;
      break;
    }
    case '-': {
      focusDepth -= 0.01f;
      if (focusDepth < zNear) focusDepth = zNear;
      break;
    }
    case '1': {
      blurStrength = 5.0f;
      break;
    }
    case '2': {
      blurStrength = 10.0f;
      break;
    }
    case '3': {
      blurStrength = 20.0f;
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