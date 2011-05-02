#ifndef __TRACKBALL__
#define __TRACKBALL__

#include <GL/glut.h>

#define STEP_DISTANCE 0.05f
#define PI 3.14159265f

class Trackball {
  public:
    Trackball();
    ~Trackball();
    
    enum MouseState {NO_BTN = 0, LEFT_BTN, RIGHT_BTN, MIDDLE_BTN};
    enum Motion {MOVE_FORWARD = 0, MOVE_BACKWARD, MOVE_LEFT, MOVE_RIGHT};
    
    void updateMousePos(int x, int y);
    void updateMouseBtn(MouseState state, int x, int y);
    void updateOffset(Motion motion);
    
    void reset(void);
    void rotateView(void);
    
  private:
    float mViewOffset[3];
    float mTheta, mPhi;
    float mLastTheta, mLastPhi;
    int mX, mY;
    MouseState mState;

    void calcTarget(float *target);
};

#endif
