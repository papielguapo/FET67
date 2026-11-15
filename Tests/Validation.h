#pragma once
#include <chrono>
#include <limits>
#include <vector>

void beginAllocationMonitoring() noexcept;
unsigned long endAllocationMonitoring() noexcept;

namespace validation
{
inline void spectrum(juce::AudioProcessor& p)
{
    p.setRateAndBufferSizeDetails(48000,128);p.prepareToPlay(48000,128);
    juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
    std::array<double,4> real{},imag{};
    for(int offset=0;offset<192000;offset+=128)
    {
        for(int i=0;i<128;++i){const float x=0.5f*static_cast<float>(std::sin(juce::MathConstants<double>::twoPi*1000*(offset+i)/48000.0));b.setSample(0,i,x);b.setSample(1,i,x);}
        p.processBlock(b,midi);
        if(offset>=144000)for(int i=0;i<128;++i)for(size_t h=0;h<4;++h)
        {
            const double phase=juce::MathConstants<double>::twoPi*1000*static_cast<double>(h+1)*(offset+i)/48000.0;
            const double x=b.getSample(0,i);real[h]+=x*std::cos(phase);imag[h]+=x*std::sin(phase);
        }
    }
    const double fundamental=std::hypot(real[0],imag[0]);
    for(size_t h=0;h<4;++h)std::cout<<"SPECTRUM harmonic="<<h+1<<" dBc="<<20*std::log10(juce::jmax(1e-15,std::hypot(real[h],imag[h])/fundamental))<<"\n";
    p.releaseResources();
}
inline void fillTone(juce::AudioBuffer<float>& b, int offset, double frequency, double sampleRate=48000, float amplitude=0.5f)
{
    for(int i=0;i<b.getNumSamples();++i)
    {
        const float x=amplitude*static_cast<float>(std::sin(juce::MathConstants<double>::twoPi*frequency*(offset+i+12)/sampleRate));
        for(int c=0;c<b.getNumChannels();++c)b.setSample(c,i,x);
    }
}

inline void bypassAndMeter()
{
    FET67Processor switched, control;
    for(auto* p:{&switched,&control}){set(*p,"input",10);set(*p,"attack",9);p->prepareToPlay(48000,128);}
    juce::AudioBuffer<float> a(2,128),b(2,128);juce::MidiBuffer midi;
    int offset=0;
    for(int block=0;block<1500;++block){fillTone(a,offset,1000);b.makeCopyOf(a);switched.processBlock(a,midi);control.processBlock(b,midi);offset+=128;}
    fillTone(a,offset,1000);b.makeCopyOf(a);switched.processBlockBypassed(a,midi);control.processBlock(b,midi);offset+=128;
    fillTone(a,offset,1000);b.makeCopyOf(a);switched.processBlock(a,midi);control.processBlock(b,midi);offset+=128;
    const float excess=20*std::log10(a.getMagnitude(0,0,128)/b.getMagnitude(0,0,128));
    require(excess<1.5f,"Host bypass return reintroduces an uncompressed peak");
    for(int block=0;block<500;++block){fillTone(a,offset,1000);b.makeCopyOf(a);switched.processBlockBypassed(a,midi);control.processBlock(b,midi);offset+=128;}
    float residual=0;
    for(int block=0;block<20;++block)
    {
        fillTone(a,offset,1000);b.makeCopyOf(a);switched.processBlock(a,midi);control.processBlock(b,midi);offset+=128;
        if(block>=10)for(int i=0;i<128;++i)residual=juce::jmax(residual,std::abs(a.getSample(0,i)-b.getSample(0,i)));
    }
    require(residual<1e-6f,"Host bypass loses compressor or filter history");
    std::cout<<"BYPASS first-block excess_dB="<<excess<<" after-fade residual="<<residual<<" latency_samples="<<switched.getLatencySamples()<<"\n";

    FET67Processor meter;set(meter,"bypass",1);meter.prepareToPlay(48000,128);
    juce::AudioBuffer<float> m(2,48000);
    for(int j=0;j<4;++j)
    {
        for(int i=0;i<48000;++i){m.setSample(0,i,i%2==0?0.25f:0);m.setSample(1,i,i%2!=0?0.25f:0);}
        meter.processBlock(m,midi);
    }
    const float expected=20*std::log10(0.25f/std::sqrt(2.0f));
    require(std::abs(meter.currentOutDb.load()-expected)<0.05f,"Stereo RMS overstates channel energy");
    std::cout<<"METER measured_dB="<<meter.currentOutDb.load()<<" expected_dB="<<expected<<"\n";
}

inline double aliasProbe(double sampleRate, double frequency)
{
    FET67Processor p;set(p,"input",10);set(p,"attack",9);p.prepareToPlay(sampleRate,128);
    juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
    const int samples=static_cast<int>(sampleRate)*4;
    double realFund=0,imagFund=0,realAlias=0,imagAlias=0;
    const double harmonic=frequency*3;
    const double folded=std::abs(harmonic-sampleRate*std::round(harmonic/sampleRate));
    for(int offset=0;offset<samples;offset+=128)
    {
        fillTone(b,offset,frequency,sampleRate);p.processBlock(b,midi);
        for(int i=0;i<128;++i)if(offset+i>=samples-static_cast<int>(sampleRate)&&offset+i<samples)
        {
            const double t=(offset+i)/sampleRate,x=b.getSample(0,i);
            realFund+=x*std::cos(juce::MathConstants<double>::twoPi*frequency*t);
            imagFund+=x*std::sin(juce::MathConstants<double>::twoPi*frequency*t);
            realAlias+=x*std::cos(juce::MathConstants<double>::twoPi*folded*t);
            imagAlias+=x*std::sin(juce::MathConstants<double>::twoPi*folded*t);
        }
    }
    const double dBc=20*std::log10(juce::jmax(1e-15,std::hypot(realAlias,imagAlias)/std::hypot(realFund,imagFund)));
    std::cout<<"ALIAS sr="<<sampleRate<<" input_Hz="<<frequency<<" folded_Hz="<<folded<<" dBc="<<dBc<<"\n";
    return dBc;
}

inline void latencyAndRobustness()
{
    juce::MidiBuffer midi;
    for(double sr:{44100.0,48000.0,88200.0,96000.0,176400.0,192000.0})
    {
        FET67Processor p;set(p,"bypass",1);p.prepareToPlay(sr,16);
        const int latency=p.getLatencySamples();require(latency>0&&latency<2048,"Unreasonable oversampling latency");
        juce::AudioBuffer<float> impulse(2,4097);impulse.clear();impulse.setSample(0,0,1);impulse.setSample(1,0,-1);
        p.processBlock(impulse,midi);
        for(int i=0;i<4097;++i){require(impulse.getSample(0,i)==(i==latency?1.f:0.f),"Dry impulse disagrees with reported latency");require(impulse.getSample(1,i)==(i==latency?-1.f:0.f),"Dry channels differ");}
        set(p,"bypass",0);p.reset();
        for(int i=0;i<4097;++i){impulse.setSample(0,i,i%11==0?std::numeric_limits<float>::quiet_NaN():i%7==0?std::numeric_limits<float>::infinity():0.1f);impulse.setSample(1,i,i%13==0?-std::numeric_limits<float>::max():0.2f);}
        p.processBlock(impulse,midi);
        for(int i=0;i<4097;++i)for(int c=0;c<2;++c)require(std::isfinite(impulse.getSample(c,i)),"Invalid input poisoned audio state");
        require(std::isfinite(p.currentOutDb.load()),"Invalid input poisoned meter state");
        juce::AudioBuffer<float> empty(2,0);p.processBlock(empty,midi);p.processBlockBypassed(empty,midi);
        std::cout<<"ROBUST sr="<<sr<<" oversized=4097 prepared=16 latency="<<latency<<" PASS\n";
    }
}

inline std::vector<float> renderBlocks(const std::vector<int>& sizes)
{
    FET67Processor p;p.prepareToPlay(48000,16);juce::MidiBuffer midi;
    std::vector<float> result;result.reserve(24000);
    size_t index=0;int offset=0;
    while(offset<24000)
    {
        const int count=juce::jmin(sizes[index++%sizes.size()],24000-offset);
        juce::AudioBuffer<float> b(2,count);
        fillTone(b,offset,997);p.processBlock(b,midi);
        for(int i=0;i<count;++i)result.push_back(b.getSample(0,i));offset+=count;
    }
    return result;
}

inline void blockInvarianceAndPerformance()
{
    const auto reference=renderBlocks({128});const auto varied=renderBlocks({1,7,31,127,1025,8193});
    float error=0;for(size_t i=0;i<reference.size();++i)error=juce::jmax(error,std::abs(reference[i]-varied[i]));
    require(error<2e-5f,"Audio depends on host block partition");std::cout<<"BLOCK_INVARIANCE max_error="<<error<<"\n";
    juce::MidiBuffer midi;
    for(int blockSize:{32,64,128,512})
    {
        FET67Processor p;p.prepareToPlay(48000,blockSize);juce::AudioBuffer<float> b(2,blockSize);
        double total=0,worst=0;const int blocks=48000*3/blockSize;
        for(int j=0;j<blocks;++j)
        {
            fillTone(b,j*blockSize,997);
            const auto start=std::chrono::steady_clock::now();p.processBlock(b,midi);
            const double t=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();total+=t;worst=juce::jmax(worst,t);
        }
        std::cout<<"PERFORMANCE block="<<blockSize<<" single-core-average-percent="<<100*total/(blocks*blockSize/48000.0)<<" worst_ms="<<worst*1000<<" deadline_ms="<<blockSize/48.0<<"\n";
    }
}

inline void realtimeAllocation()
{
    FET67Processor p;p.prepareToPlay(48000,16);juce::MidiBuffer midi;
    unsigned long total=0;
    for(int count:{0,1,32,511,512,513,32769})
    {
        juce::AudioBuffer<float> b(2,count);fillTone(b,0,997);
        beginAllocationMonitoring();p.processBlock(b,midi);total+=endAllocationMonitoring();
        beginAllocationMonitoring();p.processBlockBypassed(b,midi);total+=endAllocationMonitoring();
    }
    require(total==0,"Audio callback performs C++ heap allocations");
    std::cout<<"REALTIME_CPP_NEW allocations="<<total<<" (native malloc is outside this monitor)\n";
}

inline void silenceFloor()
{
    FET67Processor p;set(p,"input",10);set(p,"output",10);p.prepareToPlay(48000,128);
    juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
    float peak=0;
    for(int block=0;block<375;++block){b.clear();p.processBlock(b,midi);peak=juce::jmax(peak,b.getMagnitude(0,0,128));}
    require(peak==0,"Processor creates noise on digital silence");
    for(int block=0;block<375;++block){fillTone(b,block*128,997);p.processBlock(b,midi);}
    double energy=0;peak=0;
    for(int block=0;block<1875;++block)
    {
        b.clear();p.processBlock(b,midi);
        if(block>=1500)for(int i=0;i<128;++i){const float x=b.getSample(0,i);peak=juce::jmax(peak,std::abs(x));energy+=static_cast<double>(x)*x;}
    }
    const double floor=10*std::log10(juce::jmax(1e-30,energy/48000));
    require(floor<-130,"Signal tail does not settle to low noise floor");
    std::cout<<"SILENCE fresh_peak=0 tail_RMS_dBFS="<<floor<<" tail_peak="<<peak<<" (numerical reporting floor -300 dBFS)\n";
}

inline void fractionalSwitchState()
{
    FET67Processor p;
    for(const char* id:{"power","link","meterMode","bypass","fix","scFilter"})
    {
        auto* param=p.apvts.getParameter(id);param->setValueNotifyingHost(0.550392f);
        require(std::abs(param->getValue()-0.550392f)<1e-6f,"Normalized automation representation changed");
        juce::MemoryBlock state;p.getStateInformation(state);param->setValueNotifyingHost(0.25f);
        p.setStateInformation(state.getData(),static_cast<int>(state.getSize()));
        require(std::abs(param->getValue()-0.550392f)<1e-6f&&p.apvts.getRawParameterValue(id)->load()==1,"Fractional switch state does not round-trip");
    }
    auto legacy=p.apvts.copyState();
    legacy.removeChild(legacy.getChildWithProperty("id","fix"),nullptr);
    legacy.removeChild(legacy.getChildWithProperty("id","scFilter"),nullptr);
    juce::MemoryBlock oldState;
    if(auto xml=legacy.createXml())juce::AudioProcessor::copyXmlToBinary(*xml,oldState);
    p.apvts.getParameter("fix")->setValueNotifyingHost(1);
    p.apvts.getParameter("scFilter")->setValueNotifyingHost(1);
    p.setStateInformation(oldState.getData(),static_cast<int>(oldState.getSize()));
    require(p.apvts.getRawParameterValue("fix")->load()==0,"Legacy session unexpectedly enables FIX");
    require(p.apvts.getRawParameterValue("scFilter")->load()==0,"Legacy session unexpectedly enables SC FILTER");
    std::cout<<"FRACTIONAL_SWITCH_STATE PASS\n";
}

inline void levelMatch()
{
    FET67Processor p,reference;
    for(auto* processor:{&p,&reference}){set(*processor,"input",8);set(*processor,"output",2);processor->prepareToPlay(48000,128);}
    set(p,"fix",1);
    juce::AudioBuffer<float> b(2,128),r(2,128);juce::MidiBuffer midi;
    double energy=0;int offset=0;
    for(int block=0;block<3750;++block)
    {
        fillTone(b,offset,1000,48000,0.1f);r.makeCopyOf(b);p.processBlock(b,midi);reference.processBlock(r,midi);offset+=128;
        if(block>=3375)for(int i=0;i<128;++i)energy+=static_cast<double>(b.getSample(0,i))*b.getSample(0,i);
    }
    const double actual=10*std::log10(energy/48000),expected=20*std::log10(0.1/std::sqrt(2.0));
    require(std::abs(actual-expected)<0.2,"FIX does not match input RMS after settling");
    set(p,"fix",0);float difference=0;
    for(int block=0;block<30;++block)
    {
        fillTone(b,offset,1000,48000,0.1f);r.makeCopyOf(b);p.processBlock(b,midi);reference.processBlock(r,midi);offset+=128;
        if(block>=20)for(int i=0;i<128;++i)difference=juce::jmax(difference,std::abs(b.getSample(0,i)-r.getSample(0,i)));
    }
    require(difference<1e-6f,"FIX changes compressor history or does not disengage cleanly");
    set(p,"fix",1);set(p,"power",0);
    for(int block=0;block<30;++block){fillTone(b,offset,1000,48000,0.1f);p.processBlock(b,midi);offset+=128;}
    require(std::abs(b.getRMSLevel(0,0,128)-0.1f/std::sqrt(2.f))<0.003,"FIX alters the power-off dry signal");
    std::cout<<"FIX matched_RMS="<<actual<<" input_RMS="<<expected<<" disabled_residual="<<difference<<" PASS\n";
}

inline void sidechainFilter()
{
    auto gainReduction=[](bool enabled,double frequency)
    {
        FET67Processor p;set(p,"attack",9);set(p,"scFilter",enabled?1.0f:0.0f);p.prepareToPlay(48000,128);
        juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
        for(int block=0;block<1125;++block){fillTone(b,block*128,frequency);p.processBlock(b,midi);}
        return p.currentGrDb.load();
    };
    const float lowOff=gainReduction(false,40),lowOn=gainReduction(true,40);
    const float midOff=gainReduction(false,1000),midOn=gainReduction(true,1000);
    require(lowOff-lowOn>3,"SC FILTER does not reduce low-frequency triggering");
    require(std::abs(midOff-midOn)<0.5f,"SC FILTER alters midrange detection excessively");
    FET67Processor normal,filtered;set(filtered,"scFilter",1);
    normal.prepareToPlay(48000,128);filtered.prepareToPlay(48000,128);
    juce::AudioBuffer<float> a(2,128),b(2,128);juce::MidiBuffer midi;float error=0;
    for(int block=0;block<100;++block)
    {
        fillTone(a,block*128,40,48000,0.001f);b.makeCopyOf(a);normal.processBlock(a,midi);filtered.processBlock(b,midi);
        for(int i=0;i<128;++i)error=juce::jmax(error,std::abs(a.getSample(0,i)-b.getSample(0,i)));
    }
    require(error==0,"SC FILTER directly changes audio with compression inactive");
    std::cout<<"SC_FILTER 40Hz_GR_off="<<lowOff<<" on="<<lowOn<<" 1kHz_GR_off="<<midOff<<" on="<<midOn<<" inactive_audio_error="<<error<<" PASS\n";
}

inline int run()
{
    bypassAndMeter();latencyAndRobustness();
    require(aliasProbe(48000,10000)<-65,"48kHz alias rejection below release threshold");
    require(aliasProbe(44100,10000)<-65,"44.1kHz alias rejection below release threshold");
    blockInvarianceAndPerformance();realtimeAllocation();silenceFloor();fractionalSwitchState();levelMatch();sidechainFilter();
    FET67Processor colour;set(colour,"input",10);set(colour,"attack",9);spectrum(colour);
    std::cout<<"EXTENDED VALIDATION PASS\n";return 0;
}
}
