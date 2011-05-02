#include "Trackball.h"
#include <cmath>
#include <iostream>

Trackball::Trackball() {
  reset();
}

Trackball::~Trackball() {}

void Trackball::reset(void) { 
  //mViewOffset = {0.0f, 0.0f, 30.0f};
  mViewOffset[0] = 0.0f;
  mViewOffset[1] = 0.0f;
  mViewOffset[2] = 30.0f;
  mPhi = 0.0f;
  mLastPhi = mPhi;
  mTheta = PI;
  mLastTheta = mTheta;
  mX = 0;
  mY = 0;
  mState = NO_BTN;
}
    
void Trackball::updateMousePos(int x, int y) {
  // TODO: implement the angular update given by the current mouse position //
  mTheta = mLastTheta + 0.001 * (x - mX);
  if(mTheta > 2 * PI) mTheta = 0;
  if(mTheta < 0) mTheta = 2 * PI;
  mPhi = mLastPhi + 0.001 * (y - mY);
  if(mPhi > 0.5 * PI) mPhi = 0.5 * PI;
  if(mPhi < -0.5 * PI) mPhi = -0.5 * PI;
}

void Trackball::updateMouseBtn(MouseState state, int x, int y) {
  // whenever this method is called, a mouse button has been pressed or released //
  switch (state) {
    case NO_BTN : {
      // TODO: when no button is pressed, i.e. a button has been released, store the current transformation, i.e. angles theta and phi //
      // huh?!?
      break;
    }
    case LEFT_BTN : {
      // TODO: when the left button is pressed, store the current position for further movement computations //
      mLastTheta = mTheta;
      mLastPhi = mPhi;
      mX = x;
      mY = y;
      break;
    }
    case RIGHT_BTN : {
      // not used yet //
      break;
    }
    default : break;
  }
  mState = state;
}

void Trackball::updateOffset(Motion motion) {
  // whenever this method is called, a motion is intended //
  float target[] = {0.0f, 0.0f, 0.0f};
  float direction[] = {0.0f, 0.0f, 0.0f};
  calcTarget(target);

  // calculate direction vector between t and m
  // needed for forward/backward motion
  direction[0] = target[0] - mViewOffset[0];
  direction[1] = target[1] - mViewOffset[1];
  direction[2] = target[2] - mViewOffset[2];
  // normalize vector
  float dir_length = sqrt(pow(direction[0],2) + pow(direction[1],2) + pow(direction[2],2));
  direction[0] /= dir_length;
  direction[1] /= dir_length;
  direction[2] /= dir_length;

  // calculate orthogonal angle to mTheta
  // needed for sideways motion
  float mAlpha;
  if(mTheta < 0.5 * PI) mAlpha = 1.5 * PI + mTheta;
  else mAlpha = mTheta - 0.5 * PI;

  
  switch (motion) {
    case MOVE_FORWARD : {
      // TODO: move STEP_DISTANCE along viewing direction //
      mViewOffset[0] += STEP_DISTANCE * direction[0];
      mViewOffset[1] += STEP_DISTANCE * direction[1];
      mViewOffset[2] += STEP_DISTANCE * direction[2];
      break;
    }
    case MOVE_BACKWARD : {
      // TODO: move STEP_DISTANCE in opposite of viewing direction //
      mViewOffset[0] -= STEP_DISTANCE * direction[0];
      mViewOffset[1] -= STEP_DISTANCE * direction[1];
      mViewOffset[2] -= STEP_DISTANCE * direction[2];
      break;
    }
    case MOVE_LEFT : {
      // TODO: move STEP_DISTANCE to the left on the x-z-plane //
      mViewOffset[0] -= sin(mAlpha) * STEP_DISTANCE;
      mViewOffset[2] -= cos(mAlpha) * STEP_DISTANCE;
      break;
    }
    case MOVE_RIGHT : {
      // TODO: move STEP_DISTANCE to the right on the x-z-plane //
      mViewOffset[0] += sin(mAlpha) * STEP_DISTANCE;
      mViewOffset[2] += cos(mAlpha) * STEP_DISTANCE;
      break;
    }
    default : break;
  }
}

void Trackball::rotateView(void) {
  // TODO: use gluLookAt(...) to set up the view for the current view offset and viewing angles //
  // 
  float target[] = {0.0f, 0.0f, 0.0f};
  calcTarget(target);
  
  //std::cout << "mViewOffset: " << mViewOffset[0] << " | " << mViewOffset[1] << " | " << mViewOffset[2] << std::endl;
  //std::cout << "target     : " << target[0] << " | " << target[1] << " | " << target [2] << std::endl;
  //std::cout << "mPhi: " << mPhi << "  mTheta: " << mTheta << std::endl;

  gluLookAt(mViewOffset[0],mViewOffset[1],mViewOffset[2], target[0],target[1],target[2], 0,1,0);
}

void Trackball::calcTarget(float *target) {
  float radius = 10.0f;
  target[0] = radius * cos(mPhi) * sin(mTheta) + mViewOffset[0];
  target[1] = radius * sin(mPhi) + mViewOffset[1];
  target[2] = radius * cos(mPhi) * cos(mTheta) + mViewOffset[2];
}
