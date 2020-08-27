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


    bool getAnticipo() { return anticipo; }
    int getTPK() { return tpk; }
    int getTBK() { return tbk; }
    int getDelta() { return delta_t; }
    int getFade() { return fade; }


    void setTPK(int value) { tpk = value; }
    void setTBK(int value) { tbk = value; }
    void setDelta(int value) { delta_t = value; }
    void setFade(int value) { fade = value; }
    void setAnticipo(bool value) { anticipo = value; }
   
private:

    bool anticipo;
    int tpk, tbk, delta_t, fade;




};

class DummyFLoat {

public:

    DummyFLoat(float val) { value = val;}



    DummyFLoat(float val, int pos) {

        value = val;
        position = pos;
    }

    DummyFLoat(float val, int pos, bool user) {

        value = val;
        position = pos;
        isUserTransient = user;
    }

    float getValue() { return value; }
    void setValue(float val) { value = val; }
    float getPosition() { return position;}
    void setPosition(int pos) {position = pos;}
    void setOffset(int o) {offset = o;}
    int getOffset() { return offset;}
    bool getUserTransient() { return isUserTransient;}

private:

    float value;
    int position = 0;
    int offset = 0;
    bool isUserTransient = false;
};


class AudioUtils {

public:

    AudioUtils() { ; }

private:

    float detectMax(File file) {

        String fname = file.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");
        if (!isWav) return 0.0f;

        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormatReader* reader = formatManager.createReaderFor(file);
        float minR, maxR, minL, maxL;
       
        reader->readMaxLevels(0, reader->lengthInSamples, minR, maxR, minL, maxL);
        return maxL;

    }

    int audioLen(File file) {

        String fname = file.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");

        if (!isWav) return 0.0f;

        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormatReader* reader = formatManager.createReaderFor(file);
        return reader->lengthInSamples;

    }

    int initPeak(int BPM, int samplerate, float divider, File file) {


        String fname = file.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");

        if (!isWav) return 0;

        bool checkBPM = BPM > 35 && BPM < 210;
        bool checkSR = samplerate >= 44100 && samplerate < 200000;
        bool checkDiv = ((int)divider % 2 == 0 && divider <= 4) || divider == 0.5f;

        if (!checkBPM || !checkDiv || !checkSR ) return 0;


        const float threshold = 0.04f;

        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormat* audioFormat = formatManager.getDefaultFormat();
        AudioFormatReader* reader = formatManager.createReaderFor(file);


        int k = 0;


        AudioSampleBuffer* bufferTemp = new AudioBuffer<float>(2, samplerate * 5);

        reader->read(bufferTemp, 0, bufferTemp->getNumSamples(), 0, true, true);
        float const* samples = bufferTemp->getReadPointer(0);
        while (fabs(samples[k]) < threshold) k += 1;


        return k;


    }

};

