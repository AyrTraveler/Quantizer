/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TimeContainerInfo.h"
#include "Thumbnail.h"
#include "CustomLookAndFeel.h"
//#include<ctime> 
#include <iostream>
#include <fstream>
using namespace std;
using namespace juce;
//------------------------------------------------------------------------------

class DummyFLoat {

public:

    DummyFLoat(float val) {

        value = val;
    }

    float getValue() {

        return value;
    }

    void setValue(float val) {

        value = val;
    }

    DummyFLoat(float val, int pos) {

        value = val;
        position = pos;
    }

    float getPosition() {

        return position;
    }

    void setPosition(int pos) {

        position = pos;
    }

private:


    float value;
    int position = 0;

};


class MainComponent : public AudioAppComponent,
    public ChangeListener, private Timer
{
public:
    MainComponent()
        : state(Stopped),
        thumbnailCache(5),
        dtc(formatManager, transportSource,s)
        , playImageButton("play",DrawableButton::ImageFitted)
        , stopImageButton("stop", DrawableButton::ImageFitted)
        , pauseImageButton("pause", DrawableButton::ImageFitted)
        , forwardFFT(fftOrder)
    {

        
        startTimerHz(2);
        addAndMakeVisible(peakSensitivity);
        addAndMakeVisible(showGrid);
        addAndMakeVisible(showGridToggle);
        addAndMakeVisible(deleteTransient);
        addAndMakeVisible(deleteTransientLabel);
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
        
        showGridToggle.setToggleState(true, dontSendNotification);
        deleteTransient.onStateChange = [this] {dtc.deleteActive = deleteTransient.getToggleState(); };
        showGridToggle.onStateChange = [this] {specificGrid(); };

        

        Value sharedValue;
        sharedValue = Random::getSystemRandom().nextDouble() * 100;
        sGain.getValueObject().referTo(sharedValue);
        sGainLeft.getValueObject().referTo(sharedValue);
        
        sGain.onValueChange = [this] {
            
            dtc.drawLevel(sGain.getValue(), maxChannel);
           if(!dtc.getLastDroppedFile().isEmpty()) gainText.setText(remap(sGain.getValue(), maxChannel),dontSendNotification); 
        };


        sGainLeft.onValueChange = [this] {
            
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
            
           resolutionCbox.onChange = [this] {specificGrid(); };
           BPMTE.onTextChange = [this] {specificGrid(); };

           peakSensitivity.setRange(1,32,1);
           peakSensitivity.onDragEnd = [this] { detectOnsets(); 
           
             gainLabel.setText((String)tci.size(),dontSendNotification);
           };
          
           peakSensitivity.setSliderStyle(Slider::LinearHorizontal);
           peakSensitivity.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, true, 90, 25);
           peakSensitivity.setColour(Slider::thumbColourId,Colours::orange.withAlpha(0.5f));

           s.setRange(0, 1, 0);
           s.onValueChange = [this] { dtc.setZoomFactor(s.getValue()); specificPeaks(); specificGrid(); };
           s.setSkewFactor(2);
           s.setSliderStyle(Slider::LinearHorizontal);
           s.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
           s.setColour(Slider::thumbColourId, Colours::orange.withAlpha(0.5f));
             
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

        
        detectButton.setButtonText("Transients");
        detectButton.onClick = [this] {
           
          //  detectOnsets();
            gainLabel.setText((String)tci.size(), dontSendNotification);


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

    float getResolutionIndex() {

        int val = resolutionCbox.getSelectedId();

        if(useTriplets.getToggleState()) return 3.0f;

        switch (val) {

            case 1:
                return 0.5f;
                break;
            case 2:
                return 1.0f;
                break;
            case 3:
                return 2.0f;
                break;
            case 4:
                return 4.0f;
                break;

        }

        return 0.0f;
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

  
   
    void specificGrid() {

        if (!showGridToggle.getToggleState()) {

            for (int i = 0; i < dtc.gridMarkers.size();i++) {
                dtc.gridMarkers.operator[](i)->setVisible(false);
            }
            return;
        }

        int BPM = (BPMTE.getText()).getIntValue();
        int samplerate = (samplerateTE.getText()).getIntValue();
        float divider = getResolutionIndex();
        float BPS = BPM / 60.0F;
        float sampleQuantum = (float)samplerate / BPS / divider;  //quanti sample sono i 16esimi in base al samplerate
        int realSampleQuantum = sampleQuantum - floor(sampleQuantum) > 0.5 ? ceil(sampleQuantum) : floor(sampleQuantum);

       
        
        dtc.gridMarkers.clear();
        
        juce::int64 start = dtc.visibleRange.getStart()* samplerate;
        juce::int64 end = dtc.visibleRange.getEnd() * samplerate;

        juce::int64 counter = initSample;
        int cc = 0;

        while (counter<audioLen) {
            
            if (counter>=start && counter <=end) {
                dtc.smartPaint((float)counter/(float)samplerate, cc, false);
                cc += 1;
            }
            else if (counter > end) {

                break;
            }

            counter += realSampleQuantum;
            
            
        }

     

    }

    void specificPeaks() {
        gainLabel.setText((String)peaks.size(), dontSendNotification);
        dtc.smartClear();
        int size = peaks.size();
        int samplerate = (samplerateTE.getText()).getIntValue();

        for (int i = 0; i < size; i++) {
            
            

            if (peaks.operator[](i)->getValue()>0) {

                int position = peaks.operator[](i)->getPosition();
                dtc.smartPaint((float)(position*1024) / (float)samplerate, 1, true);
            }

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
        gainLabel.setText("Gain", dontSendNotification);
        outpath.setText("Output path", dontSendNotification);
        deleteTransientLabel.setText("Delete", dontSendNotification);
        showGrid.setText("Show Grid", dontSendNotification);

        Font cfont = Font(myFont);
        cfont.setHeight(cfont.getHeight() * 0.8f);
    
        resolution.setFont(cfont.withExtraKerningFactor(0.15f));
        rate.setFont(cfont.withExtraKerningFactor(0.15f));
        beats.setFont(cfont.withExtraKerningFactor(0.15f));
        triplets.setFont(cfont.withExtraKerningFactor(0.095f));
        bitdepth.setFont(cfont.withExtraKerningFactor(0.095f));
        gainLabel.setFont(cfont.withExtraKerningFactor(0.095f));
        outpath.setFont(cfont.withExtraKerningFactor(0.095f));
        deleteTransientLabel.setFont(cfont.withExtraKerningFactor(0.095f));
        showGrid.setFont(cfont.withExtraKerningFactor(0.095f));

        resolution.setFont(23);
        rate.setFont(23);
        beats.setFont(23);
        triplets.setFont(23);
        bitdepth.setFont(23);
        gainLabel.setFont(23);
        deleteTransientLabel.setFont(23);
        showGrid.setFont(23);
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
        resolutionCbox.setBounds(leftLabelX + 120, labelY, 80, 35);
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

        deleteTransientLabel.setBounds(rightLabelX + 140, labelY + y_off, 100, 30);
        deleteTransientLabel.setJustificationType(Justification::right);
        deleteTransient.setBounds(rightLabelX + 250, labelY + y_off, 25, 25);

        showGrid.setBounds(rightLabelX + 140, labelY, 100, 30);
        showGrid.setJustificationType(Justification::right);
        showGridToggle.setBounds(rightLabelX + 250, labelY, 25, 25);


        bitdepth.setBounds(rightLabelX, labelY+ y_off, 100, 30);
        bitdepth.setJustificationType(Justification::right);
        gainLabel.setBounds(rightLabelX, labelY + y_off * 2, 100, 30);
        gainLabel.setJustificationType(Justification::right);
        gainText.setBounds(rightLabelX + 110, labelY + 2 * y_off, 60, 25);
        bitsCbox.setBounds(rightLabelX + 110, labelY + y_off, 60, 35);
        bitsCbox.setJustificationType(Justification::centred);
        outpath.setBounds(leftLabelX,570,100, 30);
        outpath.setJustificationType(Justification::right);
        outputDirTE.setBounds(70, 600, getWidth()/2, 35);
        samplerateTE.setJustification(Justification::centred);
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
        deleteTransientLabel.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));
        showGrid.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.2f));

        Rectangle<int> peakBounds(60, 440 - offsetY, getWidth()/2 - 150, 30);
        peakSensitivity.setBounds(peakBounds);
        Rectangle<int> sliderBounds(60, 410-offsetY, getWidth() - 120, 30);
        s.setBounds(sliderBounds);
        Rectangle<int> dtcBounds(60, 20, getWidth() - 120, 250+130 - offsetY);
        dtc.setBounds(dtcBounds);
        Rectangle<int> sGainBounds(getWidth() - 40, 20,20 , 250+130 - offsetY);
        sGain.setBounds(sGainBounds);
        Rectangle<int> sGainLeftBounds(20, 20, 20, 250 + 130 - offsetY);
        sGainLeft.setBounds(sGainLeftBounds);

        std::unique_ptr<Drawable> play_image(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize));
        std::unique_ptr<Drawable> play_over(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize));
        std::unique_ptr<Drawable> play_dis(Drawable::createFromImageData(BinaryData::play_svg, BinaryData::play_svgSize));
        play_image->replaceColour(Colours::black, c.fromRGB(38, 46, 57).withAlpha(0.5f));
        play_dis->replaceColour(Colours::black, c.fromRGB(60, 67, 77).withAlpha(0.5f));
        play_over->replaceColour(Colours::black, Colours::orange.withAlpha(0.5f));

        std::unique_ptr<Drawable> stop_image(Drawable::createFromImageData(BinaryData::stop_svg, BinaryData::stop_svgSize));
        std::unique_ptr<Drawable> stop_over(Drawable::createFromImageData(BinaryData::stop_svg, BinaryData::stop_svgSize));
        std::unique_ptr<Drawable> stop_dis(Drawable::createFromImageData(BinaryData::stop_svg, BinaryData::stop_svgSize));

        stop_image->replaceColour(Colours::black, c.fromRGB(38, 46, 57).withAlpha(0.5f));
        stop_dis->replaceColour(Colours::black, c.fromRGB(60, 67, 77).withAlpha(0.5f));
        stop_over->replaceColour(Colours::black, Colours::orange.withAlpha(0.5f));
        
        std::unique_ptr<Drawable> pause_image(Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize));
        std::unique_ptr<Drawable> pause_over(Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize));
        std::unique_ptr<Drawable> pause_dis(Drawable::createFromImageData(BinaryData::pause_svg, BinaryData::pause_svgSize));

        pause_image->replaceColour(Colours::black, c.fromRGB(38, 46, 57).withAlpha(0.5f));
        pause_dis->replaceColour(Colours::black, c.fromRGB(60, 67, 77).withAlpha(0.5f));
        pause_over->replaceColour(Colours::black, Colours::orange.withAlpha(0.5f));

        playImageButton.setImages(play_image.get(), play_over.get(), play_over.get(), play_dis.get(), play_over.get(), play_over.get(), play_over.get(), play_dis.get());
        stopImageButton.setImages(stop_image.get(), stop_over.get(), stop_over.get(), stop_dis.get(), stop_over.get(), stop_over.get(), stop_over.get(), stop_dis.get());
        pauseImageButton.setImages(pause_image.get(), pause_over.get(), pause_over.get(), pause_dis.get(), pause_over.get(), pause_over.get(), pause_over.get(), pause_dis.get());
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

        if (!dtc.getLastDroppedFile().isEmpty()) {
            dtc.drawLevel(sGain.getValue(), maxChannel); specificGrid(); specificPeaks();}
       
        atStarting();

    }


     void showAudioResource(URL resource)
    {
        if (loadURLIntoTransport(resource)) currentAudioFile =  std::move(resource);
        s.setValue(0, dontSendNotification);
        dtc.setURL(currentAudioFile);
    }



    bool loadURLIntoTransport(const URL& audioURL)
    {
        
        transportSource.stop();
        transportSource.setSource(nullptr);
        currentAudioFileSource.reset();
        AudioFormatReader* reader = nullptr;
 
        if (audioURL.isLocalFile()) { reader = formatManager.createReaderFor(audioURL.getLocalFile());}
       
        if (reader != nullptr)
        {
            readerSource.reset(new AudioFormatReaderSource(reader, true));
            transportSource.setSource(readerSource.get(),
                                       32768,                   
                                       &thread,                 
                                       reader->sampleRate);     
            return true;
        }

        return false;
    }


    void changeListenerCallback(ChangeBroadcaster* source) override
    {


         if (source == &dtc){
           
             String fname = dtc.getLastDroppedFile().getFileName();
             bool isWav = fname.contains(".wav")  || fname.contains(".Wav") || fname.contains(".WAV");
            
             if (isWav) {

                 if(dtc.isCreated){
                 initSample = initPeak((BPMTE.getText()).getIntValue(), (samplerateTE.getText()).getIntValue(), getResolutionIndex(), dtc.getLastDroppedFile().getLocalFile());
                 dtc.isCreated = false;
                 }

                 dtc.drawLevel(sGain.getValue(), maxChannel);
                 playImageButton.setEnabled(true);
                 stopImageButton.setEnabled(true);
                 pauseImageButton.setEnabled(true);
                 quantizeButton.setEnabled(true);
                
                 showAudioResource(URL(dtc.getLastDroppedFile()));
             }
             
            
            if (!dtc.getLastDroppedFile().isEmpty() && isWav) {
                
                
                outputDirTE.setText(File::getCurrentWorkingDirectory().getFullPathName() 
                    +"\\"+
                    dtc.getLastDroppedFile().getFileName().replace(".WAV", "_NEW.WAV",true).replace("%20", " ", true)
                    ,dontSendNotification);

                maxChannel = detectMax(dtc.getLastDroppedFile().getLocalFile());
                specificGrid();
                specificPeaks();
                quantizeButton.setEnabled(true);
                              
            }
            
            sGain.setValue(0);
           }

       
            
    }

	 int getClosestBeat(int BPM, int samplerate, float divider,int i, bool isPeak) {
       
     
         float BPS = BPM / 60.0F;
         float sensitivity = peakSensitivity.getValue();
         float sampleQuantum = 1.0f / BPS / (float)divider/ sensitivity;
        
         float time = (float)i / (float)samplerate;
         float prev = 0.0f;
         int counter = 0;
         int arrayLen = dtc.gridMarkers.size();
         while (counter < arrayLen) {

             float init = dtc.gridMarkers.operator[](counter)->getSample();

             if (init<time) {

                 prev = init;

             }
             else {

                 if ((init - time) > sampleQuantum && (-prev + time) > sampleQuantum) {
                     return -1;
                 }

                 else if ((init - time) > (-prev + time)) {
                     return floor(prev*samplerate);
                 }
                 else {
                     return floor(init*samplerate);
                 }

             }

             counter += 1;
         }

   
      
       return 0.0f;

    }

    float detectMax(File file) {

        String fname = file.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");

        if (!isWav) return 0.0f;

        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormatReader* reader = formatManager.createReaderFor(file);
        float minR, maxR, minL, maxL;
        audioLen = reader->lengthInSamples;
        reader->readMaxLevels(0, reader->lengthInSamples, minR, maxR, minL, maxL);
        return maxL;
        
    }


    int initPeak(int BPM, int samplerate, float divider, File file) {
        

        String fname = file.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");
        
        if (!isWav) return 0;
       
        bool checkBPM = BPM > 35 && BPM < 210;
        bool checkSR = samplerate >= 44100 && samplerate < 200000;
        bool checkDiv = ((int)divider % 2 == 0 && divider <= 4)||divider==0.5f;

        if (!checkBPM || !checkDiv || !checkSR || !(dtc.getLastDroppedFile().toString(false).length()>0)) return 0;
  
        
        const float threshold = 0.04f;

        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormat* audioFormat = formatManager.getDefaultFormat();
        AudioFormatReader* reader = formatManager.createReaderFor(file);
       
       
        int k = 0;

        
        AudioSampleBuffer* bufferTemp = new AudioBuffer<float>(2, samplerate * 5);
       
        reader->read(bufferTemp, 0, bufferTemp->getNumSamples(), 0, true, true);
        float const* samples = bufferTemp->getReadPointer(0);
        while (fabs(samples[k]) <  threshold) k += 1;
       
        
        return k;
       

    }



 void quantize(int BPM, int divider, int samplerate, File fileName, String newFileName)
    {

        String fname = fileName.getFileName();
        bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");
        if (!isWav) return;
       
       
        AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        AudioFormat* audioFormat = formatManager.getDefaultFormat();
        AudioFormatReader* reader = formatManager.createReaderFor(fileName);
       
      
        WavAudioFormat format;
        std::unique_ptr<AudioFormatWriter> writer;
        writer.reset(format.createWriterFor(new FileOutputStream(File(newFileName), reader->lengthInSamples), samplerate, 2, 16, {}, 0));



       
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
            
            if(anticipo && anticipo_1){
                         
                         for(int u =0; u< pivot->getNumChannels(); u++)
                            pivot->copyFrom(u, 0, *buffer, u, tpk, tpk_1- delta - tpk);
                             
                         writer->writeFromAudioSampleBuffer(*pivot, 0, tpk_1-delta-tpk);
                         pivot->clear();
                          
                         for(int u =0; u< pivot->getNumChannels(); u++)
                            pivot->copyFrom(u, 0, *buffer, u, tpk_1 - delta - delta_1, delta_1);
                             
                         writer->writeFromAudioSampleBuffer(*pivot, 0,delta_1);
                         pivot->clear();
            
            }
            else if(anticipo && !anticipo_1){
                
                 for(int u =0; u< pivot->getNumChannels(); u++)
                            pivot->copyFrom(u, 0, *buffer, u, tpk, tbk_1- delta - tpk);
                             
                         writer->writeFromAudioSampleBuffer(*pivot, 0, tbk_1 - delta-tpk);
                         pivot->clear();
                         
            }
            else if(!anticipo && anticipo_1){
                
                    for(int u =0; u< pivot->getNumChannels(); u++)
                            pivot->copyFrom(u, 0, *buffer, u, tpk, tpk_1 - tpk);
                             
                         writer->writeFromAudioSampleBuffer(*pivot, 0, tpk_1 - tpk);
                         pivot->clear();
                         
                         for(int u =0; u< pivot->getNumChannels(); u++)
                            pivot->copyFrom(u, 0, *buffer, u, tpk_1 - delta - delta_1, delta_1 + delta);
                             
                         writer->writeFromAudioSampleBuffer(*pivot, 0,delta_1 + delta);
                         pivot->clear();
                
            
            }
            else if(!anticipo && !anticipo_1){
            
                for(int u =0; u< pivot->getNumChannels(); u++)
                            pivot->copyFrom(u, 0, *buffer, u, tpk, tbk_1 - tpk);
                             
                         writer->writeFromAudioSampleBuffer(*pivot, 0, tbk_1 - tpk);
                         pivot->clear();
                         
                         for(int u =0; u< pivot->getNumChannels(); u++)
                            pivot->copyFrom(u, 0, *buffer, u, tbk_1 - delta ,  delta);
                             
                         writer->writeFromAudioSampleBuffer(*pivot, 0, delta);
                         pivot->clear();
            
            }
                
            }

            
  }



  void detectOnsets(float sensitivity = 1.5f)
  {
     
      fluxes.clear();
      thresholdAverage.clear();
      peaks.clear();

      File newFileName = dtc.getLastDroppedFile().getLocalFile();
      AudioFormatManager formatManager;
      formatManager.registerBasicFormats();
      AudioFormat* audioFormat = formatManager.getDefaultFormat();
      AudioFormatReader* reader = formatManager.createReaderFor(newFileName);

      AudioSampleBuffer* buffer = new AudioBuffer<float>(2, fftSize);
      buffer->clear();
      
      zeromem(spectrum, 2048);
      zeromem(previousSpectrum, 2048);
      zeromem(pivotSpectrum, 2048);
      
      int counter = 0;
      while (counter < reader->lengthInSamples)
      {
          buffer->clear();
          reader->read(buffer, 0, buffer->getNumSamples(), counter, true, true);
          
          for (int k = 0; k < 1024; k++) {
              pivotSpectrum[k] = buffer->getReadPointer(0)[k];
          }
    
          
                 
          addFlux(pivotSpectrum);
          zeromem(pivotSpectrum, 2048);
          counter += fftSize;
      } 

      findTransients(sensitivity);

      specificPeaks();

      
  }

   void addFlux(float * samples)
  {
       
          forwardFFT.performFrequencyOnlyForwardTransform(samples);
          for (int k = 0; k < 1024; k++) {
              previousSpectrum[k] = spectrum[k];
          }

          for (int k = 0; k < 1024; k++) {
              spectrum[k] = samples[k];
          }
   
          zeromem(samples,sizeof(samples));
          float temp = compareSpectrums(spectrum, previousSpectrum, true);
          fluxes.add(new DummyFLoat(temp));
      
  }

   float compareSpectrums(float* spectrum, float* previousSpectrum, bool rectify)
   {
      
       float flux = 0;
       for (int i = 0; i < fftSize; i++)
       {
           float value = (spectrum[i] - previousSpectrum[i]);
           if (!rectify || value > 0)
           {
               flux += value;
           }
       }

       return flux;
   }


   void findTransients(float sensitivity = 1.5f, float thresholdTimeSpan = 0.5f)
   {
       setThresholdAverage(fftSize, thresholdTimeSpan, sensitivity);
       setPeaks(fftSize);
       cleanPeaks();
   }


   void setThresholdAverage( int sampleWindow,
       float thresholdTimeSpan, float thresholdMultiplier)
   {
      
       float sourceTimeSpan = (float)(sampleWindow) / (float)(samplerateTE.getText()).getIntValue();
       int windowSize = (int)(thresholdTimeSpan / sourceTimeSpan / 2);

       for (int i = 0; i < fluxes.size(); i++)
       {
          
           int start = std::max(i - windowSize, 0);
           int end = std::min(fluxes.size(), i + windowSize);
           
           float mean = 0;

           
           for (int j = start; j < end; j++)
           {
               mean += fluxes.operator[](j)->getValue();
           }

         
           mean /= (end - start);

           
           thresholdAverage.add(new DummyFLoat(mean * thresholdMultiplier));
       }

       
   }

  
   void setPeaks( int sampleCount)
   {
       
       const float indistinguishableRange = 0.01f; 
       
       int immunityPeriod = (int)((float)sampleCount
           / (float)(samplerateTE.getText()).getIntValue()
           / indistinguishableRange);

      
       
       for (int i = 0; i < fluxes.size(); i++)
       {
           float a = fluxes.operator[](i)->getValue();
           float b = thresholdAverage.operator[](i)->getValue();

           if (a >= b)
           {
               peaks.add(new DummyFLoat(a-b,i));
              
           }
           else
           {
               peaks.add(new DummyFLoat(0.0f));
           }
       }

     
       peaks.operator[](0)->setValue(0.0f);
       for (int i = 1; i < peaks.size() - 1; i++)
       {
           
           float a = peaks.operator[](i)->getValue();
           float b = peaks.operator[](i+1)->getValue();
           if (a < b)
           {
               peaks.operator[](i)->setValue(0.0f);
               continue;
           }

           
           if (peaks.operator[](i)->getValue() > 0.0f)
           {
               for (int j = i + 1; j < i + immunityPeriod; j++)
               {
                   if (peaks.operator[](j)->getValue() > 0)
                   {
                       peaks.operator[](j)->setValue(0.0f);
                   }
               }
           }
       }

      
   }


   void cleanPeaks() {

       File newFileName = dtc.getLastDroppedFile().getLocalFile();
       AudioFormatManager formatManager;
       formatManager.registerBasicFormats();
       AudioFormat* audioFormat = formatManager.getDefaultFormat();
       AudioFormatReader* reader = formatManager.createReaderFor(newFileName);

       AudioSampleBuffer* buffer = new AudioBuffer<float>(2, fftSize);
       buffer->clear();
      
       int size = peaks.size();

       float threshold = remapValue(sGainLeft.getValue(), maxChannel);

       for (int i = 2; i < size; i ++) {

           float value = peaks.operator[](i)->getValue();

           if (value > 0 ) {
               reader->read(buffer, 0, buffer->getNumSamples(), (i-1)*fftSize, true, true);
               float pre = buffer->getMagnitude(0, 0, buffer->getNumSamples());
               reader->read(buffer, 0, buffer->getNumSamples(), i * fftSize, true, true);
               float post = buffer->getMagnitude(0, 0, buffer->getNumSamples());

               if(pre < threshold && post < threshold) peaks.operator[](i)->setValue(0.0f);

           }

       }

       
       auto rate = (samplerateTE.getText()).getIntValue();
       auto BPM = (BPMTE.getText()).getIntValue();
       tci.clear();
       ofstream myfile;
       myfile.open("example8.txt");

       for (int i = 1; i < size; i++) {

           float value = peaks.operator[](i)->getValue();

           if (value > 0) {
               
               int closestBeat = getClosestBeat(BPM, rate, getResolutionIndex(), i * fftSize, true);
            
               if (closestBeat  < 0 ) peaks.operator[](i)->setValue(0.0f);
               else {

                   myfile << (String)(i * fftSize) +(String)closestBeat +  "\n";
                   tci.add(new TimeContainerInfo(i*fftSize,closestBeat));
               }
           }

       }

       bool finito = false;

       while(!finito){

           finito = true;
       for (int i = 1; i < peaks.size(); i++) {
           
           float value = peaks.operator[](i)->getValue();

           if (value == 0.0f) { peaks.remove(i, true); finito = false; break; }
       }
       }
       

   }
   
   void adjustPeaks() {
		
		
		
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


    void atStarting() {

        if(firstTimeStarted){
        BPMTE.setText("120", dontSendNotification);
        samplerateTE.setText("44100", dontSendNotification);
        gainText.setText("-", dontSendNotification);
        firstTimeStarted = false;
        }
    }
   

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

       
       
           
       if(dtc.isDown)
          {

            if(!deleteTransient.getToggleState()){
            auto start = dtc.peakMarkers.getLast()->getSample() * (samplerateTE.getText()).getIntValue();
            auto rate = (samplerateTE.getText()).getIntValue();
            auto BPM = (BPMTE.getText()).getIntValue();
            tci.add(new TimeContainerInfo(start,getClosestBeat(BPM,rate, getResolutionIndex(), start,false)));
            }
            else {
                
                peaks.remove(dtc.transientToDelete, true);
                tci.remove(dtc.transientToDelete, true);
                gainLabel.setText((String)tci.size(), dontSendNotification);

            }
           
           dtc.isDown = false;
                      
		   }

       if (dtc.scrollMoved) { dtc.scrollMoved = false; specificGrid(); specificPeaks(); }
       
      
      
    }
    

    //==========================================================================
    
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton quantizeButton;
    TextButton detectButton;
    TooltipWindow tooltipWindow{ this };

    DrawableButton playImageButton;
    DrawableButton stopImageButton;
    DrawableButton pauseImageButton;

    Slider s,sGain,sGainLeft, peakSensitivity;
    ComboBox resolutionCbox {"combo"};
    ComboBox bitsCbox{ "bits" };
    ToggleButton sixteenBits,twentyfourBits;
    Label showGrid,resolution, rate, beats, triplets, bitdepth, outpath, gainLabel, gainText, deleteTransientLabel;
    ToggleButton useTriplets, deleteTransient, showGridToggle;

    TextEditor samplerateTE, BPMTE, outputDirTE;
   
   
    TimeSliceThread thread{ "audio file preview" };
    URL currentAudioFile;
    std::unique_ptr<AudioFormatReaderSource> currentAudioFileSource;
    AudioFormatManager formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    TransportState state;
    AudioThumbnailCache thumbnailCache;
    int numTransient=0;
    int initSample=0;
    juce::int64 audioLen = 0;
    DemoThumbnailComp dtc;
    String outPath = File::getCurrentWorkingDirectory().getFullPathName();
    float maxChannel=0;
    float position = 0.0f;
    bool firstDropped = true;
    bool firstTimeStarted = true;
    enum
    {
        fftOrder = 10,
        fftSize = 1 << fftOrder
    };
    float spectrum[2*fftSize];
    float previousSpectrum[2*fftSize];
    float pivotSpectrum[2*fftSize];

    dsp::FFT forwardFFT;
    OwnedArray<DummyFLoat> fluxes;
    OwnedArray<DummyFLoat> thresholdAverage;
    OwnedArray<DummyFLoat> peaks;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

