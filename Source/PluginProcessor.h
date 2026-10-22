#pragma once
#include <JuceHeader.h>

class VocalForgeAudioProcessor : public juce::AudioProcessor
{
public:
    VocalForgeAudioProcessor();
    ~VocalForgeAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "VocalForge MultiFX"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.1; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    juce::dsp::IIR::Filter<float> hpL, hpR, lpL, lpR;
    juce::dsp::DelayLine<float, juce::dsp:: DelayLineInterpolationTypes::Linear> delayL { 4096 }, delayR { 4096 };
    double sampleRateHz = 44100.0;
    double robotPhase = 0.0;
    juce::Random random;
    float harmonyPhase = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalForgeAudioProcessor)
};
