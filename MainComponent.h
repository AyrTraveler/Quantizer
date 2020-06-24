/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"



#include <iostream>
#include <fstream>
using namespace std;


struct CustomLookAndFeel : public LookAndFeel_V4
{

    void drawTickBox(Graphics& g, Component& component,
        float x, float y, float w, float h,
        const bool ticked,
        const bool isEnabled,
        const bool shouldDrawButtonAsHighlighted,
        const bool shouldDrawButtonAsDown)
    {
        ignoreUnused(isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);

        Rectangle<float> tickBounds(x, y, w, h);
        Colour c;
        g.setColour(c.fromRGB(38, 46, 57));
        g.drawRect(tickBounds,1.5f);

        if (ticked)
        {
            g.setColour(component.findColour(ToggleButton::tickColourId));
            auto tick = getTickShape(0.75f);
            g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(4, 5).toFloat(), false));
        }
    }

    void drawButtonText(Graphics& g, TextButton& button,
        bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        Font font(getTextButtonFont(button, button.getHeight()));
        g.setFont(font);
        g.setFont(font.getHeight()+4);
        g.setColour(button.findColour(button.getToggleState() ? TextButton::textColourOnId
            : TextButton::textColourOffId)
            .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));

        const int yIndent = jmin(4, button.proportionOfHeight(0.3f));
        const int cornerSize = jmin(button.getHeight(), button.getWidth()) / 2;

        const int fontHeight = roundToInt(font.getHeight() * 0.6f);
        const int leftIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
        const int rightIndent = jmin(fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        const int textWidth = button.getWidth() - leftIndent - rightIndent;

        if (textWidth > 0)
            g.drawFittedText(button.getButtonText(),
                leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                Justification::centred, 2);
    }


    void fillTextEditorBackground(Graphics& g, int width, int height, TextEditor& textEditor)
    {
        if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) != nullptr)
        {
            g.setColour(textEditor.findColour(TextEditor::backgroundColourId));
            g.fillRect(0, 0, width, height);

            g.setColour(textEditor.findColour(TextEditor::outlineColourId));
            g.drawHorizontalLine(height - 1, 0.0f, static_cast<float> (width));
        }
        else
        {
            LookAndFeel_V2::fillTextEditorBackground(g, width, height, textEditor);
        }
    }

    void drawComboBox(Graphics& g, int width, int height, bool,
        int, int, int, int, ComboBox& box) override
    {
        auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
        Rectangle<int> boxBounds(0, 0, width, height);

       
    }

    void drawPopupMenuBackground(Graphics& g, int width, int height) override
    {
        //g.fillAll(findColour(PopupMenu::backgroundColourId));
        Colour c;
        g.fillAll(c.fromRGB(38, 46, 57));
        ignoreUnused(width, height);


    }

    void drawTextEditorOutline(Graphics& g, int width, int height, TextEditor& textEditor)
    {
        if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) == nullptr)
        {
            if (textEditor.isEnabled())
            {
                if (textEditor.hasKeyboardFocus(true) && !textEditor.isReadOnly())
                {
                    g.setColour(textEditor.findColour(TextEditor::backgroundColourId).brighter(0.1f));
                    g.drawRect(0, 0, width, height, 2);
                }
                else
                {
                    g.setColour(textEditor.findColour(TextEditor::backgroundColourId).brighter(0.1f));
                    g.drawRect(0, 0, width, height);
                }
            }
        }
    }

};

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


class DemoThumbnailComp : public Component,
    public ChangeListener,
    public FileDragAndDropTarget,
    public ChangeBroadcaster,
    private ScrollBar::Listener,
    private Timer
{
public:
    DemoThumbnailComp(AudioFormatManager& formatManager,
        AudioTransportSource& source,
        Slider& slider)
        : transportSource(source),
        zoomSlider(slider),
        thumbnail(512, formatManager, thumbnailCache)
        
    {
        thumbnail.addChangeListener(this);

        addAndMakeVisible(scrollbar);
        scrollbar.setRangeLimits(visibleRange);
        scrollbar.setAutoHide(false);
        scrollbar.addListener(this);

        currentPositionMarker.setFill(Colours::red.withAlpha(0.45f));
        addAndMakeVisible(currentPositionMarker);

        currentLevel.setFill(Colours::lightblue);
        addAndMakeVisible(currentLevel);
    }

    ~DemoThumbnailComp()
    {
        scrollbar.removeListener(this);
        thumbnail.removeChangeListener(this);
    }

    void setURL(const URL& url)
    {
        InputSource* inputSource = nullptr;

#if ! JUCE_IOS
        if (url.isLocalFile())
        {
            inputSource = new FileInputSource(url.getLocalFile());
        }
        else
#endif
        {
            if (inputSource == nullptr)
                inputSource = new URLInputSource(url);
        }

        if (inputSource != nullptr)
        {
            thumbnail.setSource(inputSource);

            Range<double> newRange(0.0, thumbnail.getTotalLength());
            scrollbar.setRangeLimits(newRange);
            setRange(newRange);

            startTimerHz(40);
        }
    }

    URL getLastDroppedFile() const noexcept { return lastFileDropped; }

    void setZoomFactor(double amount)
    {
        if (thumbnail.getTotalLength() > 0)
        {
            auto newScale = jmax(0.001, thumbnail.getTotalLength() * (1.0 - jlimit(0.0, 0.99, amount)));
            auto timeAtCentre = xToTime(getWidth() / 2.0f);

            setRange({ timeAtCentre - newScale * 0.5, timeAtCentre + newScale * 0.5 });
        }
    }

    void setRange(Range<double> newRange)
    {
        visibleRange = newRange;
        scrollbar.setCurrentRange(visibleRange);
        updateCursorPosition();
        repaint();
    }

    void setFollowsTransport(bool shouldFollow)
    {
        isFollowingTransport = shouldFollow;
    }

    void paint(Graphics& g) override
    {
       

        Colour c;

        g.fillAll(c.fromRGB(38, 46, 57));

        g.setColour((Colours::orange).withAlpha(0.5f));
       
        

        if (thumbnail.getTotalLength() > 0.0)
        {
            auto thumbArea = getLocalBounds();

            thumbArea.removeFromBottom(scrollbar.getHeight() + 4);
            thumbnail.drawChannel(g, thumbArea.reduced(2),visibleRange.getStart(), visibleRange.getEnd(),0, 1.0f);
        }
        else
        {
            auto myFont = Typeface::createSystemTypefaceFor(BinaryData::BebasNeueRegular_ttf, BinaryData::BebasNeueRegular_ttfSize);
            Font cfont = Font(myFont);
            cfont.setHeight(cfont.getHeight() * 0.8f);
            g.setFont(cfont.withExtraKerningFactor(0.095f));
            g.setFont(34.0f);

            g.drawFittedText("Drag a .wav file here", getLocalBounds(), Justification::centred, 2);
        }
    }

    void resized() override
    {
        scrollbar.setBounds(getLocalBounds().removeFromBottom(14).reduced(2));
        Colour c;
        c.fromRGB(38, 46, 57).darker(0.5f);
        scrollbar.setColour(scrollbar.thumbColourId, c.fromRGB(38, 46, 57).darker(0.5f));
        
    }

    void changeListenerCallback(ChangeBroadcaster*) override
    {
        // this method is called by the thumbnail when it has changed, so we should repaint it..
        repaint();
    }

    bool isInterestedInFileDrag(const StringArray& /*files*/) override
    {
        return true;
    }

    void filesDropped(const StringArray& files, int /*x*/, int /*y*/) override
    {
        lastFileDropped = URL(File(files[0]));
        
        sendSynchronousChangeMessage();
    }

    

    void mouseDown(const MouseEvent& event) override
    {

        mouseDrag(event);
        auto duration = transportSource.getLengthInSeconds();

        if (duration > 0.0)
        {
            auto clickPosition = event.position.x;
            auto audioPosition = (clickPosition / getWidth()) * duration;

            transportSource.setPosition(audioPosition);
            posY = event.position.y;
        }
    }

    void mouseDrag(const MouseEvent& e) override
    {
        if (canMoveTransport())
            transportSource.setPosition(jmax(0.0, xToTime((float)e.x)));
    }

    void mouseUp(const MouseEvent&) override
    {
       // transportSource.start();
    }

    void mouseWheelMove(const MouseEvent&, const MouseWheelDetails& wheel) override
    {
        if (thumbnail.getTotalLength() > 0.0)
        {
            auto newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 10.0;
            newStart = jlimit(0.0, jmax(0.0, thumbnail.getTotalLength() - (visibleRange.getLength())), newStart);

            if (canMoveTransport())
                setRange({ newStart, newStart + visibleRange.getLength() });

            if (wheel.deltaY != 0.0f)
                zoomSlider.setValue(zoomSlider.getValue() - wheel.deltaY);

            repaint();
        }
    }

    void paintMarkers(TimeContainerInfo* tci, int samplerate) {
        
        float pos = (float)tci->tpk / (float)samplerate;
        peakMarkers.add(new DrawableRectangle());
        Colour c;
        peakMarkers.getLast()->setFill(c.fromRGB(13,57,176));
        addAndMakeVisible(*peakMarkers.getLast());
        
        peakMarkers.getLast()->setRectangle(Rectangle<float>(timeToX(pos) - 0.75f, 0,
            1.0f, (float)(getHeight() - scrollbar.getHeight())));
        
    }

    void drawLevel(float level, float maxChannel) {

        auto thumbArea = getLocalBounds();
        thumbArea.removeFromBottom(scrollbar.getHeight() + 4);
        auto b = thumbArea.reduced(2).getHeight();
        auto c = juce::jmap(maxChannel,0.0f,1.0f,0.0f, (float)b / 2);
        float a = juce::jmap(level,0.0f,1.0f,(float)b/2, (float)b / 2-c);
        currentLevel.setRectangle(Rectangle<float>(0, a, (float)getWidth(), 1.5f));
    }

 OwnedArray<DrawableRectangle> peakMarkers;
 Range<double> visibleRange;
 DrawableRectangle currentLevel;
float posY;
double position = 0;
private:

    
    AudioTransportSource& transportSource;
    Slider& zoomSlider;
    ScrollBar scrollbar{ false };

    AudioThumbnailCache thumbnailCache{ 5 };
    AudioThumbnail thumbnail;
   
    bool isFollowingTransport = false;
    URL lastFileDropped;
    
    DrawableRectangle currentPositionMarker;
   
    float timeToX(const double time) const
    {
        if (visibleRange.getLength() <= 0)
            return 0;

        return getWidth() * (float)((time - visibleRange.getStart()) / visibleRange.getLength());
    }



    double xToTime(const float x) const
    {
        return (x / getWidth()) * (visibleRange.getLength()) + visibleRange.getStart();
    }

    bool canMoveTransport() const noexcept
    {
        return !(isFollowingTransport && transportSource.isPlaying());
    }

    void scrollBarMoved(ScrollBar* scrollBarThatHasMoved, double newRangeStart) override
    {
        if (scrollBarThatHasMoved == &scrollbar)
            if (!(isFollowingTransport && transportSource.isPlaying()))
                setRange(visibleRange.movedToStartAt(newRangeStart));
    }

    void timerCallback() override
    {

        
        if (canMoveTransport())
            updateCursorPosition();
        else
            setRange(visibleRange.movedToStartAt(transportSource.getCurrentPosition() - (visibleRange.getLength() / 2.0)));

 
    }

    void updateCursorPosition()
    {
       
        position = transportSource.isPlaying()? transportSource.getCurrentPosition():position;
       
        currentPositionMarker.setRectangle(Rectangle<float>(timeToX(position) - 0.75f, 0,
            1.0f, (float)(getHeight() - scrollbar.getHeight())));

      
    }
};



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
        Value sharedValue;
        sharedValue = Random::getSystemRandom().nextDouble() * 100;
        sGain.getValueObject().referTo(sharedValue);
        sGainLeft.getValueObject().referTo(sharedValue);

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

     

              addAndMakeVisible(s);
              s.setRange(0,1,0);
              s.onValueChange = [this] { dtc.setZoomFactor(s.getValue());  specificMarkers(); };

              s.setSkewFactor(2);
              s.setSliderStyle(Slider::LinearHorizontal);
              s.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
              addAndMakeVisible(&dtc);
              s.setColour(Slider::thumbColourId,Colours::orange.withAlpha(0.5f));

              addAndMakeVisible(sGain);
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

             
             
        
        addAndMakeVisible(&quantizeButton);
        quantizeButton.setButtonText("Quantize");
        quantizeButton.onClick = [this] { quantize((BPMTE.getText()).getIntValue(),4, (samplerateTE.getText()).getIntValue(), dtc.getLastDroppedFile().getLocalFile(), outputDirTE.getText()); };
        quantizeButton.setEnabled(false);

        addAndMakeVisible(&detectButton);
        detectButton.setButtonText("Peaks");
        detectButton.onClick = [this] {
            float q = remapValue(sGainLeft.getValue(), maxChannel);
            gainLabel.setText((String)q,dontSendNotification);
        isPeaking = true;
        detectPeaks((BPMTE.getText()).getIntValue(), (samplerateTE.getText()).getIntValue(), 4,q,dtc.getLastDroppedFile().getLocalFile());  
        
        loopMarkers();
        isPeaking = false;
        };

        addAndMakeVisible(&openButton);
        openButton.setButtonText("Open");
        openButton.setTooltip("Open file");
        openButton.onClick = [this] { openButtonClicked(); };

       

       

        addAndMakeVisible(&openImageButton);
        addAndMakeVisible(&playImageButton);
        addAndMakeVisible(&stopImageButton);
        addAndMakeVisible(&pauseImageButton);


        openImageButton.onClick = [this] {openButtonClicked(); };
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

            if(a/b >= c && a/b <d && !tci.operator[](k)->bypassa)
            dtc.paintMarkers(tci.operator[](k), (samplerateTE.getText()).getIntValue());
        }

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
        //dummy comment
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

        
        if (!dtc.getLastDroppedFile().isEmpty()) {

            dtc.drawLevel(sGain.getValue(), maxChannel);
        }
       
      
    }


     void showAudioResource(URL resource)
    {
       
        if (loadURLIntoTransport(resource))
            currentAudioFile =  std::move(resource);


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

     
        if (audioURL.isLocalFile())
        {
            reader = formatManager.createReaderFor(audioURL.getLocalFile());
          
            
        }
       
    

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
                        while (running * realSampleQuantum < counter + i - initSample) {
                            running += 1;
                        }


                        float howManyQuantums = (float)(counter + i - initSample) / sampleQuantum;
                        float mantissa = howManyQuantums - floor(howManyQuantums);
                        int tbk = realSampleQuantum * (mantissa > 0.5f ? ceil(howManyQuantums) : floor(howManyQuantums));




                        tci.add(new TimeContainerInfo(counter + i, tbk + initSample));
                        break;
                    }

                }

            }
            counter += bufferLength;
            preMax = maxL;
        }

        int numPeaks = tci.size();

        for (int k = 0; k < numPeaks; k++) { tci.operator[](k)->bypassa = false;}


        /*FILTRO FALSI POSITIVI*/
        for (int k = 1; k < numPeaks-1; k++) {   // MAX LEVEL < THRESHOLD

            int difference = tci.operator[](k + 1)->tpk - tci.operator[](k)->tpk;
            AudioSampleBuffer* bufferDel = new AudioBuffer<float>(2, difference);
            reader->read(bufferDel, 0, bufferDel->getNumSamples(), tci.operator[](k)->tpk, true, true);
            float a = bufferDel->getMagnitude(0, bufferDel->getNumSamples());
            float b = bufferDel->getRMSLevel(0, 0, bufferDel->getNumSamples());

            if (difference < realSampleQuantum) { tci.operator[](k)->bypassa = true; }

            else if (b < prevRMS) { tci.operator[](k)->bypassa = true;}

            else if (a < CustomThreshold) { tci.operator[](k)->bypassa = true;}
            
            prevRMS = b;
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
            int current = 0;

            

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
           
      
            

            if (tci.operator[](i)->tbk == tci.operator[](i)->tpk && i==0) { //INIT FIRST PEAK
            
                int starting = (tci.operator[](i + 1)->anticipo) ? tci.operator[](i + 1)->tpk : tci.operator[](i + 1)->tbk - tci.operator[](i + 1)->fade;
                float fd_square = tci.operator[](i + 1)->fade * tci.operator[](i + 1)->fade;
                int fd = tci.operator[](i + 1)->fade;

                if (writer != nullptr)
                    writer->writeFromAudioSampleBuffer(*buffer, tci.operator[](i)->tpk, starting - tci.operator[](i)->tpk);
             
                    
                for (int j = 0; j < fd; j++) {

                    float gain = 1 - (float)(j*j)/fd_square;
                    pivot->getWritePointer(0)[j] = gain* buffer->getReadPointer(0)[starting + j];

                   
                }

                if (writer != nullptr)
                    writer->writeFromAudioSampleBuffer(*pivot, starting - fd, fd);
                
                pivot->clear();
                current = starting;
               

            
                
            } //END FIRST
           
            if (anticipo && i >0 && i< tci.size()-1) { //SE ANTICIPO
                
                //DA 0 a delta + fd
                pivot->copyFrom(0, 0, *buffer, 0, tpk-delta-fd, delta + fd);
                pivot->copyFrom(1, 0, *buffer, 1, tpk-delta-fd, delta + fd);
              
                for (int j = 0; j < fd; j++) { 



                    pivot->getWritePointer(0)[j] = pivot->getReadPointer(0)[j]* computeGain(fd, fd_square, j, true)
                                                  + buffer->getReadPointer(0)[tpk-fd-j]*computeGain(fd, fd_square, j, false);

                  /*  if (pivot->getReadPointer(0)[j] == 0) {

                    }*/

                    pivot->getWritePointer(1)[j] = pivot->getReadPointer(0)[j];

                   
                }

                //DA delta + fd a middle+delta
                pivot->copyFrom(0, delta+fd ,*buffer,0,tpk,delta + middle);
                pivot->copyFrom(1, delta+fd, *buffer, 1, tpk, delta + middle);
                int temp = fd + middle+delta;
                
                for (int j = 0; j < delta; j++) { //FADE CRESCENTE DA TPK -DELTA -FADE A TPK -DELTA
                                                    
                    pivot->getWritePointer(0)[temp + j] = pivot->getReadPointer(0)[temp + j]  * computeGain(delta, delta*delta, j, false)
                                                        + buffer->getReadPointer(0)[tbk+middle + j] * computeGain(delta, delta * delta, j, true);
                    pivot->getWritePointer(1)[temp + j] = pivot->getReadPointer(0)[temp + j];
                }


                //DA middle+delta + a tpk+1-fade
                temp = anticipo_1 ? tpk_1 - fd_1 - tbk - middle - delta : tbk_1 - delta_1 - tbk - middle - delta;
                pivot->copyFrom(0, delta + fd + middle + delta, *buffer, 0, tbk + middle + delta, temp);
                pivot->copyFrom(1, delta + fd + middle + delta, *buffer, 1, tbk + middle + delta, temp);

                temp = anticipo_1 ? tpk_1 - fd_1 - tpk + fd : tbk_1 - delta_1 - tpk + fd;
                if (writer != nullptr)
                writer->writeFromAudioSampleBuffer(*pivot, 0, temp);
                
                pivot->clear();

           }
            else if (!tci.operator[](i)->anticipo && i > 0 && i < tci.size() - 1)//SE POSTICIPO MA PRIMA DELL'ULTIMO PICCO
            {
                myfile << "ciao2\n";
                //DA 0 a delta
                pivot->copyFrom(0, 0, *buffer, 0, tbk, delta);
                pivot->copyFrom(1, 0, *buffer, 1, tbk, delta);

                for (int j = 0; j < delta; j++) {
                    pivot->getWritePointer(0)[j] = pivot->getReadPointer(0)[j] * computeGain(delta, delta*delta, j, true)
                        + buffer->getReadPointer(0)[tbk - delta + j] * computeGain(delta, delta*delta, j, false);
                    pivot->getWritePointer(1)[j] = pivot->getReadPointer(0)[j];
                }

                //DA delta + fd a middle+delta
                pivot->copyFrom(0, delta, *buffer, 0, tpk, middle+delta);
                pivot->copyFrom(1, delta, *buffer, 1, tpk, middle+delta);
                int temp = delta + middle;

                for (int j = 0; j < delta; j++) { //FADE CRESCENTE DA TPK -DELTA -FADE A TPK -DELTA

                    pivot->getWritePointer(0)[temp + j] = pivot->getReadPointer(0)[temp + j] * computeGain(delta, delta * delta, j, false)
                                                        + buffer->getReadPointer(0)[tbk + middle + j] * computeGain(delta, delta * delta, j, true);
                    pivot->getWritePointer(1)[temp + j] = pivot->getReadPointer(0)[temp + j];
                }

                //---------------------------------------------
                temp = anticipo_1? tpk_1 - fd_1 - tbk - middle - delta : tbk_1 - delta_1 - tbk - middle - delta;
                pivot->copyFrom(0, delta + middle + delta, *buffer, 0, tbk + middle + delta, temp);
                pivot->copyFrom(1, delta + middle + delta, *buffer, 1, tbk + middle + delta, temp);

                temp = anticipo_1 ? tpk_1 - fd_1 - tbk + delta : tbk_1 - delta_1 - tbk + delta;
                if (writer != nullptr)
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

    void changeState(TransportState newState)
    {
        if (state != newState)
        {
            state = newState;

            switch (state)
            {
            case Stopped:
                stopButton.setEnabled(false);
                playButton.setEnabled(true);
                transportSource.setPosition(0.0);
                break;

            case Starting:
                playButton.setEnabled(false);
                transportSource.start();
                break;

            case Playing:
                stopButton.setEnabled(true);
                break;

            case Stopping:
                transportSource.stop();
                break;

            default:
                jassertfalse;
                break;
            }
        }
    }

    void transportSourceChanged()
    {
        if (transportSource.isPlaying())
            changeState(Playing);
        else
            changeState(Stopped);
    }

    void openButtonClicked()
    {
        FileChooser chooser("Select a Wave file to play...",
            {},
            "*.wav;*.mp3");

        if (chooser.browseForFileToOpen())
        {
            File file = chooser.getResult();
            opened = chooser.getResult();
            showAudioResource(URL(file));
            if (auto* reader = formatManager.createReaderFor(file))
            {
                std::unique_ptr<AudioFormatReaderSource> newSource(new AudioFormatReaderSource(reader, true));
                transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                playButton.setEnabled(true);
                
                
                readerSource.reset(newSource.release());
            }
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
       if(!isPeaking) specificMarkers();
    }
    

    //==========================================================================
    
    TextButton openButton;
    TextButton playButton;
    TextButton stopButton;
    TextButton quantizeButton;
    TextButton detectButton;
    TooltipWindow tooltipWindow{ this }; // instance required for ToolTips to work

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

