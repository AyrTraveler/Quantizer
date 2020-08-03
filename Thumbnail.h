/*
  ==============================================================================

    Thumbnail.h
    Created: 24 Jun 2020 5:42:33pm
    Author:  alelo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "TimeContainerInfo.h"
using namespace juce;
class CustomRect : public DrawableRectangle
{
    public:
    
    void setSample (float s){
    
        sample = s;
    }
    
    float getSample(){
      return sample;
    }
    
    
  private:
    float sample;
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
            thumbnail.drawChannel(g, thumbArea.reduced(2), visibleRange.getStart(), visibleRange.getEnd(), 0, 1.0f);
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

            if (!deleteActive)
                paintSingleMarker(clickPosition);
            else
                deleteSingleMarker(clickPosition);
        }
        
        
          
    }
    
    void smartClear() {
    
        for (int i = 0; i < peakMarkers.size(); i++) {
          peakMarkers.operator[](i)->setVisible(false);
      }
      peakMarkers.clear();
    }
    
     void paintSingleMarker(float x) {

         if (x<latestCLick) return;
        
         latestCLick = x;

        peakMarkers.add(new CustomRect());
       
        peakMarkers.getLast()->setFill(Colours::cyan);
        addAndMakeVisible(*peakMarkers.getLast());

        peakMarkers.getLast()->setRectangle(Rectangle<float>(x - 0.75f, 0,
            0.4f, (float)(getHeight() - scrollbar.getHeight())));
            
       

            peakMarkers.getLast()->setSample(xToTime(x));
            isDown = true;
    }

     void deleteSingleMarker(float x) {

         float deleteTime = xToTime(x);
         float difference = 1000000.0f;
         int toDelete = -1;
         for (int i = 0; i < peakMarkers.size(); i ++) {

             float i_time = peakMarkers.operator[](i)->getSample();

             if (fabs(i_time - deleteTime) < difference) {

              
                 difference = fabs(i_time - deleteTime);
                 toDelete = i;
                 transientToDelete = i_time;
             }
         }


         peakMarkers.operator[](toDelete)->setVisible(false);
         peakMarkers.remove(toDelete,true);
        
         isDown = true;
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
        /*if (thumbnail.getTotalLength() > 0.0)
        {
           auto newStart = visibleRange.getStart() - wheel.deltaX * (visibleRange.getLength()) / 200.0;
            newStart = jlimit(0.0, jmax(0.0, thumbnail.getTotalLength() - (visibleRange.getLength())), newStart);

            if (canMoveTransport())
                setRange({ newStart, newStart + visibleRange.getLength() });

            if (wheel.deltaY != 0.0f)
                zoomSlider.setValue(zoomSlider.getValue() - wheel.deltaY);

            repaint();
        }*/
    }

    void paintMarkers(TimeContainerInfo* tci, int samplerate) {

        float pos = (float)tci->tpk / (float)samplerate;
        peakMarkers.add(new CustomRect());
        
        peakMarkers.getLast()->setFill(Colours::deeppink);
        addAndMakeVisible(*peakMarkers.getLast());

        peakMarkers.getLast()->setRectangle(Rectangle<float>(timeToX(pos) - 0.75f, 0,
            1.0f, (float)(getHeight() - scrollbar.getHeight())));

    }

    void smartPaint(float audioPos, int pos, bool isPeak, bool isUser) {
        
        
        if (isPeak) {
            peakMarkers.add(new CustomRect());
            Colour c = isUser? juce::Colours::cyan : juce::Colours::deeppink;
            peakMarkers.getLast()->setFill(c);
        }
            
        else {
            gridMarkers.add(new CustomRect());
            gridMarkers.getLast()->setFill(juce::Colours::lightgreen);
        }
           
        CustomRect* last = isPeak ? peakMarkers.getLast() : gridMarkers.getLast();

        last->setSample(audioPos);
        addAndMakeVisible(*last);
        float w = (pos % 4 == 0) ? 1.0f: 0.4f;

        last->setRectangle(Rectangle<float>(timeToX(audioPos) - 0.75f, 0,
           w, (float)(getHeight() - scrollbar.getHeight())));
    }

    void drawLevel(float level, float maxChannel) {

        auto thumbArea = getLocalBounds();
        thumbArea.removeFromBottom(scrollbar.getHeight() + 4);
        auto b = thumbArea.reduced(2).getHeight();
        auto c = juce::jmap(maxChannel, 0.0f, 1.0f, 0.0f, (float)b / 2);
        float a = juce::jmap(level, 0.0f, 1.0f, (float)b / 2, (float)b / 2 - c);
        currentLevel.setRectangle(Rectangle<float>(0, a, (float)getWidth(), 1.5f));
    }

    OwnedArray<CustomRect> peakMarkers;
    OwnedArray<CustomRect> gridMarkers;
    Range<double> visibleRange;
    DrawableRectangle currentLevel;
    float posY;
    double position = 0;
    float transientToDelete = -1;
    bool isDown = false;
    bool deleteActive = false;
    bool isCreated = true;
    bool scrollMoved = false;
    bool showGrid= true;
private:


    AudioTransportSource& transportSource;
    Slider& zoomSlider;
    ScrollBar scrollbar{ false };
    float latestCLick = 0.0f;
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

        scrollMoved = true;

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

        position = transportSource.isPlaying() ? transportSource.getCurrentPosition() : position;

        currentPositionMarker.setRectangle(Rectangle<float>(timeToX(position) - 0.75f, 0,
            1.0f, (float)(getHeight() - scrollbar.getHeight())));


    }
};
