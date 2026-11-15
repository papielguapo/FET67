#pragma once
#include "PluginProcessor.h"
#include <juce_gui_basics/juce_gui_basics.h>

inline constexpr int kBaseWidth = 1140, kBaseHeight = 156;

class FET67LookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics&, int, int, int, int, float, float, float, juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool, bool) override;
};

class FaceplateComponent final : public juce::Component, private juce::Timer
{
public:
    explicit FaceplateComponent(FET67Processor&);
    ~FaceplateComponent() override;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    void timerCallback() override;
    void paintMeter(juce::Graphics&);
    FET67Processor& processor;
    FET67LookAndFeel look;
    std::array<juce::Slider,5> knobs;
    std::array<juce::ToggleButton,4> switches;
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::array<std::unique_ptr<SA>,5> knobAttachments;
    std::array<std::unique_ptr<BA>,4> switchAttachments;
    float shownGr = 0, shownOut = -100;
    juce::TooltipWindow tooltips { this, 500 };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FaceplateComponent)
};

class FET67Editor final : public juce::AudioProcessorEditor
{
public:
    explicit FET67Editor(FET67Processor&);
    void resized() override;
private:
    FET67Processor& processor;
    FaceplateComponent faceplate;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FET67Editor)
};
