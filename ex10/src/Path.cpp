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

#define DEBUGBLA
ControlPoint Path::getPositionForTime(float t) {
#ifdef DEBUGBLA
  std::cout << "  Path::getPositionForTime(float)" << '\n';
  std::cout << "  Eingabezeit: " << t << '\n';
  std::cout << "  Anfangszeit: " << mMin_time << '\n';
  std::cout << "  Endzeit: " << mMax_time << '\n';
#endif
  
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
#ifdef DEBUGBLA
  std::cout << "  Beschnittene Zeit: " << t << '\n';
#endif
  
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
#ifdef DEBUGBLA
    std::cout << "    Gewählter Punkt " << i << ": " << points[i] << '\n';
#endif
  }  
  assert(points[0].time < t || points[0].time == -1);
  assert(points[1].time <= t || points[1].time == -1);
  assert(points[2].time >= t || points[2].time == -1);
  assert(points[3].time > t || points[3].time == -1);
 
  // DONE: compute the interpolated point //
  char M[4][4] = {{-1,  3, -3,  1},
                  { 2, -5,  4, -1},
                  {-1,  0,  1,  0},
                  { 0,  2,  0,  0}};
  ControlPoint tmp_points[4];
  for (unsigned int i = 0; i < 4; i++) {
    tmp_points[i] = points[0] * M[i][0];
    tmp_points[i] += points[1] * M[i][1];
    tmp_points[i] += points[2] * M[i][2];
    tmp_points[i] += points[3] * M[i][3];
//    for (unsigned int j = 1; j < 4; j++)
//      tmp_points[i] += points[j] * M[i][j];
  }
  
  // init return value //
  ControlPoint P(0,0,0,0);
  
  float _t = t - points[1].time;
  float the_power_of_t[4] = {_t*_t*_t, _t*_t, _t, 1.0f};
  for (unsigned int i = 0; i < 4; i++) {
    ControlPoint cp(tmp_points[i] * the_power_of_t[i]);
    P += cp;
#ifdef DEBUGBLA
    std::cout << "  Temporärer Punkt " << i << ": " << tmp_points[i] << '\n';
    std::cout << "  the power of t " << i << ": " << the_power_of_t[i] << '\n';
    std::cout << "  Änderung: " << cp << '\n';
    std::cout << "  Interpolierter Punkt(Zwischenstand): " << P << '\n';
#endif
  }

#if 0
  ControlPoint a(0,1,2,3);
  ControlPoint b(4,5,6,7);
  ControlPoint c = a*10 + b;
  std::cout << c << std::endl;
  std::cout << c/2 << std::endl;
#endif

  P = P/2;
  P.time = t;
#ifdef DEBUGBLA
  std::cout << "  Interpolierter Punkt: " << P << std::endl;
#endif
  return P;
}

std::ostream& operator<<(std::ostream& stream, const ControlPoint& cp) {
  stream << "X: " << cp.pos[0] << ", Y: " << cp.pos[1] << ", Z: " << cp.pos[2] << ", Time: " << cp.time;
  return stream;
}
