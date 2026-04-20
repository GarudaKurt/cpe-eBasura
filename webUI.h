#ifndef __WEBUI__H
#define __WEBUI__H

#include <Arduino.h>

class WEBUI {
  public:
    WEBUI();
    String buildHTML();
};

#endif