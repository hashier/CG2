#include "Path.h"
#include <iostream>
#include <limits>
#include <cassert>
#include <cmath>

Path::Path(ControlPoint start, ControlPoint end, bool looped) {
  mControlPoints.resize(2);
  setFirstControlPoint(start);
  setLastControlPoint(end);
  setLooped(looped);
  mMin_time = std::numeric_limits<int>::max();
  mMax_time = 0;
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
  assert(point.time >= 0);
  if (point.time < mMin_time)
    mMin_time = point.time;
  if (point.time > mMax_time)
    mMax_time = point.time;
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
  
  // DONE: check sanity of t and adapt if neccessary (path might be looped) //
  if (!isLooped()) {
    if (t <= mMin_time)
      return mControlPoints[1];
    if (t >= mMax_time)
      return mControlPoints[mControlPoints.size() - 2];
  }
  float diff = mMax_time - mMin_time;
  if (t < mMin_time) {
    float count = -floor((t-mMin_time) / diff);
    t += count * diff;
  }
  if (t > mMax_time) {
    float count = floor((t-mMin_time) / diff);
    t -= count * diff;
  }
  assert(t >= mMin_time && t <= mMax_time);
  
  // DONE: select correct control point segment for t //
  std::vector<ControlPoint>::iterator start_point = mControlPoints.begin();
  while ((start_point+2)->time < t)
    start_point++;
    
  // DONE: since t is globally defined for the whole path, you need to compute your interpolation step for the current segment //
  ControlPoint points[4];
  for (unsigned int i = 0; i < 4; i++, start_point++) {
    if (start_point == mControlPoints.end())
      start_point = mControlPoints.begin()+2;
    points[i] = *start_point;
  }  
  
  // DONE: compute the interpolated point //
  char M[4][4] = {{-1,  3, -3,  1},
                  { 2, -5,  4, -1},
                  {-1,  0,  1,  0},
                  { 0,  2,  0,  0}};
  ControlPoint tmp_points[4];
  for (unsigned int i = 0; i < 4; i++) {
    tmp_points[i] = points[0] * M[i][0];
    for (unsigned int j = 1; j < 4; j++)
      tmp_points[i] += points[j] * M[i][j];
  }
  
  return (tmp_points[0]*t*t*t + tmp_points[1]*t*t + tmp_points[2]*t + tmp_points[3])/2;
}
