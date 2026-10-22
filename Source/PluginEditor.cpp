#include "PluginEditor.h"

VocalForgeAudioProcessorEditor::VocalForgeAudioProcessorEditor(VocalForgeAudioProcessor& p)
 : AudioProcessorEditor(&p), processor(p)
{
    setSize(680, 420);
    title.setText("VOCALFORGE  |  MULTI-FX VOCAL", juce::dontSendNotification);
    title.setFont(juce::Font(24.0f, juce::Font::bold));
    title.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(title);

    juce::ToggleButton* buttons[] = { &chipButton, &robotButton, &toaButton, &doublerButton, &harmonyButton };
    for (auto* b : buttons) addAndMakeVisible(*b);

    auto setupSlider = [this](juce::Slider& s, juce::Label& l, const juce::String& text) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(s); addAndMakeVisible(l);
    };
    setupSlider(pitch, pitchLabel, "Chip pitch");
    setupSlider(robotFreq, robotLabel, "Robot Hz");
    setupSlider(drive, driveLabel, "TOA drive");
    setupSlider(delay, delayLabel, "Doubler delay");
    setupSlider(output, outputLabel, "Output dB");

    auto& apvts = processor.parameters;
    aChip = std::make_unique<ButtonAttachment>(apvts, "chip.on", chipButton);
    aRobot = std::make_unique<ButtonAttachment>(apvts, "robot.on", robotButton);
    aToa = std::make_unique<ButtonAttachment>(apvts, "toa.on", toaButton);
    aDoubler = std::make_unique<ButtonAttachment>(apvts, "doubler.on", doublerButton);
    aHarmony = std::make_unique<ButtonAttachment>(apvts, "harmony.on", harmonyButton);
    aPitch = std::make_unique<SliderAttachment>(apvts, "chip.pitch", pitch);
    aRobotFreq = std::make_unique<SliderAttachment>(apvts, "robot.freq", robotFreq);
    aDrive = std::make_unique<SliderAttachment>(apvts, "toa.drive", drive);
    aDelay = std::make_unique<SliderAttachment>(apvts, "doubler.delay", delay);
    aOutput = std::make_unique<SliderAttachment>(apvts, "output", output);
}
void VocalForgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(18, 23, 35));
    g.setColour(juce::Colour(45, 58, 78));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(8), 12.0f);
    g.setColour(juce::Colours::white);
    g.drawFittedText("MVP build: pitch-shift and harmony algorithms need production DSP modules.",
        18, getHeight() - 42, getWidth() - 36, 24, juce::Justification::centred, 1);
}
void VocalForgeAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(18);
    title.setBounds(area.removeFromTop(48));
    auto row = area.removeFromTop(38);
    const int bw = row.getWidth() / 5;
    chipButton.setBounds(row.removeFromLeft(bw));
    robotButton.setBounds(row.removeFromLeft(bw));
    toaButton.setBounds(row.removeFromLeft(bw));
    doublerButton.setBounds(row.removeFromLeft(bw));
    harmonyButton.setBounds(row);
    area.removeFromTop(10);
    auto sliders = area.removeFromTop(230);
    const int sw = sliders.getWidth() / 5;
    juce::Slider* ss[] = { &pitch, &robotFreq, &drive, &delay, &output };
    juce::Label* ll[] = { &pitchLabel, &robotLabel, &driveLabel, &delayLabel, &outputLabel };
    for (int i = 0; i < 5; ++i) {
        auto cell = sliders.removeFromLeft(sw);
        ll[i]->setBounds(cell.removeFromTop(28));
        ss[i]->setBounds(cell.reduced(4));
    }
}
