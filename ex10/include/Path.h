#ifndef __PATH__
#define __PATH__

#include <vector>
#include <ostream>

struct ControlPoint {
  ControlPoint(float x = 0, float y = 0, float z = 0, float t = -1.0) {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    time = t;
  }
  ControlPoint(const ControlPoint& cp) {
    pos[0] = cp.pos[0];
    pos[1] = cp.pos[1];
    pos[2] = cp.pos[2];
    time = cp.time;
  }
  ControlPoint& operator+=(const ControlPoint& cp) {
    pos[0] += cp.pos[0];
    pos[1] += cp.pos[1];
    pos[2] += cp.pos[2];
    time += cp.time;
    return *this;
  }
  ControlPoint operator+(const ControlPoint& cp) const {
    ControlPoint tmp(*this);
    tmp += cp;
    return tmp;
  }
  ControlPoint operator*(const int factor) {
    return ControlPoint(factor * pos[0], factor * pos[1], factor * pos[2], factor * time);
  }
  ControlPoint operator*(const float factor) {
    return ControlPoint(factor * pos[0], factor * pos[1], factor * pos[2], factor * time);
  }
  ControlPoint operator/(const int factor) {
    return ControlPoint(pos[0] / factor, pos[1] / factor, pos[2] / factor, time / factor);
  }
  float pos[3];
  float time;
};
std::ostream& operator<<(std::ostream& stream, const ControlPoint& cp);

class Path {
  public:
    Path(ControlPoint start = ControlPoint(), ControlPoint end = ControlPoint(), bool looped = false);
    
    void setFirstControlPoint(ControlPoint point);
    void setLastControlPoint(ControlPoint point);
    void addIntermediateControlPoint(ControlPoint point);
    
    void setLooped(bool looped);
    bool isLooped();
    
    ControlPoint getPositionForTime(float t);
  private:
    bool mIsLooped;
    std::vector<ControlPoint> mControlPoints;
    float mMin_time;
    float mMax_time;
};

#endif
