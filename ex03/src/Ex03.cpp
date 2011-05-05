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
void keyboardEvent(unsigned char key, int x, int y);
void mouseEvent(int button, int state, int x, int y);
void mouseMoveEvent(int x, int y);

void invertRotTransMat(float *mat, float *inv);
void invertProjectionMat(float *mat, float *inv);

// TODO: implement the rendering of the scene below //
void renderScene();

// these varables control the viewport size and the frustum parameters //
GLint windowWidth, windowHeight;
GLfloat zNear, zFar;
GLfloat fov;

// TODO: create two separate trackballs here - one for the 1st person camera and one for the 3rd person camera //
// you may initialize each trackball with an initial viewing direction and offset to (0,0,0) //

// init objLoader //
ObjLoader objLoader;


// materials //

// TODO: setup 4 different materials here //
// define four different colored materials (e.g. red, green, blue and yellow) and try different parameters for ambient, specular and shininess exponent //

// lights //

// TODO: setup the colors of two lightsources here //
// one lightsource will be placed at a static position so define it's position here, too //
// create a white light source and a reddish one //

int main (int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  
  windowWidth = 1024;
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
  
  initGL();
  
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  
  // load obj file here //
  objLoader.loadObjFile("./meshes/camera.obj", "camera");
  
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

void updateGL() {
  GLint viewportWidth = windowWidth / 2;
  GLint viewportHeight = windowHeight;
  GLfloat aspectRatio = (GLfloat)viewportWidth / viewportHeight;
  
  GLfloat projectionMatrix[16];
  GLfloat projectionMatrix_inv[16];
  GLfloat modelviewMatrix[16];
  GLfloat modelviewMatrix_inv[16];
  
  // clear renderbuffer //
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // render left viewport (actual camera view / 1st person camera) //
  
  // TODO: enable lighting and smooth rendering here //
  
  // TODO: setup the viewport for the 1st person camera view here -> use the left half of the window //
  
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // TODO: setup the camera's frustum here using gluPerspective (use the parameters fov, aspectRatio, zNear and zFar, which are already defined) //
  // TODO: save the current projection matrix for later use when rendering the 3rd person view //
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  // init light in camera space //
  // TODO: init a light fixed to the camera's position here //
  // since we define the lights position 'before' any other transformations, the lights position //
  // will not be transformed by any following modelview transformations when arranging the scene //
  // enable lightsource 0 //
  // use the camera's own position to place the light in the scene //
  // load light properties from the definitions at the top of this file //
  
  // TODO: now rotate the view according to the camera's trackball //
  
  // init light in world space (scene-fixed position) //
  // TODO: enable another light source here //
  // this time the lightsource will be affected by the view-rotation of the camera //
  // and is therefore fixed in the observed scene //
  // use the position and light parameters of the second lightsource defined above //
  
  // TODO: save your current modelview matrix here //
  glGetFloatv(GL_MODELVIEW_MATRIX, modelviewMatrix);
  
  // now we render the actual scene //
  renderScene();
  
  
  // now that we are done rendering the left viewport let's continue with the right one //
  
  // TODO: setup the viewport to the second half of the window //
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // TODO: again setup the 3rd person camera frustum here //
  // but instead of using the values fov, zNear and zFar, use an opening angle of 45 deg, a near plane at 0.1f and the far plane at 1000 //
  // this is because the parameters used above should control only the first viewport and not the world space preview //
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // TODO: rotate your view using the world space trackball //
  
  // render scene //
  // TODO: render the same scene with the exact same lighting (world-space lighting) as before //
  
  // render camera //
  // TODO: now, to visualize the 1st person camera's position in world space, render a MeshObj at the camera's position //
  // hint: we have saved the camera's position relative to the scene in the modelview matrix //
  // -> make use of this matrix to place your camera model //
  // also: DISABLE any lighting from now on //
  // you may choose a plain color for the camera model //
  
  // render frustum //
  GLfloat frustumCorners[8][3] = {{-1,-1,-1},{ 1,-1,-1},{ 1, 1,-1},{-1, 1,-1},
                                  {-1,-1, 1},{ 1,-1, 1},{ 1, 1, 1},{-1, 1, 1}};
  // TODO: now we want to render a visualization of our camera's frustum (the one used in the left viewport) //
  // use the already given unit-cube given by frustumCorners to render GL_LINES for the frustums edges //
  // to transform the cube into the actual frustum you need to make use of the previously saved projection matrix //
  // hint: you also need the modelview matrix to place the frustum correctly (like you placed your camera model) //
  // after correct transformation render the edges of the frustum //
                                   
  // swap render and screen buffer //
  glutSwapBuffers();
}

void renderScene() {
  // TODO: render your scene here //
  // place four spheres at the following positions using 'glutSolidSphere(GLFloat radius, GLuint stacks, GLuint slices)'
  // 1st Sphere at (-3.000, 0.000, 0.000) //
  // 2nd Sphere at ( 3.000, 0.000, 0.000) //
  // 3rd Sphere at ( 0.000, 0.000,-5.196) //
  // 4th Sphere at ( 0.000, 4.905,-1.732) //
  // use a radius of 3 for all spheres, choose the stacks and slices parameters so that the spheres appear smooth //
  // and use one of the previously defined materials for each sphere //
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
  Trackball *trackball;
  // TODO: determine, which trackball should be updated //
  switch (key) {
    case 'x':
    case 27 : {
      exit(0);
      break;
    }
    case 'w': {
      // move forward //
      trackball->updateOffset(Trackball::MOVE_FORWARD);
      break;
    }
    case 's': {
      // move backward //
      trackball->updateOffset(Trackball::MOVE_BACKWARD);
      break;
    }
    case 'a': {
      // move left //
      trackball->updateOffset(Trackball::MOVE_LEFT);
      break;
    }
    case 'd': {
      // move right //
      trackball->updateOffset(Trackball::MOVE_RIGHT);
      break;
    }
    case 'z': {
      fov += 0.1f;
      break;
    }
    case 'h': {
      fov -= 0.1f;
      if (fov < 1) fov = 1;
      break;
    }
    case 'r': {
      zNear += 0.1f;
      break;
    }
    case 'f': {
      zNear -= 0.1f;
      if (zNear < 0.1f) zNear = 0.1f;
      break;
    }
    case 't': {
      zFar += 0.1f;
      break;
    }
    case 'g': {
      zFar -= 0.1f;
      if (zFar < zNear) zFar = zNear + 0.01f;
      break;
    }
  }
  glutPostRedisplay();
}

void mouseEvent(int button, int state, int x, int y) {
  Trackball *trackball;
  // TODO: determine, which trackball should be updated //
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
  trackball->updateMouseBtn(mouseState, x, y);
}

void mouseMoveEvent(int x, int y) {
  Trackball *trackball;
  // TODO: determine, which trackball should be updated //
  trackball->updateMousePos(x, y);
}

/** inverts transformation matrices only consisting of rotations and translations **/
void invertRotTransMat(float m[16], float i[16]) {
  float t[3] = {m[12], m[13], m[14]};
  float o[3];
    
  for (int j = 0; j < 3; ++j) {
    o[j] = m[4 * j] * t[0] + m[4 * j + 1] * t[1] + m[4 * j + 2] * t[2];  
  }
  
  i[0] = m[0]; i[1] = m[4]; i[2] = m[8]; i[3] =0;
  i[4] = m[1]; i[5] = m[5]; i[6] = m[9]; i[7] =0;
  i[8] = m[2]; i[9] = m[6]; i[10]= m[10];i[11]=0;
  i[12]=-o[0]; i[13]=-o[1]; i[14]=-o[2]; i[15]=1;
}

/** inverts a GL_PROJECTION_MATRIX **/
void invertProjectionMat(float m[16], float i[16]) {
  for (int j = 0; j < 16; ++j) i[j] = 0;
  i[0] = 1 / m[0];
  i[5] = 1 / m[5];
  i[10]= m[15];
  i[11]= 1 / m[14];
  i[14]= m[11];
  i[15]= m[10] / m[14];
}
