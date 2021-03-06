/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 24 Jun 2020 10:49:14pm
    Author:  alelo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;
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
        g.drawRect(tickBounds, 1.5f);

        if (ticked)
        {
            g.setColour(component.findColour(ToggleButton::tickColourId));
            auto tick = getTickShape(0.75f);
            g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(5, 6).toFloat(), false));
        }
    }

    void drawButtonText(Graphics& g, TextButton& button,
        bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
    {
        Font font(getTextButtonFont(button, button.getHeight()));
        g.setFont(font);
        g.setFont(font.getHeight() + 4);
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
           /* g.setColour(textEditor.findColour(TextEditor::backgroundColourId));
            g.fillRect(0, 0, width, height);

            g.setColour(textEditor.findColour(TextEditor::outlineColourId));
            g.drawHorizontalLine(height - 1, 0.0f, static_cast<float> (width));*/
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
       
        Colour c;
        g.fillAll(c.fromRGB(38, 46, 57));
        ignoreUnused(width, height);


    }


    void drawGroupComponentOutline(Graphics& g, int width, int height,
        const String& text, const Justification& position,
        GroupComponent& group) override
    {
        const float textH = 25.0f;
        const float indent = 3.0f;
        const float textEdgeGap = 4.0f;
        auto cs = 5.0f;

        Font f(textH);

        Path p;
        auto x = indent;
        auto y = f.getAscent() - 3.0f;
        auto w = jmax(0.0f, width - x * 2.0f);
        auto h = jmax(0.0f, height - y - indent);
        cs = jmin(cs, w * 0.5f, h * 0.5f);
        auto cs2 = 2.0f * cs;

        auto textW = text.isEmpty() ? 0 : jlimit(0.0f, jmax(0.0f, w - cs2 - textEdgeGap * 2), f.getStringWidth(text) + textEdgeGap * 2.0f);
        auto textX = cs + textEdgeGap;

        if (position.testFlags(Justification::horizontallyCentred))
            textX = cs + (w - cs2 - textW) * 0.5f;
        else if (position.testFlags(Justification::right))
            textX = w - cs - textW - textEdgeGap;

        p.startNewSubPath(x + textX + textW, y);
        p.lineTo(x + w - cs, y);

        p.addArc(x + w - cs2, y, cs2, cs2, 0, MathConstants<float>::halfPi);
        p.lineTo(x + w, y + h - cs);

        p.addArc(x + w - cs2, y + h - cs2, cs2, cs2, MathConstants<float>::halfPi, MathConstants<float>::pi);
        p.lineTo(x + cs, y + h);

        p.addArc(x, y + h - cs2, cs2, cs2, MathConstants<float>::pi, MathConstants<float>::pi * 1.5f);
        p.lineTo(x, y + cs);

        p.addArc(x, y, cs2, cs2, MathConstants<float>::pi * 1.5f, MathConstants<float>::twoPi);
        p.lineTo(x + textX, y);

        auto alpha = group.isEnabled() ? 1.0f : 0.5f;

        g.setColour(group.findColour(GroupComponent::outlineColourId)
            .withMultipliedAlpha(alpha));

        g.strokePath(p, PathStrokeType(2.0f));

        g.setColour(group.findColour(GroupComponent::textColourId)
            .withMultipliedAlpha(alpha));
        g.setFont(f);
        g.drawText(text,
            roundToInt(x + textX), 0,
            roundToInt(textW),
            roundToInt(textH),
            Justification::centred, true);
    }

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const Slider::SliderStyle style, Slider& slider)
    {
        if (slider.isBar())
        {
            Colour c;
            g.setColour(c.fromRGB(71, 74, 79));
            //g.setColour(slider.findColour(Slider::trackColourId).brighter(0.05));
            g.fillRect(slider.isHorizontal() ? Rectangle<float>(static_cast<float> (x), y + 0.5f, sliderPos - x, height - 1.0f)
                : Rectangle<float>(x + 0.5f, sliderPos, width - 1.0f, y + (height - sliderPos)));
        }
        else
        {
            auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
            auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

            auto trackWidth = jmin(6.0f, slider.isHorizontal() ? height * 0.25f : width * 0.25f);

            Point<float> startPoint(slider.isHorizontal() ? x : x + width * 0.5f,
                slider.isHorizontal() ? y + height * 0.5f : height + y);

            Point<float> endPoint(slider.isHorizontal() ? width + x : startPoint.x,
                slider.isHorizontal() ? startPoint.y : y);

            Path backgroundTrack;
            backgroundTrack.startNewSubPath(startPoint);
            backgroundTrack.lineTo(endPoint);
//            g.setColour(slider.findColour(Slider::backgroundColourId));
            Colour c;
            g.setColour(c.fromRGB(71, 74, 79));
            g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

            Path valueTrack;
            Point<float> minPoint, maxPoint, thumbPoint;

            if (isTwoVal || isThreeVal)
            {
                minPoint = { slider.isHorizontal() ? minSliderPos : width * 0.5f,
                             slider.isHorizontal() ? height * 0.5f : minSliderPos };

                if (isThreeVal)
                    thumbPoint = { slider.isHorizontal() ? sliderPos : width * 0.5f,
                                   slider.isHorizontal() ? height * 0.5f : sliderPos };

                maxPoint = { slider.isHorizontal() ? maxSliderPos : width * 0.5f,
                             slider.isHorizontal() ? height * 0.5f : maxSliderPos };
            }
            else
            {
                auto kx = slider.isHorizontal() ? sliderPos : (x + width * 0.5f);
                auto ky = slider.isHorizontal() ? (y + height * 0.5f) : sliderPos;

                minPoint = startPoint;
                maxPoint = { kx, ky };
            }

            auto thumbWidth = getSliderThumbRadius(slider);

            valueTrack.startNewSubPath(minPoint);
            valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
           // g.setColour(slider.findColour(Slider::trackColourId));
           
            g.setColour(c.fromRGB(71, 74, 79).darker(0.4f));
            g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

            if (!isTwoVal)
            {
                g.setColour(slider.findColour(Slider::thumbColourId));
                g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint));
            }

            if (isTwoVal || isThreeVal)
            {
                auto sr = jmin(trackWidth, (slider.isHorizontal() ? height : width) * 0.4f);
                auto pointerColour = slider.findColour(Slider::thumbColourId);

                if (slider.isHorizontal())
                {
                    drawPointer(g, minSliderPos - sr,
                        jmax(0.0f, y + height * 0.5f - trackWidth * 2.0f),
                        trackWidth * 2.0f, pointerColour, 2);

                    drawPointer(g, maxSliderPos - trackWidth,
                        jmin(y + height - trackWidth * 2.0f, y + height * 0.5f),
                        trackWidth * 2.0f, pointerColour, 4);
                }
                else
                {
                    drawPointer(g, jmax(0.0f, x + width * 0.5f - trackWidth * 2.0f),
                        minSliderPos - trackWidth,
                        trackWidth * 2.0f, pointerColour, 1);

                    drawPointer(g, jmin(x + width - trackWidth * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                        trackWidth * 2.0f, pointerColour, 3);
                }
            }
        }
    }

   

        void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
            const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
        {
           // auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
            auto outline = Colour(71, 74, 79).darker(0.4f);
            auto fill = Colour(71, 74, 79);
           // auto fill = slider.findColour(Slider::rotarySliderFillColourId);

            auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

            auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
            auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            auto lineW = jmin(8.0f, radius * 0.5f);
            auto arcRadius = radius - lineW * 0.5f;

            Path backgroundArc;
            backgroundArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                rotaryStartAngle,
                rotaryEndAngle,
                true);

            g.setColour(outline);
            g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

            if (slider.isEnabled())
            {
                Path valueArc;
                valueArc.addCentredArc(bounds.getCentreX(),
                    bounds.getCentreY(),
                    arcRadius,
                    arcRadius,
                    0.0f,
                    rotaryStartAngle,
                    toAngle,
                    true);

                g.setColour(fill);
                g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
            }

            auto thumbWidth = lineW * 2.0f;
            Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
                bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

            g.setColour(slider.findColour(Slider::thumbColourId));
            g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
        }


        Font getComboBoxFont(ComboBox& box)
        {
           // return { jmin(30.0f, box.getHeight() * 0.65f) };
            return { box.getHeight() * 0.65f };
        }

        void drawAlertBox(Graphics& g, AlertWindow& alert,
            const Rectangle<int>& textArea, TextLayout& textLayout) override
        {
            auto cornerSize = 4.0f;

            g.setColour(alert.findColour(AlertWindow::outlineColourId));
            g.drawRoundedRectangle(alert.getLocalBounds().toFloat(), cornerSize, 2.0f);

            auto bounds = alert.getLocalBounds().reduced(1);
            g.reduceClipRegion(bounds);

            g.setColour(alert.findColour(AlertWindow::backgroundColourId));
            g.fillRoundedRectangle(bounds.toFloat(), cornerSize);

            auto iconSpaceUsed = 0;

            auto iconWidth = 80;
            auto iconSize = jmin(iconWidth + 50, bounds.getHeight() + 20);

            if (alert.containsAnyExtraComponents() || alert.getNumButtons() > 2)
                iconSize = jmin(iconSize, textArea.getHeight() + 50);

            Rectangle<int> iconRect(iconSize / -10, iconSize / -10,
                iconSize, iconSize);

            if (alert.getAlertType() != AlertWindow::NoIcon)
            {
                Path icon;
                char character;
                uint32 colour;

                if (alert.getAlertType() == AlertWindow::WarningIcon)
                {
                    character = '!';

                    icon.addTriangle(iconRect.getX() + iconRect.getWidth() * 0.5f, (float)iconRect.getY(),
                        static_cast<float> (iconRect.getRight()), static_cast<float> (iconRect.getBottom()),
                        static_cast<float> (iconRect.getX()), static_cast<float> (iconRect.getBottom()));

                    icon = icon.createPathWithRoundedCorners(5.0f);
                    colour = 0x66ff2a00;
                }
                else
                {
                    colour = Colour(0xff00b0b9).withAlpha(0.4f).getARGB();
                    character = alert.getAlertType() == AlertWindow::InfoIcon ? 'i' : '?';

                    icon.addEllipse(iconRect.toFloat());
                }

                GlyphArrangement ga;
                ga.addFittedText({ iconRect.getHeight() * 0.9f, Font::bold },
                    String::charToString((juce_wchar)(uint8)character),
                    static_cast<float> (iconRect.getX()), static_cast<float> (iconRect.getY()),
                    static_cast<float> (iconRect.getWidth()), static_cast<float> (iconRect.getHeight()),
                    Justification::centred, false);
                ga.createPath(icon);

                icon.setUsingNonZeroWinding(false);
                g.setColour(Colour(colour));
                g.fillPath(icon);

                iconSpaceUsed = iconWidth;
            }

            g.setColour(alert.findColour(AlertWindow::textColourId));

            Rectangle<int> alertBounds(bounds.getX() + iconSpaceUsed, 30,
                bounds.getWidth(), bounds.getHeight() - getAlertWindowButtonHeight() - 0);


           
        }

        Font getAlertWindowMessageFont() override { return { 20.0f }; }


        Label* createSliderTextBox(Slider& slider) override
        {
            auto* l = LookAndFeel_V2::createSliderTextBox(slider);

            if (getCurrentColourScheme() == LookAndFeel_V4::getGreyColourScheme() && (slider.getSliderStyle() == Slider::LinearBar
                || slider.getSliderStyle() == Slider::LinearBarVertical))
            {
                l->setColour(Label::textColourId, Colours::black.withAlpha(0.7f));
            }

            l->setColour(Label::outlineWhenEditingColourId, Colours::transparentBlack);
            l->setFont(25);

            return l;
        }

        void drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
            const bool isSeparator, const bool isActive,
            const bool isHighlighted, const bool isTicked,
            const bool hasSubMenu, const String& text,
            const String& shortcutKeyText,
            const Drawable* icon, const Colour* const textColourToUse) override
        {
            if (isSeparator)
            {
                auto r = area.reduced(5, 0);
                r.removeFromTop(roundToInt((r.getHeight() * 0.5f) - 0.5f));

                g.setColour(findColour(PopupMenu::textColourId).withAlpha(0.3f));
                g.fillRect(r.removeFromTop(1));
            }
            else
            {
                auto textColour = (textColourToUse == nullptr ? findColour(PopupMenu::textColourId)
                    : *textColourToUse);

                auto r = area.reduced(1);

                if (isHighlighted && isActive)
                {
                    g.setColour(findColour(PopupMenu::highlightedBackgroundColourId));
                    g.fillRect(r);

                    g.setColour(findColour(PopupMenu::highlightedTextColourId));
                }
                else
                {
                    g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
                }

                r.reduce(jmin(5, area.getWidth() / 20), 0);

                auto font = Font(20);

                auto maxFontHeight = r.getHeight() / 1.3f;

                if (font.getHeight() > maxFontHeight)
                    font.setHeight(maxFontHeight);

                g.setFont(font);

                auto iconArea = r.removeFromLeft(roundToInt(maxFontHeight)).toFloat();

                if (icon != nullptr)
                {
                    icon->drawWithin(g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
                    r.removeFromLeft(roundToInt(maxFontHeight * 0.5f));
                }
                else if (isTicked)
                {
                    auto tick = getTickShape(1.0f);
                    g.fillPath(tick, tick.getTransformToScaleToFit(iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
                }

                if (hasSubMenu)
                {
                    auto arrowH = 0.6f * getPopupMenuFont().getAscent();

                    auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
                    auto halfH = static_cast<float> (r.getCentreY());

                    Path path;
                    path.startNewSubPath(x, halfH - arrowH * 0.5f);
                    path.lineTo(x + arrowH * 0.6f, halfH);
                    path.lineTo(x, halfH + arrowH * 0.5f);

                    g.strokePath(path, PathStrokeType(2.0f));
                }

                r.removeFromRight(3);
                g.drawFittedText(text, r, Justification::centred, 1);

                if (shortcutKeyText.isNotEmpty())
                {
                    auto f2 = font;
                    f2.setHeight(f2.getHeight() * 0.75f);
                    f2.setHorizontalScale(0.95f);
                    g.setFont(f2);

                    g.drawText(shortcutKeyText, r, Justification::centredRight, true);
                }
            }
        }

};

