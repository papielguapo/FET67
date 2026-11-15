#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath>

namespace
{
float gain(float db) { return std::pow(10.0f, db * 0.05f); }
float db(float x) { return 20.0f * std::log10(juce::jmax(1.0e-9f, x)); }
float inputLevel(float x)
{
    if (x <= 0) return 0;
    if (x < 0.1f) return gain(-24.0f) * x * 10.0f;
    return gain(-24.0f + (x - 0.1f) * (54.0f / 9.9f));
}
float attackSeconds(float x) { return 20.0e-6f * std::pow(40.0f, (x - 1.0f) / 8.0f); }
float releaseSeconds(float x) { return 0.05f * std::pow(22.0f, (x - 1.0f) / 8.0f); }
// Feedback topology needs (R - 1) controller gain to produce a settled R:1 slope.
float reduction(float level, float threshold, float knee, float r)
{
    const float over = level - threshold;
    if (over <= -knee * 0.5f) return 0;
    const float slope = r - 1.0f;
    if (over >= knee * 0.5f) return juce::jmin(60.0f, slope * over);
    const float v = over + knee * 0.5f;
    return juce::jmin(60.0f, slope * v * v / (2.0f * knee));
}
}

juce::AudioProcessorValueTreeState::ParameterLayout FET67Processor::makeParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout p;
    const auto addFloat = [&p](const char* id, const char* name, float lo, float hi, float def)
    { p.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id, 1}, name, juce::NormalisableRange<float>(lo, hi), def)); };
    const auto addBool = [&p](const char* id, const char* name, bool def)
    { p.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{id, 1}, name, def)); };
    addBool("power", "Power", true);
    addFloat("input", "Input", 0, 10, 4.5f);
    addFloat("output", "Output", 0, 10, 5);
    addFloat("attack", "Attack (1 fast, 9 slow)", 1, 9, 3);
    addFloat("release", "Release (1 fast, 9 slow)", 1, 9, 4);
    p.add(std::make_unique<juce::AudioParameterInt>(juce::ParameterID{"ratio", 1}, "Ratio", 0, 4, 0));
    addBool("link", "Stereo Link", true);
    addBool("meterMode", "Meter Output", false);
    addBool("bypass", "Bypass", false);
    return p;
}
FET67Processor::FET67Processor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true).withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "FET67", makeParameters())
{
    power = apvts.getRawParameterValue("power"); input = apvts.getRawParameterValue("input");
    output = apvts.getRawParameterValue("output"); attack = apvts.getRawParameterValue("attack");
    release = apvts.getRawParameterValue("release"); ratio = apvts.getRawParameterValue("ratio");
    link = apvts.getRawParameterValue("link"); bypass = apvts.getRawParameterValue("bypass");
}
bool FET67Processor::isBusesLayoutSupported(const BusesLayout& l) const
{
    return (l.getMainInputChannelSet() == juce::AudioChannelSet::mono() || l.getMainInputChannelSet() == juce::AudioChannelSet::stereo())
        && l.getMainInputChannelSet() == l.getMainOutputChannelSet();
}
void FET67Processor::prepareToPlay(double sr, int)
{
    rate = juce::jmax(8000.0, sr);
    for (auto& c : channels)
    {
        c.inputHP.pole = static_cast<float>(std::exp(-juce::MathConstants<double>::twoPi * 18.0 / rate));
        c.fetHP.pole = static_cast<float>(std::exp(-juce::MathConstants<double>::twoPi * 20.0 / rate));
    }
    for (auto* s : { &inputGain, &outputGain, &wet, &attackTime, &releaseTime, &linkMix }) s->reset(rate, 0.02);
    meterRelease = static_cast<float>(std::exp(-1.0 / (rate * 0.25)));
    rmsPole = static_cast<float>(std::exp(-1.0 / (rate * 0.3)));
    reset();
}
void FET67Processor::reset()
{
    for (auto& c : channels) { c.inputHP.clear(); c.fetHP.clear(); c.delayed = c.fast = c.slow = c.lag = c.gr = 0; }
    inputGain.setCurrentAndTargetValue(inputLevel(input->load()));
    outputGain.setCurrentAndTargetValue(gain((output->load() - 5) * 4.8f));
    wet.setCurrentAndTargetValue(power->load() > 0.5f && bypass->load() < 0.5f ? 1.0f : 0.0f);
    attackTime.setCurrentAndTargetValue(attackSeconds(attack->load()));
    releaseTime.setCurrentAndTargetValue(releaseSeconds(release->load()));
    linkMix.setCurrentAndTargetValue(link->load());
    rms = grMeter = 0; currentGrDb.store(0); currentOutDb.store(-100);
    wasBypassed = false;
}
void FET67Processor::processBlock(juce::AudioBuffer<float>& b, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals guard;
    midi.clear();
    const int n = juce::jmin(2, juce::jmin(b.getNumChannels(), getTotalNumInputChannels()));
    for (int ch = n; ch < b.getNumChannels(); ++ch) b.clear(ch, 0, b.getNumSamples());
    if (n == 0) return;
    if (wasBypassed) { reset(); wasBypassed = false; }
    inputGain.setTargetValue(inputLevel(input->load()));
    outputGain.setTargetValue(gain((output->load() - 5) * 4.8f));
    attackTime.setTargetValue(attackSeconds(attack->load()));
    releaseTime.setTargetValue(releaseSeconds(release->load()));
    linkMix.setTargetValue(link->load() > 0.5f ? 1.0f : 0.0f);
    wet.setTargetValue(power->load() > 0.5f && bypass->load() < 0.5f ? 1.0f : 0.0f);
    const int mode = juce::jlimit(0, 4, juce::roundToInt(ratio->load()));
    constexpr float ratios[] {4,8,12,20,24}, thresholds[] {-18,-16.5f,-15,-13.5f,-21.5f}, knees[] {4.5f,3.2f,2.4f,1.5f,7.5f};
    const bool all = mode == 4;
    const float slowCharge = static_cast<float>(std::exp(-1.0 / (rate * 0.15)));
    const float lagPole = static_cast<float>(std::exp(-1.0 / (rate * 0.00018)));
    auto* left = b.getWritePointer(0);
    auto* right = n == 2 ? b.getWritePointer(1) : nullptr;
    for (int i = 0; i < b.getNumSamples(); ++i)
    {
        const float inG = inputGain.getNextValue(), outG = outputGain.getNextValue(), mix = wet.getNextValue();
        const float at = attackTime.getNextValue(), rt = releaseTime.getNextValue(), linked = linkMix.getNextValue();
        const float ac = static_cast<float>(std::exp(-1.0 / (rate * (all ? 0.00045 : at))));
        const float rc = static_cast<float>(std::exp(-1.0 / (rate * rt)));
        const float src = static_cast<float>(std::exp(-1.0 / (rate * rt * (all ? 5.0 : 3.0))));
        const float maxDetector = juce::jmax(std::abs(channels[0].delayed), n == 2 ? std::abs(channels[1].delayed) : 0.0f);
        float energy = 0, maxGr = 0;
        for (int ch = 0; ch < n; ++ch)
        {
            auto& c = channels[static_cast<size_t>(ch)];
            float& sample = ch == 0 ? left[i] : right[i];
            const float dry = std::isfinite(sample) ? sample : 0;
            float x = juce::jlimit(-64.0f, 64.0f, dry * inG);
            // Low-level unity slope; increasing iron curvature with manual input drive.
            x = c.inputHP.tick(std::tanh(x * 0.16f) / 0.16f);
            float detector = std::abs(c.delayed) + linked * (maxDetector - std::abs(c.delayed));
            if (all) { c.lag = lagPole * c.lag + (1 - lagPole) * detector; detector = c.lag; }
            const float target = reduction(db(detector), thresholds[mode], knees[mode], ratios[mode]);
            const float fastPole = target > c.fast ? ac : rc;
            c.fast = fastPole * c.fast + (1 - fastPole) * target;
            const float slowTarget = c.fast * juce::jlimit(0.0f, 0.75f, (c.fast - 3.0f) / 24.0f);
            const float sp = slowTarget > c.slow ? slowCharge : src;
            c.slow = sp * c.slow + (1 - sp) * slowTarget;
            c.gr = juce::jlimit(0.0f, 60.0f, juce::jmax(c.fast, c.slow));
            x *= gain(-c.gr);
            c.delayed = x;
            // Rev A asymmetry, with bounded LN compensation of higher-order curvature.
            const float drive = juce::jmin(1.0f, c.gr / 24.0f);
            const float even = 0.055f * drive * x * x / (1 + x * x);
            const float fet = x + even - 0.008f * drive * x * x * x / (1 + x * x);
            x = c.fetHP.tick(fet) * outG;
            // -18 dBFS RMS = 0 VU; +18 dBu aligns with a -4 dBFS peak knee
            // when 0 VU is +4 dBu. The requested -12 peak mark is a separate display reference.
            constexpr float knee = 0.63095734f;
            const float a = std::abs(x);
            if (a > knee) x = std::copysign(knee + 0.35f * std::tanh((a - knee) / 0.35f), x);
            sample = mix <= 0 ? dry : dry + mix * (x - dry);
            energy = juce::jmax(energy, sample * sample);
            maxGr = juce::jmax(maxGr, c.gr * mix);
        }
        rms = rmsPole * rms + (1 - rmsPole) * energy;
        grMeter = juce::jmax(maxGr, grMeter * meterRelease);
    }
    currentGrDb.store(grMeter, std::memory_order_relaxed);
    currentOutDb.store(juce::jmax(-100.0f, 10.0f * std::log10(juce::jmax(1.0e-10f, rms))), std::memory_order_relaxed);
}
void FET67Processor::processBlockBypassed(juce::AudioBuffer<float>& b, juce::MidiBuffer& m)
{
    m.clear();
    wasBypassed = true;
    const int n = juce::jmin(b.getNumChannels(), getTotalNumInputChannels());
    for (int c = n; c < b.getNumChannels(); ++c) b.clear(c, 0, b.getNumSamples());
    for (int i = 0; i < b.getNumSamples(); ++i)
    {
        float e = 0;
        for (int c = 0; c < n; ++c) { const float x = b.getSample(c,i); e = juce::jmax(e,x*x); }
        rms = rmsPole*rms + (1-rmsPole)*e;
    }
    currentGrDb.store(0); currentOutDb.store(10*std::log10(juce::jmax(1.0e-10f,rms)));
}
juce::AudioProcessorParameter* FET67Processor::getBypassParameter() const { return apvts.getParameter("bypass"); }
void FET67Processor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = apvts.copyState();
    state.setProperty("editorWidth", editorWidth.load(), nullptr);
    state.setProperty("editorHeight", editorHeight.load(), nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary(*xml, dest);
}
void FET67Processor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(apvts.state.getType()))
        {
            auto state = juce::ValueTree::fromXml(*xml);
            editorWidth.store(juce::jlimit(817, 4085, static_cast<int>(state.getProperty("editorWidth", 1634))));
            editorHeight.store(juce::jlimit(126, 630, static_cast<int>(state.getProperty("editorHeight", 252))));
            apvts.replaceState(state);
        }
}
juce::AudioProcessorEditor* FET67Processor::createEditor() { return new FET67Editor(*this); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new FET67Processor(); }
