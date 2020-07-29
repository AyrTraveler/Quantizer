/*
  ==============================================================================

    CustomLookAndFeel.h
    Created: 24 Jun 2020 10:49:14pm
    Author:  alelo

  ==============================================================================
*/
 
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

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
            g.fillPath(tick, tick.getTransformToScaleToFit(tickBounds.reduced(4, 5).toFloat(), false));
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

