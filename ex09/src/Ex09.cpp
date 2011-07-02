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
// TODO: implement this method below //
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

// predefined FBO variables (Textures and FBO handle) //
GLuint fboTexture[2];
GLuint fboDepthTexture;
GLuint fbo;

// variabled controlling blur shader rendering //
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
  
  // TODO: initialize and load your blur shader here //
}

void initFBO() {
  // TODO: init OpenGL texture objects for color data and depth information          //
  // Note that a shader cannot write into a texture and simultaneously read from it. //
  // So when a color attachment of a FBO is enabled for writing, a shader cannot     //
  // access the attached texture for reading. Thus you may want to initialize TWO    //
  // textures that can be used in an alternating way (similar to ping-pong-buffering)//
  
  // TODO: create FBO //
  
  // TODO: attach textures to FBO //
  
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
  // TODO: render scene into first color attachment of FBO -> use as filter texture later on //
  
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
  
  // TODO: enable blur shader //
  
  // TODO: disable depth testing AND writing - we do not want to change the depth map recovered from the previous render pass //
  
  // TODO: upload needed uniforms and pass your textures to the shader //
  
  // TODO: render first (horizontal) blur pass to second color attachment in FBO //
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  renderScreenFillingQuad();
  
  // TODO: vertical blur depending on depth -> final rendering to screen //
  
  // TODO: update uniforms that may have changed (filter orientation, color texture data from last render pass) //
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  renderScreenFillingQuad();
  
  // TODO: disable blur shader and re-enable depth testing again //
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