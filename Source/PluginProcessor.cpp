#include "PluginProcessor.h"
#include "PluginEditor.h"

static juce::AudioProcessorValueTreeState::ParameterLayout makeParams()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout p;
    auto b = [&p](const char* id, const char* name, bool v) {
        p.add(std::make_unique<AudioParameterBool>(ParameterID{id, 1}, name, v));
    };
    auto f = [&p](const char* id, const char* name, float lo, float hi, float v, const char* unit = "") {
        p.add(std::make_unique<AudioParameterFloat>(ParameterID{id, 1}, name,
            NormalisableRange<float>(lo, hi), v, AudioParameterFloatAttributes().withLabel(unit)));
    };
    b("chip.on", "Chipmunk", false);
    b("robot.on", "Robot", false);
    b("toa.on", "Megaphone", false);
    b("doubler.on", "Doubler", false);
    b("harmony.on", "Harmony", false);
    f("chip.pitch", "Chip Pitch", 5.0f, 12.0f, 7.0f, "st");
    f("chip.mix", "Chip Mix", 0.0f, 100.0f, 100.0f, "%");
    f("robot.freq", "Robot Carrier", 40.0f, 400.0f, 100.0f, "Hz");
    f("robot.mix", "Robot Mix", 0.0f, 100.0f, 50.0f, "%");
    f("toa.drive", "TOA Drive", 0.0f, 12.0f, 3.0f, "dB");
    f("toa.mix", "TOA Mix", 0.0f, 100.0f, 100.0f, "%");
    f("doubler.delay", "Doubler Delay", 10.0f, 40.0f, 18.0f, "ms");
    f("doubler.width", "Doubler Width", 0.0f, 100.0f, 70.0f, "%");
    f("doubler.mix", "Doubler Mix", 0.0f, 100.0f, 30.0f, "%");
    f("harmony.interval", "Harmony Interval", 3.0f, 7.0f, 4.0f, "semitones");
    f("harmony.mix", "Harmony Mix", 0.0f, 100.0f, 25.0f, "%");
    f("output", "Output", -24.0f, 6.0f, 0.0f, "dB");
    return p;
}

VocalForgeAudioProcessor::VocalForgeAudioProcessor()
 : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                   .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
   parameters(*this, nullptr, "PARAMETERS", makeParams())
{
}

bool VocalForgeAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    auto in = l.getMainInputChannelSet();
    auto out = l.getMainOutputChannelSet();
    return in == out && (in == juce::AudioChannelSet::mono() || in == juce::AudioChannelSet::stereo());
}

void VocalForgeAudioProcessor::prepareToPlay(double sr, int block)
{
    sampleRateHz = sr;
    juce::dsp::ProcessSpec spec { sr, static_cast<juce::uint32>(block), 1 };
    hpL.prepare(spec); hpR.prepare(spec); lpL.prepare(spec); lpR.prepare(spec);
    auto hp = juce::dsp::IIR::Coefficients<float>::makeHighPass(sr, 300.0);
    auto lp = juce::dsp::IIR::Coefficients<float>::makeLowPass(sr, 4000.0);
    hpL.coefficients = hp; hpR.coefficients = hp; lpL.coefficients = lp; lpR.coefficients = lp;
    hpL.reset(); hpR.reset(); lpL.reset(); lpR.reset();
    delayL.prepare(spec); delayR.prepare(spec);
    delayL.setMaximumDelayInSamples(4095); delayR.setMaximumDelayInSamples(4095);
    delayL.reset(); delayR.reset();
}

void VocalForgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ignoreUnused(midi);
    juce::ScopedNoDenormals noDenormals;
    const int channels = buffer.getNumChannels(), n = buffer.getNumSamples();
    auto val = [this](const char* id) { return parameters.getRawParameterValue(id)->load(); };
    const bool chip = val("chip.on") > 0.5f;
    const bool robot = val("robot.on") > 0.5f;
    const bool toa = val("toa.on") > 0.5f;
    const bool doubler = val("doubler.on") > 0.5f;
    const bool harmony = val("harmony.on") > 0.5f;
    const float chipMix = val("chip.mix") * .01f;
    const float chipPitch = val("chip.pitch");
    const float robotMix = val("robot.mix") * .01f;
    const float robotHz = val("robot.freq");
    const float toaMix = val("toa.mix") * .01f;
    const float drive = std::pow(10.0f, val("toa.drive") / 20.0f);
    const float delayMs = val("doubler.delay");
    const float width = val("doubler.width") * .01f;
    const float doublerMix = val("doubler.mix") * .01f;
    const float harmonyMix = val("harmony.mix") * .01f;
    const float harmonyInterval = val("harmony.interval");
    const float outGain = juce::Decibels::decibelsToGain(val("output"));

    for (int i = 0; i < n; ++i)
    {
        const float dryL = buffer.getSample(0, i);
        const float dryR = channels > 1 ? buffer.getSample(1, i) : dryL;
        float l = dryL, r = dryR;

        // NOTE: This MVP uses a crude resampling-like pitch approximation only for
        // demonstration; it is NOT a duration-preserving production pitch shifter.
        if (chip)
        {
            const float ratio = std::pow(2.0f, chipPitch / 12.0f);
            const float shapedL = std::tanh(l * ratio);
            const float shapedR = std::tanh(r * ratio);
            l = l * (1.0f - chipMix) + shapedL * chipMix;
            r = r * (1.0f - chipMix) + shapedR * chipMix;
        }

        if (robot)
        {
            const float carrier = std::sin(juce::MathConstants<double>::twoPi * robotPhase);
            l = l * (1.0f - robotMix) + (l * carrier) * robotMix;
            r = r * (1.0f - robotMix) + (r * carrier) * robotMix;
        }

        if (toa)
        {
            const float fl = lpL.processSample(hpL.processSample(l));
            const float fr = channels > 1 ? lpR.processSample(hpR.processSample(r)) : fl;
            l = l * (1.0f - toaMix) + std::tanh(drive * fl) * toaMix;
            r = r * (1.0f - toaMix) + std::tanh(drive * fr) * toaMix;
        }

        if (doubler)
        {
            const int d = juce::jlimit(1, 4095, (int)(delayMs * 0.001 * sampleRateHz));
            delayL.setDelay((float)d);
            delayR.setDelay((float)juce::jlimit(1, 4095, d + (int)(width * 0.012f * sampleRateHz)));
            const float dl = delayL.popSample(0);
            const float dr = delayR.popSample(0);
            delayL.pushSample(0, l);
            delayR.pushSample(0, r);
            l = l * (1.0f - doublerMix) + dl * doublerMix;
            r = r * (1.0f - doublerMix) + dr * doublerMix;
        }

        // Harmony placeholder: ring-modulated voice is a temporary audible demo,
        // NOT pitch-detected diatonic harmony. Replace with pitch detector + shifter.
        if (harmony)
        {
            const float carrier = std::sin(juce::MathConstants<float>::twoPi * harmonyPhase);
            const float hm = harmonyMix;
            l = l * (1.0f - hm) + (l * carrier) * hm;
            r = r * (1.0f - hm) + (r * carrier) * hm;
            harmonyPhase += (float)(220.0 * std::pow(2.0, harmonyInterval / 12.0) / sampleRateHz);
            if (harmonyPhase >= 1.0f) harmonyPhase -= 1.0f;
        }

        buffer.setSample(0, i, l * outGain);
        if (channels > 1) buffer.setSample(1, i, r * outGain);
        robotPhase += robotHz / sampleRateHz;
        if (robotPhase >= 1.0) robotPhase -= 1.0;
    }
}

void VocalForgeAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}
void VocalForgeAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}
juce::AudioProcessorEditor* VocalForgeAudioProcessor::createEditor()
{
    return new VocalForgeAudioProcessorEditor(*this);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalForgeAudioProcessor();
}
