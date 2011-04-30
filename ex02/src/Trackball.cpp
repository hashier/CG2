#include "Trackball.h"
#include <cmath>
#include <iostream>

Trackball::Trackball() {
  reset();
}

Trackball::~Trackball() {}

void Trackball::reset(void) { 
  mViewOffset = {0.0f, 0.0f, 30.0f};
  mPhi = 0.0f;
  mLastPhi = mPhi;
  mTheta = 3.14f;
  mLastTheta = mTheta;
  mX = 0;
  mY = 0;
  mState = NO_BTN;
}
    
void Trackball::updateMousePos(int x, int y) {
  // TODO: implement the angular update given by the current mouse position //
  mTheta = mLastTheta + 0.001 * (x - mX);
  mPhi = mLastPhi + 0.001 * (y - mY);
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
  
  switch (motion) {
    case MOVE_FORWARD : {
      // TODO: move STEP_DISTANCE along viewing direction //
      mViewOffset[2]--;
      break;
    }
    case MOVE_BACKWARD : {
      // TODO: move STEP_DISTANCE in opposite of viewing direction //
      mViewOffset[2]++;
      break;
    }
    case MOVE_LEFT : {
      // TODO: move STEP_DISTANCE to the left on the x-z-plane //
      mViewOffset[0]--;
      break;
    }
    case MOVE_RIGHT : {
      // TODO: move STEP_DISTANCE to the right on the x-z-plane //
      mViewOffset[0]++;
      break;
    }
    default : break;
  }
}

void Trackball::rotateView(void) {
  // TODO: use gluLookAt(...) to set up the view for the current view offset and viewing angles //
  // 
  float target[] = {0.0f, 0.0f, 0.0f};
  float radius = 10.0f;
  /*
  target[1] = cos(mPhi) * target[1] - sin(mPhi) * target[2];
  target[2] = sin(mPhi) * target[1] + cos(mPhi) * target[2];

  target[0] = cos(mTheta) * target[0] + sin(mTheta) * target[2];
  target[2] = -sin(mTheta) * target[0] + cos(mTheta) * target[2];
  */
  target[0] = radius * cos(mPhi) * sin(mTheta) + mViewOffset[0];
  target[1] = radius * sin(mPhi) + mViewOffset[1];
  target[2] = radius * cos(mPhi) * cos(mTheta) + mViewOffset[2];
  
  //std::cout << "mViewOffset: " << mViewOffset[0] << " | " << mViewOffset[1] << " | " << mViewOffset[2] << std::endl;
  //std::cout << "target     : " << target[0] << " | " << target[1] << " | " << target [2] << std::endl;
  //std::cout << "mPhi: " << mPhi << "  mTheta: " << mTheta << std::endl;

  gluLookAt(mViewOffset[0],mViewOffset[1],mViewOffset[2], target[0],target[1],target[2], 0,1,0);
}
