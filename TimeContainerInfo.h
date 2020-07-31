/*
  ==============================================================================

    TimeContainerInfo.h
    Created: 24 Jun 2020 5:42:57pm
    Author:  alelo

  ==============================================================================
*/
 
#pragma once
#include <JuceHeader.h>
using namespace std;
using namespace juce;
class TimeContainerInfo {


public:

    TimeContainerInfo(int tpk, int tbk)
        : tbk(tbk), tpk(tpk)
    {

        anticipo = tbk > tpk ? true : false;
        delta_t = abs(tbk - tpk);
        fade = (int)(delta_t / 4.0);
    };

    bool anticipo, bypassa = false;
    int tpk, tbk, delta_t, fade;





};
