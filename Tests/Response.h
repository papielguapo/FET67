#pragma once
inline int calibrateInput(juce::AudioProcessor& p)
{
    juce::AudioProcessorParameter* input=nullptr;
    for(auto* parameter:p.getParameters())if(parameter->getName(100)=="Input")input=parameter;
    require(input!=nullptr,"No public Input parameter");
    p.setRateAndBufferSizeDetails(48000,128);p.prepareToPlay(48000,128);
    float lo=0.001f,hi=0.95f,value=0;double attenuation=0;
    for(int iteration=0;iteration<12;++iteration)
    {
        value=(lo+hi)*0.5f;input->setValueNotifyingHost(value);
        // Long settling avoids mistaking program-dependent release history for gain.
        const double small=sine(p,1e-5f,12);
        const double large=sine(p,0.5f,6);
        attenuation=small-large+20*std::log10(0.5/1e-5);
        std::cout<<"CALIBRATION_STEP "<<iteration<<" input="<<value<<" attenuation="<<attenuation<<"\n";
        if(attenuation>6)hi=value;else lo=value;
    }
    std::cout<<"CALIBRATION normalized_input="<<value<<" effective_attenuation_dB="<<attenuation<<" input_text="<<input->getText(value,100)<<"\n";
    p.releaseResources();require(std::abs(attenuation-6)<0.1,"Effective attenuation calibration did not converge");return 0;
}
inline int settledResponse(juce::AudioProcessor& p, float amplitude=1e-5f)
{
    juce::MidiBuffer midi;
    std::cout<<"RESPONSE,sample_rate,Hz,gain_dB\n";
    for(double sr:{44100.,48000.,96000.})for(double hz:{1.,2.,5.,10.,20.,50.,100.,200.,1000.,5000.,10000.,15000.,20000.})
    {
        p.setRateAndBufferSizeDetails(sr,128);p.prepareToPlay(sr,128);p.reset();
        double re=0,im=0;int measured=0;
        for(int offset=0;offset<static_cast<int>(sr*3);offset+=128)
        {
            const int count=juce::jmin(128,static_cast<int>(sr*3)-offset);
            juce::AudioBuffer<float> b(juce::jmax(2,p.getTotalNumInputChannels()),count);b.clear();
            for(int i=0;i<count;++i)for(int ch=0;ch<p.getMainBusNumInputChannels();++ch)
                b.setSample(ch,i,amplitude*std::sin(juce::MathConstants<double>::twoPi*hz*(offset+i)/sr));
            p.processBlock(b,midi);
            for(int i=0;i<count;++i)if(offset+i>=static_cast<int>(sr*2))
            {
                const double phase=juce::MathConstants<double>::twoPi*hz*(offset+i)/sr;
                const float x=b.getSample(0,i);require(std::isfinite(x),"Response contains nonfinite output");
                re+=x*std::cos(phase);im+=x*std::sin(phase);++measured;
            }
        }
        const double response=20*std::log10(juce::jmax(1e-20,2*std::hypot(re,im)/(measured*amplitude)));
        std::ostringstream line;line<<"RESPONSE,"<<sr<<","<<hz<<","<<response<<"\n";std::cout<<line.str();
        p.releaseResources();
    }
    return 0;
}
