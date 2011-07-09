#include "Path.h"
#include <iostream>

Path::Path() {
  mControlPoints.resize(2);
  setFirstControlPoint(ControlPoint());
  setLastControlPoint(ControlPoint());
  setLooped(false);
}

Path::Path(ControlPoint start, ControlPoint end, bool looped) {
  mControlPoints.resize(2);
  setFirstControlPoint(start);
  setLastControlPoint(end);
  setLooped(looped);
}
    
void Path::setFirstControlPoint(ControlPoint point) {
  point.time = -1;
  mControlPoints.front() = point;
}

void Path::setLastControlPoint(ControlPoint point) {
  point.time = -1;
  mControlPoints.back() = point;
}

void Path::addIntermediateControlPoint(ControlPoint point) {
  for (std::vector<ControlPoint>::iterator pathPointIter = mControlPoints.begin() + 1; pathPointIter != mControlPoints.end(); ++pathPointIter) {
    if (pathPointIter->time > point.time || pathPointIter->time < 0) {
      mControlPoints.insert(pathPointIter, point);
      break;
    }
  }
}

void Path::setLooped(bool looped) {
  mIsLooped = looped;
}

bool Path::isLooped() {
  return mIsLooped;
}

ControlPoint Path::getPositionForTime(float t) {
  // init return value //
  ControlPoint P(0,0,0,t);
  
  // TODO: check sanity of t and adapt if neccessary (path might be looped) //
  
  // TODO: select correct control point segment for t //
  
  // TODO: since t is globally defined for the whole path, you need to compute your interpolation step for the current segment //
  
  // TODO: compute the interpolated point //
  
  return P;
}