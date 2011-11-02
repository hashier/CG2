#include "Path.h"
#include <iostream>

Path::Path() {
  mControlPoints.resize(2);
  setFirstControlPoint(ControlPoint());
  setLastControlPoint(ControlPoint());
  mIsLooped = false;
}

Path::Path(ControlPoint start, ControlPoint end, bool looped) {
  mControlPoints.resize(2);
  setFirstControlPoint(start);
  setLastControlPoint(end);
  mIsLooped = looped;
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
  // check if t is within bounds //
  if (mControlPoints.size() < 4) {
    std::cerr << "(Path::getPositionForTime) - Not enough control points given!" << std::endl;
    return P;
  }
  
  if (t < mControlPoints.at(1).time) {
    if (isLooped()) {
      while (t < mControlPoints.at(1).time) t += mControlPoints.at(mControlPoints.size() - 2).time;
    } else {
      std::cerr << "(Path::getPositionForTime) - Warning: t = " << t << " exceeds path definition. Path begins at: t = " << mControlPoints.at(1).time << std::endl;
      t = mControlPoints.at(1).time;
    }
  }
  if (t > mControlPoints.at(mControlPoints.size() - 2).time) {
    if (isLooped()) {
      while (t > mControlPoints.at(mControlPoints.size() - 2).time) t -= mControlPoints.at(mControlPoints.size() - 2).time;
    } else {
      std::cerr << "(Path::getPositionForTime) - Warning: t = " << t << " exceeds path definition. Path ends at: t = " << mControlPoints.at(mControlPoints.size() - 2).time  << std::endl;
      t = mControlPoints.at(mControlPoints.size() - 2).time;
    }
  }
  
  // get correct set of control point segment for t //
  std::vector<ControlPoint>::iterator pathPointIter = mControlPoints.begin();
  ControlPoint *P0 = &*(pathPointIter++);
  ControlPoint *P1 = &*(pathPointIter++);
  ControlPoint *P2 = &*(pathPointIter++);
  ControlPoint *P3 = &*(pathPointIter++);
  while (P2->time < t && P3->time > 0) {
    P0 = P1;
    P1 = P2;
    P2 = P3;
    P3 = &*(pathPointIter++);
  }
  
  float t_1 = (t - P1->time) / (P2->time - P1->time);
  float t_2 = t_1 * t_1;
  float t_3 = t_2 * t_1;
  
  // compute per component //
  for (unsigned int i = 0; i < 3; ++i) {
    P.pos[i] = t_3 * (-1 * P0->pos[i] + 3 * P1->pos[i] - 3 * P2->pos[i] + P3->pos[i])
             + t_2 * ( 2 * P0->pos[i] - 5 * P1->pos[i] + 4 * P2->pos[i] - P3->pos[i])
             + t_1 * (-1 * P0->pos[i] + P2->pos[i])
             + 2 * P1->pos[i];
    P.pos[i] /= 2;
  }
  
  return P;
}