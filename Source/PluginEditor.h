#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class VocalForgeAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit VocalForgeAudioProcessorEditor(VocalForgeAudioProcessor&);
    ~VocalForgeAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;
private:
    VocalForgeAudioProcessor& processor;
    juce::Label title;
    juce::ToggleButton chipButton {"Chipmunk"}, robotButton {"Robot"},
        toaButton {"Megaphone"}, doublerButton {"Doubler"}, harmonyButton {"Harmony"};
    juce::Slider pitch, robotFreq, drive, delay, output;
    juce::Label pitchLabel, robotLabel, driveLabel, delayLabel, outputLabel;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<ButtonAttachment> aChip, aRobot, aToa, aDoubler, aHarmony;
    std::unique_ptr<SliderAttachment> aPitch, aRobotFreq, aDrive, aDelay, aOutput;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalForgeAudioProcessorEditor)
};
