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

#include <iostream>
#include <fstream>

using namespace std;
using namespace juce;
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
        , stopImageButton("stop", DrawableButton::ImageFitted)
        , pauseImageButton("pause", DrawableButton::ImageFitted)
        , forwardFFT(fftOrder)
        , aw("","",AlertWindow::WarningIcon)
    {

        setSize(600,600);

        

        tci.clear();
       

        startTimerHz(2);
        addAndMakeVisible(InputGroup);
        addAndMakeVisible(TransientGroup);
        addAndMakeVisible(OutputGroup);
        addAndMakeVisible(peakSensitivity);
       
        addAndMakeVisible(sGainLeft);
        addAndMakeVisible(resolution);
        addAndMakeVisible(rate);
        addAndMakeVisible(beats);
        addAndMakeVisible(sensitivity);
        addAndMakeVisible(value);
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
        addAndMakeVisible(showGridToggle);
        addAndMakeVisible(&quantizeButton);
        addAndMakeVisible(&deleteButton);
        addAndMakeVisible(&addButton);
        addAndMakeVisible(&playImageButton);
        addAndMakeVisible(&stopImageButton);
        addAndMakeVisible(&pauseImageButton);
        addAndMakeVisible(sGain);
        addAndMakeVisible(gainTextRight);
        addAndMakeVisible(gainTextLeft);
        addAndMakeVisible(rateSlider);
        addAndMakeVisible(BPMSlider);

        showGridToggle.setClickingTogglesState(true);
        showGridToggle.setToggleState(true, dontSendNotification);
        showGridToggle.setButtonText("Grid");
        deleteButton.onStateChange = [this] {dtc.deleteActive = deleteButton.getToggleState(); };
        showGridToggle.onStateChange = [this] {specificGrid(); };

        

        Value sharedValue;
        sharedValue = Random::getSystemRandom().nextDouble() * 100;
        sGain.getValueObject().referTo(sharedValue);
        sGainLeft.getValueObject().referTo(sharedValue);
        
        sGain.onValueChange = [this] {
            
            dtc.drawLevel(sGain.getValue(), maxChannel);
            if (!dtc.getLastDroppedFile().isEmpty()) gainTextRight.setText(remap(sGain.getValue(), maxChannel), dontSendNotification);
            gainTextLeft.setText(remap(sGain.getValue(), maxChannel), dontSendNotification);
        };


        sGainLeft.onValueChange = [this] {
            
            dtc.drawLevel(sGain.getValue(), maxChannel);
            if (!dtc.getLastDroppedFile().isEmpty()) gainTextRight.setText(remap(sGain.getValue(), maxChannel), dontSendNotification);
            gainTextLeft.setText(remap(sGain.getValue(), maxChannel), dontSendNotification); };

        bitsCbox.addItem("16", 1);
        bitsCbox.addItem("24", 2);
        bitsCbox.setSelectedItemIndex(1);

        resolutionCbox.addItem("1/2", 1);
        resolutionCbox.addItem("1/4", 2);
        resolutionCbox.addItem("1/8", 3);
        resolutionCbox.addItem("1/16", 4);
        resolutionCbox.setSelectedItemIndex(2);
           
           Colour barCol;
           resolutionCbox.onChange = [this] {specificGrid(); };
           BPMTE.onTextChange = [this] {specificGrid(); };

           rateSlider.setRange(44100,176100,44100);
          
           
           peakSensitivity.setRange(1,32,1);
           peakSensitivity.onDragEnd = [this] { detectOnsets();
           
           gainTextRight.setText((String)tci.size(),dontSendNotification);
           
           };

           peakSensitivity.onValueChange = [this] {
               
               value.setText((String)peakSensitivity.getValue(), dontSendNotification);

           };
          
           peakSensitivity.setSliderStyle(Slider::Rotary);
           peakSensitivity.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
           peakSensitivity.setColour(Slider::thumbColourId,Colours::orange.withAlpha(0.5f));

           s.setRange(0, 1, 0);
           s.onValueChange = [this] { dtc.setZoomFactor(s.getValue()); specificPeaks(); specificGrid(); };
           s.setSkewFactor(2);
           s.setSliderStyle(Slider::LinearHorizontal);
           s.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
           s.setColour(Slider::thumbColourId, Colours::orange.withAlpha(0.5f));
           

          
              sGain.setRange(0, 1, 0);
             
              sGain.setSliderStyle(Slider::LinearBarVertical);
              sGain.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
              sGain.setColour(Slider::textBoxOutlineColourId, barCol.fromRGB(38, 46, 57).darker(0.7f));
              sGain.setColour(Slider::trackColourId, barCol.fromRGB(38, 46, 57).darker(0.7f));

              dtc.addChangeListener(this);
              sGainLeft.setRange(0, 1, 0);
              sGainLeft.setSliderStyle(Slider::LinearBarVertical);
              sGainLeft.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
              sGainLeft.setColour(Slider::textBoxOutlineColourId, barCol.fromRGB(38, 46, 57).darker(0.7f));
              sGainLeft.setColour(Slider::trackColourId, barCol.fromRGB(38, 46, 57).darker(0.7f));

       
        quantizeButton.setButtonText("Quantize");
        quantizeButton.onClick = [this] { 
            
                sortList();
                quantize((BPMTE.getText()).getIntValue(), getResolutionIndex(), (samplerateTE.getText()).getIntValue(), dtc.getLastDroppedFile().getLocalFile(), outputDirTE.getText());
                aw.showMessageBox(AlertWindow::InfoIcon, "Success", "The file has been correctly exported at: " + outputDirTE.getText(), "Ok");
       
            
        };
        quantizeButton.setEnabled(false);

        
        deleteButton.setButtonText("Delete");
        deleteButton.setClickingTogglesState(true);
        deleteButton.setToggleState(false,dontSendNotification);

        addButton.setButtonText("Add");
        addButton.setClickingTogglesState(true);
        addButton.setToggleState(true, dontSendNotification);

        deleteButton.setRadioGroupId(345);
        addButton.setRadioGroupId(345);
        
        playImageButton.onClick = [this] {playButtonClicked(); };
        stopImageButton.onClick = [this] {stopButtonClicked(); };
        pauseImageButton.onClick = [this] {pauseButtonClicked(); };
         
        playImageButton.setEnabled(false);
        stopImageButton.setEnabled(false);
        pauseImageButton.setEnabled(false);

        peakSensitivity.setEnabled(false);
        deleteButton.setEnabled(false);
        addButton.setEnabled(false);

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
      
        g.fillAll(c.fromRGB(19,23,26));
       
        Path p;
        p.startNewSubPath(0,0);
        p.lineTo(getWidth(),0);
        p.lineTo(getWidth(), getHeight());
        p.lineTo(0, getHeight());
        p.closeSubPath();
        g.fillPath(p);

        
        auto y = getHeight() / 2 + 0.33 * InputGroup.getBounds().getHeight();
        auto x = 0.1* InputGroup.getBounds().getWidth() + InputGroup.getBounds().getX();
        auto width = 0.8 * InputGroup.getBounds().getWidth();
        auto height = 3;

        for(int i = 0; i<4; i ++){
            
            rect.add(new DrawableRectangle());
            rect.getLast()->setFill(c.fromRGB(38, 46, 57));
            addAndMakeVisible(*rect.getLast());

            auto offsetY = (i%2==0)? 0.33:0.66;
            auto posX =    (i<2)?    InputGroup.getBounds().getX(): OutputGroup.getBounds().getX();
            y = getHeight() / 2 + offsetY * InputGroup.getBounds().getHeight();
            x = 0.1 * InputGroup.getBounds().getWidth() + posX;
         

            rect.operator[](i)->setRectangle(Rectangle<float>(x,y,width,height));

        }

        

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
       
        String e = String(d,1);
        
        return a==0.0f? "-inf" :(String)e;
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
                dtc.smartPaint((float)counter/(float)samplerate, cc, false, false);
                cc += 1;
            }
            else if (counter > end) {

                break;
            }

            counter += realSampleQuantum;
            
            
        }

     

    }

    void specificPeaks() {
       
        dtc.smartClear();
        int size = peaks.size();
        int samplerate = (samplerateTE.getText()).getIntValue();

        for (int i = 0; i < size; i++) {
            
            

            if (peaks.operator[](i)->getValue()>0) {

                auto position = peaks.operator[](i)->getPosition();
                bool isUserTransient = peaks.operator[](i)->getUserTransient();
                auto offset = peaks.operator[](i)->getOffset();
                dtc.smartPaint((float)(position*fftSize + offset) / (float)samplerate, 1, true, isUserTransient);
            }

        }

    }

   
    void resized() override
    {
        //PARAMS
        auto offsetY = 100;
        auto leftLabelX = 45;
        auto rightLabelX = 310;
        auto labelY = 420;
        auto y_off = 45;
        auto leftContentX = 1;
        auto rightContentX = 1;
    
        Rectangle<int> dtcBounds(60, 20, getWidth() - 120, 0.35 * getHeight());
        Rectangle<int> sliderBounds(60, dtcBounds.getBottom() + 15, dtcBounds.getWidth()*0.95, 30);
        Rectangle<int> sGainRightBounds(getWidth() - 42, 20, 24, 0.35 * getHeight());
        Rectangle<int> sGainLeftBounds(18, 20, 24, 0.35 * getHeight());

        //BOUNDS
        playImageButton.setBounds((getWidth() - 140) / 2 + 100, sliderBounds.getBottom() + 15 , 40, 40);
        pauseImageButton.setBounds((getWidth() - 140) / 2 + 50, sliderBounds.getBottom() + 15, 40, 40);
        stopImageButton.setBounds((getWidth() - 140) / 2, sliderBounds.getBottom() + 15, 40, 40);
        
        Rectangle<int> r = playImageButton.getBounds();
        InputGroup.setBounds(10+dtcBounds.getX(), r.getBottom() + 15, 0.33 * dtcBounds.getWidth() - 20, getHeight() / 2 - 70);
        OutputGroup.setBounds(10 + dtcBounds.getX() + 0.66 * dtcBounds.getWidth(), r.getBottom() + 15, 0.33 * dtcBounds.getWidth() - 20, getHeight() / 2 - 70);
        TransientGroup.setBounds(10 + dtcBounds.getX() + 0.33 * dtcBounds.getWidth(), r.getBottom() + 15, 0.33 * dtcBounds.getWidth() - 20, getHeight() / 2 - 70);
        
         r = InputGroup.getBounds();
        rate.setBounds(r.getX() + 0.1 * r.getWidth(), r.getY() + 0.15*r.getHeight(), 100, 30);
        samplerateTE.setBounds(rate.getRight() + 10, r.getY() + 0.15 * r.getHeight(), 0.8 * r.getWidth() - rate.getWidth(), 25);
        beats.setBounds(r.getX() + 0.1 * r.getWidth(), r.getY() + 0.45* r.getHeight(), 100, 30);
        BPMTE.setBounds(beats.getRight() + 10, r.getY() + 0.45 * r.getHeight(), 0.8 * r.getWidth() - beats.getWidth(), 25);
        resolution.setBounds(r.getX() + 0.1 * r.getWidth(), r.getY() + 0.75 * r.getHeight(), 100, 30);
        resolutionCbox.setBounds(resolution.getRight() + 23, r.getY() + 0.74 * r.getHeight(), 0.8 * r.getWidth() - resolution.getWidth(), 38);
        
        r = OutputGroup.getBounds();
        quantizeButton.setBounds(r.getX() + 0.6 * r.getWidth(), r.getY() + 0.75 * r.getHeight(), 0.2 * r.getWidth(), 30);
        outpath.setBounds(r.getX() + 0.1 * r.getWidth(), r.getY() + 0.45 * r.getHeight(), 100, 30);
        outputDirTE.setBounds(outpath.getRight() + 10, r.getY() + 0.43 * r.getHeight(), 0.8 * r.getWidth() - outpath.getWidth(), 35);
        bitdepth.setBounds(r.getX() + 0.1 * r.getWidth(), r.getY() + 0.15 * r.getHeight(), 100, 30);
        bitsCbox.setBounds(bitdepth.getRight() + 10, r.getY() + 0.14 * r.getHeight(), 0.8 * r.getWidth() - bitdepth.getWidth(), 38);
        
        auto peakX = TransientGroup.getBounds().getX() + 0.3 * TransientGroup.getBounds().getWidth();
        auto peakY = TransientGroup.getBounds().getY() + 0.12 * TransientGroup.getBounds().getHeight();
        auto peakSize = 0.4 * TransientGroup.getBounds().getWidth();
        Rectangle<int> peakBounds(peakX, peakY, peakSize, peakSize);
        peakSensitivity.setBounds(peakBounds);

        r = TransientGroup.getBounds();
        sensitivity.setBounds(r.getX() + 0.35 * r.getWidth(), peakSensitivity.getBounds().getY() + peakSensitivity.getBounds().getHeight()*0.3, 0.3 * OutputGroup.getBounds().getWidth(), 30);
        value.setBounds(r.getX() + 0.35 * r.getWidth(), peakSensitivity.getBounds().getY() + peakSensitivity.getBounds().getHeight()*0.5, 0.3 * OutputGroup.getBounds().getWidth(), 30);
        deleteButton.setBounds(r.getX() + 0.6 * r.getWidth(), quantizeButton.getY(), 0.2 * r.getWidth(), 30);
        addButton.setBounds(r.getX() + 0.2 * r.getWidth(), quantizeButton.getY(), 0.2*r.getWidth(), 30);
       
        
        showGridToggle.setBounds(sliderBounds.getRight() + 10 , sliderBounds.getY() , 50, 25);
        
    
        s.setBounds(sliderBounds);
        dtc.setBounds(dtcBounds);
        sGain.setBounds(sGainRightBounds);
        sGainLeft.setBounds(sGainLeftBounds);


        r = sGain.getBounds();
        gainTextRight.setBounds(r.getX()-6, r.getBottomLeft().getY() + 6, 40, 24);
        r = sGainLeft.getBounds();
        gainTextLeft.setBounds(r.getX()-6, r.getBottomLeft().getY() + 6, 40, 24);

        //TEXT
        InputGroup.setText("Input Parameters");
        OutputGroup.setText("Rendering");
        TransientGroup.setText("Transients Detection");
        resolution.setText("Resolution", dontSendNotification);
        rate.setText("Sample Rate", dontSendNotification);
        beats.setText("BPM", dontSendNotification);
        sensitivity.setText("Sensitivity", dontSendNotification);
        bitdepth.setText("Bit depth", dontSendNotification);
        outpath.setText("Output path", dontSendNotification);

        //JUSTIFICATION
        InputGroup.setTextLabelPosition(Justification::left);
        OutputGroup.setTextLabelPosition(Justification::left);
        TransientGroup.setTextLabelPosition(Justification::left);
        resolution.setJustificationType(Justification::left);
        resolutionCbox.setJustificationType(Justification::centred);
        rate.setJustificationType(Justification::left);
        beats.setJustificationType(Justification::left);
        bitsCbox.setJustificationType(Justification::centred);
        outpath.setJustificationType(Justification::left);
        samplerateTE.setJustification(Justification::centred);
        BPMTE.setJustification(Justification::centred);
        bitdepth.setJustificationType(Justification::left);
        sensitivity.setJustificationType(Justification::centred);
        value.setJustificationType(Justification::centred);
        bitsCbox.setJustificationType(Justification::centred);

        //COLOURS
        Colour c;
        resolution.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.25f));
        rate.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.25f));
        beats.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.25f));
        sensitivity.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.25f));
        value.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.25f));
        bitdepth.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.25f));
        outpath.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.25f));
        gainTextRight.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.3f));
        gainTextLeft.setColour(Label::textColourId, c.fromRGB(38, 46, 57).brighter(0.3f));
        InputGroup.setColour(GroupComponent::textColourId, c.fromRGB(38, 46, 57).brighter(0.6f));
        InputGroup.setColour(GroupComponent::outlineColourId, c.fromRGB(38, 46, 57).brighter(0.6f));
        OutputGroup.setColour(GroupComponent::textColourId, c.fromRGB(38, 46, 57).brighter(0.6f));
        OutputGroup.setColour(GroupComponent::outlineColourId, c.fromRGB(38, 46, 57).brighter(0.6f));
        TransientGroup.setColour(GroupComponent::textColourId, c.fromRGB(38, 46, 57).brighter(0.6f));
        TransientGroup.setColour(GroupComponent::outlineColourId, c.fromRGB(38, 46, 57).brighter(0.6f));
        deleteButton.setColour(TextButton::buttonOnColourId, Colours::orange);
        addButton.setColour(TextButton::buttonOnColourId, Colours::orange);
        showGridToggle.setColour(TextButton::buttonOnColourId, Colours::orange);

        deleteButton.setColour(TextButton::textColourOffId, deleteButton.findColour(TextButton::textColourOnId).darker(0.3f));
        addButton.setColour(TextButton::textColourOffId, deleteButton.findColour(TextButton::textColourOnId).darker(0.3f));
        showGridToggle.setColour(TextButton::textColourOffId, deleteButton.findColour(TextButton::textColourOnId).darker(0.3f));

        samplerateTE.setColour(TextEditor::outlineColourId, Colours::transparentBlack);
        samplerateTE.setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
        samplerateTE.setColour(TextEditor::focusedOutlineColourId, Colours::transparentBlack);

        BPMTE.setColour(TextEditor::outlineColourId, Colours::transparentBlack);
        BPMTE.setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
        BPMTE.setColour(TextEditor::focusedOutlineColourId, Colours::transparentBlack);

        outputDirTE.setColour(TextEditor::outlineColourId, Colours::transparentBlack);
        outputDirTE.setColour(TextEditor::backgroundColourId, Colours::transparentBlack);
        outputDirTE.setColour(TextEditor::focusedOutlineColourId, Colours::transparentBlack);

        //FONTS
        auto myFont = Typeface::createSystemTypefaceFor(BinaryData::BebasNeueRegular_ttf, BinaryData::BebasNeueRegular_ttfSize);
        LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(myFont);
        Font cfont = Font(myFont);
        cfont.setHeight(cfont.getHeight() * 0.8f);
        resolution.setFont(cfont.withExtraKerningFactor(0.15f));
        rate.setFont(cfont.withExtraKerningFactor(0.15f));
        beats.setFont(cfont.withExtraKerningFactor(0.15f));
        sensitivity.setFont(cfont.withExtraKerningFactor(0.15f));
        value.setFont(cfont.withExtraKerningFactor(0.15f));
        bitdepth.setFont(cfont.withExtraKerningFactor(0.095f));
        outpath.setFont(cfont.withExtraKerningFactor(0.095f));
        resolution.setFont(25);
        rate.setFont(25);
        beats.setFont(25);
        sensitivity.setFont(25);
        value.setFont(25);
        bitdepth.setFont(25);
        outpath.setFont(25);
        gainTextRight.setFont(18);
        gainTextLeft.setFont(18);
        outputDirTE.setFont(23);
        BPMTE.setFont(25);
        samplerateTE.setFont(25);


        //PLAY PAUSE STOP BUTTONS
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

         
        //PREPARE
        if (!dtc.getLastDroppedFile().isEmpty()) {
            dtc.drawLevel(sGain.getValue(), maxChannel); specificGrid(); specificPeaks();}
       
        atStarting();

    }

    void changeListenerCallback(ChangeBroadcaster* source) override
    {


        if (source == &dtc) {

            String fname = dtc.getLastDroppedFile().getFileName();
            bool isWav = fname.contains(".wav") || fname.contains(".Wav") || fname.contains(".WAV");

            if (isWav) {

                if (dtc.isCreated) {
                    initSample = initPeak((BPMTE.getText()).getIntValue(), (samplerateTE.getText()).getIntValue(), getResolutionIndex(), dtc.getLastDroppedFile().getLocalFile());
                    dtc.isCreated = false;
                }

                dtc.drawLevel(sGain.getValue(), maxChannel);
                playImageButton.setEnabled(true);
                stopImageButton.setEnabled(true);
                pauseImageButton.setEnabled(true);
                quantizeButton.setEnabled(true);
                peakSensitivity.setEnabled(true);
                deleteButton.setEnabled(true);
                addButton.setEnabled(true);

                showAudioResource(URL(dtc.getLastDroppedFile()));
            }


            if (!dtc.getLastDroppedFile().isEmpty() && isWav) {


                outputDirTE.setText(File::getCurrentWorkingDirectory().getFullPathName()
                    + "\\" +
                    dtc.getLastDroppedFile().getFileName().replace(".WAV", "_NEW.WAV", true).replace("%20", " ", true)
                    , dontSendNotification);

                maxChannel = detectMax(dtc.getLastDroppedFile().getLocalFile());
                specificGrid();
                specificPeaks();
                quantizeButton.setEnabled(true);

            }

            sGain.setValue(0);
        }



    }

     void showAudioResource(URL resource)
    {
        if (loadURLIntoTransport(resource)) currentAudioFile =  std::move(resource);
        s.setValue(0, dontSendNotification);
        dtc.setURL(currentAudioFile);
    }


     bool checkInputParams() {

         auto r = samplerateTE.getText().getIntValue();
         auto b = BPMTE.getText().getIntValue();
         auto transientCheck = tci.size() > 0;

         if (1==1) {

           

             auto rateBounds = r < 100000 && r >= 44100;
             auto BPMBounds = b < 220 && r >= 20;

             return rateBounds && BPMBounds && transientCheck;
         }

         return false;
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


   

	 int getClosestBeat(int BPM, int samplerate, float divider,int i, bool isUser) {
       
     
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

                 if (!isUser && (init - time) > sampleQuantum && (-prev + time) > sampleQuantum) {
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

    void sortList() {


        auto N = tci.size();

        int TPK,TBK, delta,fade;
        bool anticipo;

        for (int j = 0; j < N - 1; j++)
            for (int i = 0; i < N - 1; i++)
                if (tci.operator[](i)->getTPK() > tci.operator[](i + 1)->getTPK())
                {
                    TPK = tci.operator[](i)->getTPK();
                    tci.operator[](i)->setTPK(tci.operator[](i+1)->getTPK());
                    tci.operator[](i+1)->setTPK(TPK);

                    TBK = tci.operator[](i)->getTBK();
                    tci.operator[](i)->setTBK(tci.operator[](i + 1)->getTBK());
                    tci.operator[](i + 1)->setTBK(TBK);

                    delta = tci.operator[](i)->getDelta();
                    tci.operator[](i)->setDelta(tci.operator[](i + 1)->getDelta());
                    tci.operator[](i + 1)->setDelta(delta);

                    fade = tci.operator[](i)->getFade();
                    tci.operator[](i)->setFade(tci.operator[](i + 1)->getFade());
                    tci.operator[](i + 1)->setFade(fade);

                    anticipo = tci.operator[](i)->getAnticipo();
                    tci.operator[](i)->setAnticipo(tci.operator[](i + 1)->getAnticipo());
                    tci.operator[](i + 1)->setAnticipo(anticipo);
                }

     
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
        
            /*    ofstream myfile;
    myfile.open("example9.txt");
    myfile << (String)tci.size()   + "\n";*/

            for (int i = 0; i < tci.size(); i++) {
           
            bool anticipo = tci.operator[](i)->getAnticipo();
            int tpk = tci.operator[](i)->getTPK();
            int tbk = tci.operator[](i)->getTBK();
            int fd = tci.operator[](i)->getFade();
            float fd_square = fd * fd;
            int delta = tci.operator[](i)->getDelta();
            int starting = tpk - fd - delta;
         
            bool anticipo_1 = i==(tci.size()-1)? false : tci.operator[](i + 1)->getAnticipo();
            int tpk_1 = i == (tci.size() - 1) ? reader->lengthInSamples : tci.operator[](i + 1)->getTPK();
            int tbk_1 = i == (tci.size() - 1) ? reader->lengthInSamples : tci.operator[](i + 1)->getTBK();
            int fd_1 = i == (tci.size() - 1) ? tci.operator[](i)->getFade() :tci.operator[](i+1)->getFade();
            int delta_1 = i == (tci.size() - 1) ? tci.operator[](i)->getDelta() :tci.operator[](i+1)->getDelta();
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



  /*-------------TRANSIENTS DETECTION-------------------*/

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
          float newFlux = compareSpectrums(spectrum, previousSpectrum, true);
          fluxes.add(new DummyFLoat(newFlux));
      
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
       /*ELIMINO I TRANSIENT SOTTO LA THRESHOLD SETTATA DA UTENTE*/
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

       //LIMATURA
       int offset = 128;
       float thr = 0.04f;

       for (int i = 2; i < size; i++) {

           buffer->clear();
           float value = peaks.operator[](i)->getValue();
           

           if (value > 0) {
               
               reader->read(buffer, 0, 2*offset, (i) * fftSize-offset, true, true);
               auto maxOffset = buffer->getMagnitude(0, 0, 2*offset);

               if (maxOffset < thr) {

                   buffer->clear();
                   int counter = 0;
                   reader->read(buffer, 0, buffer->getNumSamples(), (i)*fftSize, true, true);
                   
                   float const* samples = buffer->getReadPointer(0);
                   while (fabs(samples[counter]) < thr && counter < fftSize) counter += 1;

                   peaks.operator[](i)->setOffset(counter);
               }
               
           }

       }


       auto rate = (samplerateTE.getText()).getIntValue();
       auto BPM = (BPMTE.getText()).getIntValue();
       tci.clear();
       
       
     
       /*tolgo transient troppo vicini e aggiungo peaks all'array di pointer*/
       for (int i = 1; i < size; i++) {

           float value = peaks.operator[](i)->getValue();

           if (value > 0) {
               
               int closestBeat = getClosestBeat(BPM, rate, getResolutionIndex(), i * fftSize, false);
            
               if (closestBeat  < 0 ) peaks.operator[](i)->setValue(0.0f);
               else {

                   auto offset = peaks.operator[](i)->getOffset();
                   tci.add(new TimeContainerInfo(i*fftSize + offset,closestBeat));
               }
           }

       }

       bool cleanedList = false;

       while(!cleanedList){

           cleanedList = true;
       for (int i = 1; i < peaks.size(); i++) {
           
           float value = peaks.operator[](i)->getValue();

           if (value == 0.0f) { peaks.remove(i, true); cleanedList = false; break; }
       }
       }
       

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
        gainTextRight.setText("", dontSendNotification);
        gainTextLeft.setText("", dontSendNotification);
        firstTimeStarted = false;
        }
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

            if(!deleteButton.getToggleState()){
                
                auto start = dtc.peakMarkers.getLast()->getSample() * (samplerateTE.getText()).getIntValue();
                auto rate = (samplerateTE.getText()).getIntValue();
                auto BPM = (BPMTE.getText()).getIntValue();

                peaks.add(new DummyFLoat(1, (int)(start/(float)fftSize),true));
                tci.add(new TimeContainerInfo(start,getClosestBeat(BPM,rate, getResolutionIndex(), start,true)));
            }
            else {
                
                removeTransient(dtc.transientToDelete);
               
            }
           
           dtc.isDown = false;
                      
		   }

       if (dtc.scrollMoved) { dtc.scrollMoved = false;
                                specificGrid();
                                specificPeaks(); }
       
      
      
    }


    void removeTransient(float transientToDelete) {

        auto srate = (samplerateTE.getText()).getIntValue();
        int difference = 1000000;
        auto current = srate * transientToDelete;
        int found = -1;

        for (int i = 0; i < tci.size();i++) {

           auto tpk =  tci.operator[](i)->getTPK();
           
           if (fabs(tpk - current)<difference) {
               found = i;
               difference = fabs(tpk - current);
           }

        }

        tci.remove(found, true);

        difference = 1000000;
        found = -1;

        for (int i = 0; i < peaks.size(); i++) {

            auto position = peaks.operator[](i)->getPosition() * fftSize;

            if (fabs(position- current) < difference) {
                found = i;
                difference = fabs(position - current);
            }

        }

        peaks.remove(found,true);

    }
    

    //==========================================================================

    AlertWindow aw;

    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton quantizeButton;
    TextButton deleteButton;
    TextButton addButton;
    TextButton showGridToggle;

    DrawableButton playImageButton;
    DrawableButton stopImageButton;
    DrawableButton pauseImageButton;

    Slider s,sGain,sGainLeft, peakSensitivity, rateSlider, BPMSlider;
    ComboBox resolutionCbox {"combo"};
    ComboBox bitsCbox{ "bits" };
   
    Label resolution, rate, beats, bitdepth, outpath, gainTextRight, gainTextLeft, sensitivity, value;
    ToggleButton useTriplets;

    TextEditor samplerateTE, BPMTE, outputDirTE;
    GroupComponent InputGroup, TransientGroup, OutputGroup;

   
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
    OwnedArray<TimeContainerInfo> tci;
    OwnedArray<DrawableRectangle> rect;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

