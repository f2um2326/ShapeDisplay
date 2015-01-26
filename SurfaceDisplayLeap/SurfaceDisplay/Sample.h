#ifndef _SAMPLE_LOADER_H_INCLUDED_
#define _SAMPLE_LOADER_H_INCLUDED_


#include <iostream>
#include "Leap.h"
using namespace Leap;

#define LEFT_SWIPE 0
#define RIGHT_SWIPE 1

extern int gesture_flag;
extern int current_height;
extern int bar_heights[4][4];

class SampleListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

  private:
};

#endif // _SAMPLE_LOADER_H_INCLUDED_