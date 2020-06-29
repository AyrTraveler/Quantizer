/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "TimeContainerInfo.h"
#include "Thumbnail.h"
#include "CustomLookAndFeel.h"

#include <iostream>
#include <fstream>
using namespace std;

//------------------------------------------------------------------------------

class MainComponent : public AudioAppComponent,
    public ChangeListener, private Timer
{
public:
    MainComponent()
        : state(Stopped),
        thumbnailCache(5),
        dtc(formatManager, transportSource,s)
        , playImageButton("play",DrawableButton::ImageFitted)
        , openImageButton("open", DrawableButton::ImageFitted)
        , stopImageButton("stop", DrawableButton::ImageFitted)
        , pauseImageButton("pause", DrawableButton::ImageFitted)
    {


        startTimerHz(2);
        addAndMakeVisible(gainLabel);
        addAndMakeVisible(gainText);
        addAndMakeVisible(sGainLeft);
        addAndMakeVisible(resolution);
        addAndMakeVisible(rate);
        addAndMakeVisible(beats);
        addAndMakeVisible(triplets);
        addAndMakeVisible(bitdepth);
        addAndMakeVisible(outpath);
        addAndMakeVisible(samplerateTE);
        addAndMakeVisible(BPMTE);
        addAndMakeVisible(outputDirTE);
        addAndMakeVisible(resolutionCbox);
        addAndMakeVisible(bitsCbox);
        addAndMakeVisible(useTriplets);
        addAndMakeVisible(outputDirTE);
        addAndMakeVisible(s);
        addAndMakeVisible(&dtc);
        addAndMakeVisible(&quantizeButton);
        addAndMakeVisible(&detectButton);
        addAndMakeVisible(&playImageButton);
        addAndMakeVisible(&stopImageButton);
        addAndMakeVisible(&pauseImageButton);
        addAndMakeVisible(sGain);
        
        Value sharedValue;
        sharedValue = Random::getSystemRandom().nextDouble() * 100;
        sGain.getValueObject().referTo(sharedValue);
        sGainLeft.getValueObject().referTo(sharedValue);
        
        sGain.onValueChange = [this] {
            float q = remapValue(sGainLeft.getValue(), maxChannel);
            dtc.drawLevel(sGain.getValue(), maxChannel);
           if(!dtc.getLastDroppedFile().isEmpty()) gainText.setText(remap(sGain.getValue(), maxChannel),dontSendNotification); 
        };


        sGainLeft.onValueChange = [this] {
            float q = remapValue(sGainLeft.getValue(), maxChannel);
            dtc.drawLevel(sGain.getValue(), maxChannel);
            if (!dtc.getLastDroppedFile().isEmpty()) gainText.setText(remap(sGain.getValue(), maxChannel), dontSendNotification); };

        bitsCbox.addItem("16", 1);
        bitsCbox.addItem("24", 2);
        bitsCbox.setSelectedItemIndex(1);

        resolutionCbox.addItem("1/2", 1);
        resolutionCbox.addItem("1/4", 2);
        resolutionCbox.addItem("1/8", 3);
        resolutionCbox.addItem("1/16", 4);
        resolutionCbox.setSelectedItemIndex(2);

           s.setRange(0,1,0);
           s.onValueChange = [this] { dtc.setZoomFactor(s.getValue());  specificMarkers(); };
           s.setSkewFactor(2);
           s.setSliderStyle(Slider::LinearHorizontal);
           s.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
           s.setColour(Slider::thumbColourId,Colours::orange.withAlpha(0.5f));

             
              sGain.setRange(0, 1, 0);
              Colour barCol;
              sGain.setSliderStyle(Slider::LinearBarVertical);
              sGain.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
              sGain.setColour(Slider::textBoxOutlineColourId,barCol.fromRGB(38, 46, 57));
              sGain.setColour(Slider::trackColourId, barCol.fromRGB(38, 46, 57));

              dtc.addChangeListener(this);
              sGainLeft.setRange(0, 1, 0);
              sGainLeft.setSliderStyle(Slider::LinearBarVertical);
              sGainLeft.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
              sGainLeft.setColour(Slider::textBoxOutlineColourId, barCol.fromRGB(38, 46, 57));
              sGainLeft.setColour(Slider::trackColourId, barCol.fromRGB(38, 46, 57));

       
        quantizeButton.setButtonText("Quantize");
        quantizeButton.onClick = [this] { quantize((BPMTE.getText()).getIntValue(),4, (samplerateTE.getText()).getIntValue(), dtc.getLastDroppedFile().getLocalFile(), outputDirTE.getText()); };
        quantizeButton.setEnabled(false);

        
        detectButton.setButtonText("Peaks");
        detectButton.onClick = [this] {
            float q = remapValue(sGainLeft.getValue(), maxChannel);
            gainLabel.setText((String)q,dontSendNotification);
        isPeaking = true;
        detectPeaks((BPMTE.getText()).getIntValue(), (samplerateTE.getText()).getIntValue(), 4,q,dtc.getLastDroppedFile().getLocalFile());  
        
        loopMarkers();
        isPeaking = false;
        };

        
        playImageButton.onClick = [this] {playButtonClicked(); };
        stopImageButton.onClick = [this] {stopButtonClicked(); };
        pauseImageButton.onClick = [this] {pauseButtonClicked(); };
         
        playImageButton.setEnabled(false);
        stopImageButton.setEnabled(false);
        pauseImageButton.setEnabled(false);

        setSize(600, 660);

        formatManager.registerBasicFormats();
        transportSource.addChangeListener(this);

        setAudioChannels(2, 2);
        thread.startThread(3);

        auto* claf = new CustomLookAndFeel();
        addLookAndFeel(claf, "Custom Look And Feel");
        setupCustomLookAndFeelColours(*claf);
        setAllLookAndFeels(lookAndFeels.operator[](0));

    }

    ~MainComponent() override
    {
       
       
        shutdownAudio();
    }

   void paint(Graphics& g) override
    {
        Colour c;
        g.fillAll(c.fromRGB(15,18,22));
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
    {
        if (readerSource.get() == nullptr)
            bufferToFill.clearActiveBufferRegion();
        else
            transportSource.getNextAudioBlock(bufferToFill);
    }

    void releaseResources() override
    {
        transportSource.releaseResources();
    }

  
    
    String remap(float a, float b) {


        float c = juce::jmap(a, 0.0f, 1.0f, 0.0f, b);
        float d = Decibels::gainToDecibels(c);
       
        String e = String(d,2);
        
        return a==0.0f? "-inf" :(String)e + " dB";
    }

    float remapValue(float a, float b) {


        return juce::jmap(a, 0.0f, 1.0f, 0.0f, b);
        
    }

    void loopMarkers() {

        int s = tci.size();
        dtc.peakMarkers.clear();
        for (int k = 0; k < s;k++) {
            if(!tci.operator[](k)->bypassa)
            dtc.paintMarkers(tci.operator[](k), (samplerateTE.getText()).getIntValue());
        }

    }
    void specificMarkers() {

        int s = tci.size();
        int p = dtc.peakMarkers.size();

        for (int j = 0; j <p; j++) {
            dtc.peakMarkers.operator[](j)->setVisible(false);
        }

        dtc.peakMarkers.clear();
        for (int k = 0; k < s; k++) {

            float a = tci.operator[](k)->tpk;
            float b = (samplerateTE.getText()).getIntValue();
            float c = dtc.visibleRange.getStart();
            float d = dtc.visibleRange.getEnd();

            if(a/b >= c && a/b <d)
            dtc.paintMarkers(tci.operator[](k), (samplerateTE.getText()).getIntValue());
        }

    }

   

   
    void resized() override
    {
 
        auto myFont = Typeface::createSystemTypefaceFor(BinaryData::BebasNeueRegular_ttf, BinaryData::BebasNeueRegular_ttfSize);
        LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(myFont);
        Font newFont = Font(myFont);

        int offsetY = 100;
        resolution.setText("Resolution", dontSendNotification);
        rate.setText("Sample Rate", dontSendNotification);
        beats.setText("BPM", dontSendNotification);
        triplets.setText("Triplets", dontSendNotification);
        bitdepth.setText("Bit depth", dontSendNotification);
        gainLabel.setText("Threshold", dontSendNotification);
        outpath.setText("Output path", dontSendNotification);
    
        Font cfont = Font(myFont);
        cfont.setHeight(cfont.getHeight() * 0.8f);
    
        resolution.setFont(cfont.withExtraKerningFactor(0.15f));
        rate.setFont(cfont.withExtraKerningFactor(0.15f));
        beats.setFont(cfont.withExtraKerningFactor(0.15f));
        triplets.setFont(cfont.withExtraKerningFactor(0.095f));
        bitdepth.setFont(cfont.withExtraKerningFactor(0.095f));
        gainLabel.setFont(cfont.withExtraKerningFactor(0.095f));
        outpath.setFont(cfont.withExtraKerningFactor(0.095f));

        resolution.setFont(23);
        rate.setFont(23);
        beats.setFont(23);
        triplets.setFont(23);
        bitdepth.setFont(23);
        gainLabel.setFont(23);
        outpath.setFont(23);
        gainText.setFont(23);
        outputDirTE.setFont(20);
        BPMTE.setFont(18);
        samplerateTE.setFont(18);

        int leftLabelX = 45;
        int rightLabelX = 310;
        int labelY = 420;
        int y_off = 45;
        int leftContentX = 1;
        int rightContentX = 1;
        
        resolution.setBounds(leftLabelX, labelY,100,30);
        resolution.setJustificationType(Justification::right);
        resolutionCbox.setBounds(leftLabelX + 120, labelY, 80, 25);
        resolutionCbox.setJustificationType(Justification::centred);
        rate.setBounds(leftLabelX, labelY+ y_off, 100, 30);
        rate.setJustificationType(Justification::right);
        samplerateTE.setBounds(leftLabelX + 120, labelY + y_off, 80, 25);
        beats.setBounds(leftLabelX, labelY+ y_off*2, 100,30);
        beats.setJustificationType(Justification::right);
        BPMTE.setBounds(leftLabelX+120, labelY + y_off * 2, 80, 25);
        triplets.setBounds(rightLabelX, labelY, 100, 30);
        triplets.setJustificationType(Justification::right);
        useTriplets.setBounds(rightLabelX +110, labelY, 25, 25);
        bitdepth.setBounds(rightLabelX, labelY+ y_off, 100, 30);
        bitdepth.setJustificationType(Justification::right);
        gainLabel.setBounds(rightLabelX, labelY + y_off * 2, 100, 30);
        gainLabel.setJustificationType(Justification::right);
        gainText.setBounds(rightLabelX + 110, labelY + 2 * y_off, 60, 25);
        bitsCbox.setBounds(rightLabelX + 110, labelY + y_off, 60, 25);
        bitsCbox.setJustificationType(Justification::centred);
        outpath.setBounds(leftLabelX,570,100, 30);
        outpath.setJustificationType(Justification::right);
        outputDirTE.setBounds(70, 600, 290, 35);
        outputDirTE.setText(outPath + "\\",dontSendNotification);
        samplerateTE.setText("44100",dontSendNotification);
        samplerateTE.setJustification(Justification::centred);
        BPMTE.setText("118", dontSendNotification);
        BPMTE.setJustification(Justification::centred);
      
        Colour c;
        resolution.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        rate.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        beats.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        triplets.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        bitdepth.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        outpath.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        gainLabel.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        gainText.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.3f));

        Rectangle<int> sliderBounds(60, 410-offsetY, getWidth() - 120, 30);
        s.setBounds(sliderBounds);
        Rectangle<int> dtcBounds(60, 20, getWidth() - 120, 250+130 - offsetY);
        dtc.setBounds(dtcBounds);
        Rectangle<int> sGainBounds(getWidth() - 40, 20,20 , 250+130 - offsetY);
        sGain.setBounds(sGainBounds);
        Rectangle<int> sGainLeftBounds(20, 20, 20, 250 + 130 - offsetY);
        sGainLeft.setBounds(sGainLeftBounds);

        ScopedPointer<Drawable> play_image(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize));
        ScopedPointer<Drawable> play_over(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize));
        ScopedPointer<Drawable> play_dis(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize));
        play_image->replaceColour(Colours::black, c.fromRGB(38, 46, 57).withAlpha(0.5f));
        play_dis->replaceColour(Colours::black, c.fromRGB(60, 67, 77).withAlpha(0.5f));
        play_over->replaceColour(Colours::black, Colours::orange.withAlpha(0.5f));

        ScopedPointer<Drawable> stop_image(Drawable::createFromImageData(BinaryData::stopbuttonblackroundedsquare_svg, BinaryData::stopbuttonblackroundedsquare_svgSize));
        ScopedPointer<Drawable> stop_over(Drawable::createFromImageData(BinaryData::stopbuttonblackroundedsquare_svg, BinaryData::stopbuttonblackroundedsquare_svgSize));
        ScopedPointer<Drawable> stop_dis(Drawable::createFromImageData(BinaryData::stopbuttonblackroundedsquare_svg, BinaryData::stopbuttonblackroundedsquare_svgSize));

        stop_image->replaceColour(Colours::black, c.fromRGB(38, 46, 57).withAlpha(0.5f));
        stop_dis->replaceColour(Colours::black, c.fromRGB(60, 67, 77).withAlpha(0.5f));
        stop_over->replaceColour(Colours::black, Colours::orange.withAlpha(0.5f));
        
        ScopedPointer<Drawable> pause_image(Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize));
        ScopedPointer<Drawable> pause_over(Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize));
        ScopedPointer<Drawable> pause_dis(Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize));

        pause_image->replaceColour(Colours::black, c.fromRGB(38, 46, 57).withAlpha(0.5f));
        pause_dis->replaceColour(Colours::black, c.fromRGB(60, 67, 77).withAlpha(0.5f));
        pause_over->replaceColour(Colours::black, Colours::orange.withAlpha(0.5f));

        playImageButton.setImages(play_image, play_over, play_over, play_dis, play_over, play_over, play_over, play_dis);
        stopImageButton.setImages(stop_image, stop_over, stop_over, stop_dis, stop_over, stop_over, stop_over, stop_dis);
        pauseImageButton.setImages(pause_image, pause_over, pause_over, pause_dis, pause_over, pause_over, pause_over, pause_dis);
        playImageButton.setRadioGroupId(123);
        stopImageButton.setRadioGroupId(123);
        pauseImageButton.setRadioGroupId(123);
        playImageButton.setClickingTogglesState(true);
        stopImageButton.setClickingTogglesState(true);
        pauseImageButton.setClickingTogglesState(true);

        quantizeButton.setBounds(getWidth() - 120, 600, 80 , 34);
        detectButton.setBounds(getWidth() - 210, 600,80, 34);
        playImageButton.setBounds((getWidth()-140)/2+ 100, 445 - offsetY, 40, 40);
        pauseImageButton.setBounds((getWidth() - 140) / 2 + 50, 445 - offsetY, 40, 40);
        stopImageButton.setBounds((getWidth() - 140) / 2 , 445 - offsetY, 40, 40);

        if (!dtc.getLastDroppedFile().isEmpty()) { dtc.drawLevel(sGain.getValue(), maxChannel);}
       
    }


     void showAudioResource(URL resource)
    {
        if (loadURLIntoTransport(resource)) currentAudioFile =  std::move(resource);
        s.setValue(0, dontSendNotification);
        dtc.setURL(currentAudioFile);
    }



    bool loadURLIntoTransport(const URL& audioURL)
    {
        // unload the previous file source and delete it..
        transportSource.stop();
        transportSource.setSource(nullptr);
        currentAudioFileSource.reset();
        AudioFormatReader* reader = nullptr;
 
        if (audioURL.isLocalFile()) { reader = formatManager.createReaderFor(audioURL.getLocalFile());}
       
        if (reader != nullptr)
        {
            readerSource.reset(new AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(),
                                       32768,                   // tells it to buffer this many samples ahead
                                       &thread,                 // this is the background thread to use for reading-ahead
                                       reader->sampleRate);     // allows for sample rate correction
            return true;
        }

        return false;
    }


    void changeListenerCallback(ChangeBroadcaster* source) override
    {
         if (source == &dtc){
              
             String fname = dtc.getLastDroppedFile().getFileName();
             bool isWav = fname.contains(".wav")  || fname.contains(".Wav") || fname.contains(".WAV");
            
             if(isWav) showAudioResource(URL(dtc.getLastDroppedFile()));
          
             gainText.setText("", dontSendNotification);

            if (!dtc.getLastDroppedFile().isEmpty() && isWav) {
                
               
                outputDirTE.setText(File::getCurrentWorkingDirectory().getFullPathName() 
                    +"\\"+
                    dtc.getLastDroppedFile().getFileName().replace(".WAV", "_NEW.WAV",true).replace("%20", " ", true)
                    ,dontSendNotification);

                maxChannel = detectMax(dtc.getLastDroppedFile().getLocalFile());
                quantizeButton.setEnabled(false);
                              
            }
            
            if(isWav){
            dtc.drawLevel(sGain.getValue(), maxChannel);
            playImageButton.setEnabled(true);
            stopImageButton.setEnabled(true);
            pauseImageButton.setEnabled(true);
            }
           }

        
            
    }

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


    void detectPeaks(int BPM, int samplerate, float divider,float CustomThreshold, File file) {
        

        ofstream myfile;
        myfile.open("example2.txt");


        String fname = file.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");
        
        if (!isWav) return;
       
        bool checkBPM = BPM > 35 && BPM < 210;
        bool checkSR = samplerate >= 44100 && samplerate < 200000;
        bool checkDiv = ((int)divider % 2 == 0 && divider <= 4)||divider==0.5f;

        if (!checkBPM || !checkDiv || !checkSR || !(dtc.getLastDroppedFile().toString(false).length()>0)) return;
  
        tci.clear(true);
        
        /*TIME INFO*/
        float BPS = BPM / 60.0F;
        float timeQuantum = 1.0f / BPS / (float)divider;  //quanto dura un sedicesimo in secondi
        float sampleQuantum = (float)samplerate / BPS / (float)divider;  //quanti sample sono i 16esimi in base al samplerate
        int realSampleQuantum = sampleQuantum - floor(sampleQuantum) > 0.5 ? ceil(sampleQuantum) : floor(sampleQuantum);
        const float bufferLength = 512.0f;
        const float threshold = 0.04f;

        /*LEGGO FILE WAV*/
        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormat* audioFormat = formatManager.getDefaultFormat();
        AudioFormatReader* reader = formatManager.createReaderFor(file);
        int top = (int)(reader->lengthInSamples / bufferLength);

        float minR, maxR, minL, maxL;

        int initSample;
        int k = 0;

        /*TEST*/
        AudioSampleBuffer* bufferTemp = new AudioBuffer<float>(2, samplerate * 5);
        reader->readMaxLevels(0, reader->lengthInSamples,minR, maxR, minL, maxL);
        maxChannel = maxL;
        reader->read(bufferTemp, 0, bufferTemp->getNumSamples(), 0, true, true);
        float const* samples = bufferTemp->getReadPointer(0);
        while (fabs(samples[k]) < 3 * threshold) k += 1;
        initSample = k;
        tci.add(new TimeContainerInfo(initSample, initSample));
        myfile <<(String)initSample + " - "+ (String)initSample + "\n";
        int counter = initSample;
        float preMax = 100;
        float min = 10.0f;
        float prevRMS = 0.0f;
     
        while (counter < reader->lengthInSamples) {

            AudioSampleBuffer* bufferTemp2 = new AudioBuffer<float>(2, bufferLength);
            reader->read(bufferTemp2, 0, bufferTemp2->getNumSamples(), counter, true, true);
            maxL = bufferTemp2->getMagnitude(0, bufferTemp2->getNumSamples());
            min = maxL < min ? maxL : min;
            if ((preMax<threshold && maxL > threshold) || (maxL - preMax) > 0.1) { //INIZIO TRANSIENT

            AudioSampleBuffer* bufferTemp3 = new AudioBuffer<float>(2, bufferLength);
            reader->read(bufferTemp3, 0, bufferTemp3->getNumSamples(), counter - bufferLength, true, true);
            bufferTemp3->clear();

            reader->read(bufferTemp3, 0, bufferTemp3->getNumSamples(), counter, true, true);
            delete bufferTemp3;

            float const* samples = bufferTemp2->getReadPointer(0);

                for (int i = 0; i < bufferTemp2->getNumSamples(); ++i)
                {

                    if (fabs(samples[i]) == maxL) {

                        int running = 0;
                        while (running * realSampleQuantum < counter + i - initSample) {running += 1;}
                        float howManyQuantums = (float)(counter + i - initSample) / sampleQuantum;
                        float mantissa = howManyQuantums - floor(howManyQuantums);
                        int tbk = realSampleQuantum * (mantissa > 0.5f ? ceil(howManyQuantums) : floor(howManyQuantums));
                        tci.add(new TimeContainerInfo(counter + i, tbk + initSample));
                        myfile << (String)(counter + i) + " - " + (String)(tbk + initSample)+ "\n";
                        break;
                    }
              }
            }
            counter += bufferLength;
            preMax = maxL;
        }

        
        
        /*FILTRO FALSI POSITIVI*/
        int numPeaks = tci.size();

        for (int k = 0; k < numPeaks; k++) { tci.operator[](k)->bypassa = false; }


        /*FILTRO FALSI POSITIVI*/
        for (int k = 1; k < numPeaks - 1; k++) {   // MAX LEVEL < THRESHOLD

            int difference = tci.operator[](k + 1)->tpk - tci.operator[](k)->tpk;
            int difference_tb = tci.operator[](k + 1)->tbk - tci.operator[](k)->tbk;
            AudioSampleBuffer* bufferDel = new AudioBuffer<float>(2, difference);
            reader->read(bufferDel, 0, difference, tci.operator[](k)->tpk, true, true);
            float a = bufferDel->getMagnitude(0, difference);
            float b = bufferDel->getRMSLevel(0, 0, difference);

            if (difference_tb == 0) { tci.operator[](k)->bypassa = true; }

            else if (difference < realSampleQuantum) { tci.operator[](k)->bypassa = true; }

            else if (b < prevRMS) { tci.operator[](k)->bypassa = true; prevRMS = b; }

            else if (a < CustomThreshold) { tci.operator[](k)->bypassa = true; prevRMS = b;}

            else { prevRMS = b; }
            
        }
       
        bool finito = false;
         
        while (!finito) {
            numPeaks = tci.size();
            finito = true;
            for (int y = 0; y < tci.size() - 1; y++) {


                if (tci.operator[](y)->bypassa) {
                    finito = false;
                    tci.remove(y,true);
                    break;
                }
            }

        }
        
        quantizeButton.setEnabled(true);

    }


    /*   QUANTIZE
            
            1. LEGGO FILE
            2. APRO FILE IN SCRITTURA
            3. CREO 2 BUFFER IN CUI COPIO IL READER CREATO AL PUNTO 1

            4. FOREACH element in TCI (per ogni peak individuato al detectPeaks)
                4.1 se k==0  -->scrivo
                4.2 se k > 0
                    4.2.1 se ANTICIPO -->scrivo
                    4.2.2 se POSTICIPO-->scrivo
            
            5. CANCELLO I BUFFER creati al punto 3

    */

    void quantize(int BPM, int divider, int samplerate, File fileName, String newFileName)
    {
        String fname = fileName.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");
        if (!isWav) return;
       
        /*LEGGO FILE WAV*/
        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormat* audioFormat = formatManager.getDefaultFormat();
        AudioFormatReader* reader = formatManager.createReaderFor(fileName);
       
        /*CREO FILE WAV IN SCRITTURA*/
        WavAudioFormat format;
        std::unique_ptr<AudioFormatWriter> writer;
        writer.reset(format.createWriterFor(new FileOutputStream(File(newFileName), reader->lengthInSamples), samplerate, 2, 16, {}, 0));


        ofstream myfile;
        myfile.open("example.txt");
       
            AudioSampleBuffer* buffer = new AudioBuffer<float>(2, reader->lengthInSamples);
            AudioSampleBuffer* pivot = new AudioBuffer<float>(2, reader->lengthInSamples);
            buffer->clear();
            pivot->clear();
            reader->read(buffer, 0, buffer->getNumSamples(), 0, true, true);
            int current = 0, tot = reader->lengthInSamples;
        

            for (int i = 0; i < tci.size(); i++) {
           
            bool anticipo = tci.operator[](i)->anticipo;
            int tpk = tci.operator[](i)->tpk;
            int tbk = tci.operator[](i)->tbk;
            int fd = tci.operator[](i)->fade;
            float fd_square = fd * fd;
            int delta = tci.operator[](i)->delta_t;
            int starting = tpk - fd - delta;
         
            bool anticipo_1 = i==(tci.size()-1)? false : tci.operator[](i + 1)->anticipo;
            int tpk_1 = i == (tci.size() - 1) ? reader->lengthInSamples : tci.operator[](i + 1)->tpk;
            int tbk_1 = i == (tci.size() - 1) ? reader->lengthInSamples : tci.operator[](i + 1)->tbk;
            int fd_1 = i == (tci.size() - 1) ? tci.operator[](i)->fade:tci.operator[](i+1)->fade;
            int delta_1 = i == (tci.size() - 1) ? tci.operator[](i)->delta_t:tci.operator[](i+1)->delta_t;
            int middle = i == (tci.size() - 1) ? floor((float)(tbk + reader->lengthInSamples) / 2.0f) - tbk : floor((float)(tbk + tpk_1) / 2.0f) - tbk;

            if (anticipo && i< tci.size()-1) { //SE ANTICIPO
                
                //DA 0 a delta + fd
                pivot->copyFrom(0, 0, *buffer, 0, tpk-delta-fd, delta + fd);
                pivot->copyFrom(1, 0, *buffer, 1, tpk-delta-fd, delta + fd);
                
                current += delta + fd;

                for (int j = 0; j < fd; j++) { 
                    pivot->getWritePointer(0)[j] = pivot->getReadPointer(0)[j]* computeGain(fd, fd_square, j, true)
                                                  + buffer->getReadPointer(0)[tpk-fd-j]*computeGain(fd, fd_square, j, false);

                    pivot->getWritePointer(1)[j] = pivot->getReadPointer(0)[j];
                }
                Range<float> a = pivot->findMinMax(0, 0, delta + fd);
                myfile << "Anticipo - " + (String)a.getStart() + " - " + (String)i + " - first\n";
               
                if(delta + fd>0)
                writer->writeFromAudioSampleBuffer(*pivot, 0, delta + fd);
                pivot->clear();
                

                //DA delta + fd a middle+delta
                pivot->copyFrom(0, 0 ,*buffer,0,tpk,delta + middle);
                pivot->copyFrom(1, 0, *buffer, 1, tpk, delta + middle);
              
                current += delta + middle;
                
                for (int j = 0; j < delta; j++) { 
                                                    
                    pivot->getWritePointer(0)[middle + j] = pivot->getReadPointer(0)[middle + j]  * computeGain(delta, delta*delta, j, false)
                                                        + buffer->getReadPointer(0)[tbk+middle + j] * computeGain(delta, delta * delta, j, true);
                    pivot->getWritePointer(1)[middle + j] = pivot->getReadPointer(0)[middle + j];
                }

                a = pivot->findMinMax(0, 0, delta + middle);
                myfile << "Anticipo - " + (String)a.getStart() + " - " + (String)i + " - second\n";

                writer->writeFromAudioSampleBuffer(*pivot, 0, delta + middle);
                pivot->clear();

                //DA middle+delta + a tpk+1-fade
                int   temp = anticipo_1 ? tpk_1 - fd_1 - tbk - middle - delta : tbk_1 - delta_1 - tbk - middle - delta;
                pivot->copyFrom(0, 0, *buffer, 0, tbk + middle + delta, temp);
                pivot->copyFrom(1, 0, *buffer, 1, tbk + middle + delta, temp);

                current += temp;
                a = pivot->findMinMax(0, 0, temp);
                myfile << "Anticipo - " + (String)a.getStart() + " - " + (String)i + " - third\n";
               
                writer->writeFromAudioSampleBuffer(*pivot, 0, temp);
                pivot->clear();

           }
            else if (!tci.operator[](i)->anticipo && i < tci.size() - 1)//SE POSTICIPO MA PRIMA DELL'ULTIMO PICCO
            {
               
                //DA 0 a delta
                pivot->copyFrom(0, 0, *buffer, 0, tbk, delta);
                pivot->copyFrom(1, 0, *buffer, 1, tbk, delta);
                current += delta ;

                for (int j = 0; j < delta; j++) {
                    pivot->getWritePointer(0)[j] = pivot->getReadPointer(0)[j] * computeGain(delta, delta*delta, j, true)
                        + buffer->getReadPointer(0)[tbk - delta + j] * computeGain(delta, delta*delta, j, false);
                    pivot->getWritePointer(1)[j] = pivot->getReadPointer(0)[j];
                }
                Range<float> a = pivot->findMinMax(0, 0, delta + fd);
                myfile << "Delay - " + (String)a.getStart() + " - " + (String)i + " - first\n";

                writer->writeFromAudioSampleBuffer(*pivot, 0, delta);
                pivot->clear();

                //DA delta + fd a middle+delta
                pivot->copyFrom(0, 0, *buffer, 0, tpk, middle+delta);
                pivot->copyFrom(1, 0, *buffer, 1, tpk, middle+delta);
                current += middle + delta;

                for (int j = 0; j < delta; j++) {

                    pivot->getWritePointer(0)[middle + j] = pivot->getReadPointer(0)[middle + j] * computeGain(delta, delta * delta, j, false)
                                                        + buffer->getReadPointer(0)[tbk + middle + j] * computeGain(delta, delta * delta, j, true);
                    pivot->getWritePointer(1)[middle + j] = pivot->getReadPointer(0)[middle + j];
                }

                a = pivot->findMinMax(0, 0, delta + middle);
                myfile << "Delay - " + (String)a.getStart() + " - " + (String)i + " - second\n";

                writer->writeFromAudioSampleBuffer(*pivot, 0, middle + delta);
                pivot->clear();
               
                int temp = anticipo_1? tpk_1 - fd_1 - tbk - middle - delta : tbk_1 - delta_1 - tbk - middle - delta;
                pivot->copyFrom(0, 0, *buffer, 0, tbk + middle + delta, temp);
                pivot->copyFrom(1, 0, *buffer, 1, tbk + middle + delta, temp);
               
                current += temp;
                a = pivot->findMinMax(0, 0, temp);
                myfile << "Delay - " + (String)a.getStart() + " - " + (String)i + " - third\n";
                writer->writeFromAudioSampleBuffer(*pivot, 0, temp);
                pivot->clear();

            }
          
      
        }

            myfile.close();
            delete pivot;
            delete buffer;

    }

   
private:
    

    OwnedArray<LookAndFeel> lookAndFeels;


    void addLookAndFeel(LookAndFeel* laf, const String& name)
    {
        lookAndFeels.add(laf);

    }


    void setAllLookAndFeels(LookAndFeel* laf)
    {
        for (auto* child : getChildren())
            child->setLookAndFeel(laf);
    }

    void setupCustomLookAndFeelColours(LookAndFeel& laf)
    {
    }

     OwnedArray<TimeContainerInfo> tci;
  
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

   

    float computeGain(int fade, float square, int j, bool asc) {

            return asc ? 2 * (float)j / (float)fade - (float)(j * j) / square
                       : 1 - (float)(j * j) / square;
    }

  

    void playButtonClicked()
    {
        
        if (!transportSource.isPlaying())
        
        {
            transportSource.setPosition(position);
            transportSource.start();
        }
    }

    void stopButtonClicked()
    {
        if (transportSource.isPlaying())
        {
           
            transportSource.stop();
            
        }
        position = 0;
        dtc.position = position;
    }

    void pauseButtonClicked()
    {
        if (transportSource.isPlaying())
        {
            position = transportSource.getCurrentPosition();
            transportSource.stop();

        }
    }


    void timerCallback() override
    {
       if(!isPeaking) specificMarkers();
    }
    

    //==========================================================================
    
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton quantizeButton;
    TextButton detectButton;
    TooltipWindow tooltipWindow{ this };

    File opened;
    DrawableButton openImageButton;
    DrawableButton playImageButton;
    DrawableButton stopImageButton;
    DrawableButton pauseImageButton;

    Slider s,sGain,sGainLeft;
    ComboBox resolutionCbox {"combo"};
    ComboBox bitsCbox{ "bits" };
    ToggleButton sixteenBits,twentyfourBits;
    Label resolution, rate, beats, triplets, bitdepth, outpath, gainLabel, gainText;
    ToggleButton useTriplets;
    TextEditor samplerateTE, BPMTE, outputDirTE;
    bool isPeaking = false;
   
    TimeSliceThread thread{ "audio file preview" };
    URL currentAudioFile;
    std::unique_ptr<AudioFormatReaderSource> currentAudioFileSource;
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;
    AudioThumbnailCache thumbnailCache;

    DemoThumbnailComp dtc;
    String outPath = File::getCurrentWorkingDirectory().getFullPathName();
    float maxChannel=0;
    float position = 0.0f;
    bool firstDropped = true;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

