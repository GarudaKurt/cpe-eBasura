#ifndef __WEBJSON__H
#define __WEBJSON__H

#include <Arduino.h>

struct BinData;

class WEBJSON {

  public:

    WEBJSON();

    String buildJSON(
        BinData bins[],
        int numBins,
        int threshold
    );

};

#endif