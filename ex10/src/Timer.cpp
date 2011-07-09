#include "Timer.h"

Timer::Timer() {
  mResetted = true;
  mRunning = false;
  mBegin = 0;
  mEnd = 0;
}

void Timer::start() {
  if(!isRunning()) {
    if (mResetted) {
      mBegin = clock();
    } else {
      mBegin -= mEnd - clock();
    }
    mRunning = true;
    mResetted = false;
  }
}

void Timer::stop() {
  if(isRunning()) {
    mEnd = clock();
    mRunning = false;
  }
}

void Timer::reset() {
  bool wasRunning = isRunning();
  if (wasRunning) {
    stop();
  }
  mResetted = true;
  mBegin = 0;
  mEnd = clock();
  if (wasRunning) {
    start();
  }
}

bool Timer::isRunning() {
  return mRunning;
}

double Timer::getTime() {
  if (isRunning()) {
    return (double)(clock() - mBegin) / (double)CLOCKS_PER_SEC;
  } else {
    return (double)(mEnd - mBegin);
  }
}


bool Timer::isOver(double seconds) {
  return seconds >= getTime();
}