#include "../Source/PluginProcessor.h"
#include "../Source/PluginEditor.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdexcept>

namespace
{
void set(juce::AudioProcessor& p, const juce::String& name, float value)
{
    auto& f=dynamic_cast<FET67Processor&>(p);
    auto* parameter=f.apvts.getParameter(name);
    parameter->setValueNotifyingHost(parameter->convertTo0to1(value));
}
void require(bool ok,const char* message) {if(!ok)throw std::runtime_error(message);}
double sine(juce::AudioProcessor& p,float level,int seconds=3,double frequency=1000)
{
    juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
    double energy=0;int count=0;const int frames=48000*seconds;
    for(int offset=0;offset<frames;offset+=128)
    {
        for(int i=0;i<128;++i) {float x=level*std::sin(juce::MathConstants<double>::twoPi*frequency*(offset+i)/48000.0);b.setSample(0,i,x);b.setSample(1,i,x);}
        p.processBlock(b,midi);
        for(int i=0;i<128;++i){const auto x=b.getSample(0,i);require(std::isfinite(x),"Non-finite output");if(offset>frames-48000){energy+=x*x;++count;}}
    }
    return 10*std::log10(juce::jmax(1e-20,energy/juce::jmax(1,count)));
}
void measure(juce::AudioProcessor& p)
{
    p.setRateAndBufferSizeDetails(48000,128);p.prepareToPlay(48000,128);
    std::cout<<"TRANSFER,input_dBFS_peak,output_dBFS_RMS\n";
    for(float level:{-48.f,-36.f,-30.f,-24.f,-18.f,-12.f,-6.f,0.f}){p.reset();std::cout<<"TRANSFER,"<<level<<","<<sine(p,std::pow(10.f,level/20.f))<<"\n";}
    std::cout<<"FREQUENCY,Hz,output_dBFS_RMS,input_dBFS_peak=-48\n";
    for(double f:{30.0,80.0,1000.0,10000.0,18000.0}){p.reset();std::cout<<"FREQUENCY,"<<f<<","<<sine(p,std::pow(10.f,-48.f/20.f),3,f)<<"\n";}
    p.reset();juce::AudioBuffer<float> b(2,48);juce::MidiBuffer midi;
    std::cout<<"STEP,time_ms,input_dBFS_RMS,output_dBFS_RMS\n";
    for(int ms=0;ms<2000;++ms)
    {
        const float a=std::pow(10.f,(ms>=100&&ms<600?-6.f:-36.f)/20.f);
        for(int i=0;i<48;++i){float x=a*std::sin(juce::MathConstants<double>::twoPi*1000*(ms*48+i)/48000.0);b.setSample(0,i,x);b.setSample(1,i,x);}
        p.processBlock(b,midi);std::cout<<"STEP,"<<ms<<","<<20*std::log10(a/std::sqrt(2.f))<<","<<20*std::log10(juce::jmax(1e-10f,b.getRMSLevel(0,0,48)))<<"\n";
    }
    p.releaseResources();
}
void dumpParameters(juce::AudioProcessor& p)
{
    int i=0;for(auto* parameter:p.getParameters()) {std::cout<<"PARAM "<<i++<<" | "<<parameter->getName(100)<<" | "<<parameter->getValue()<<" | "<<parameter->getText(parameter->getValue(),100).toStdString()<<"\n";
        if(parameter->getNumSteps()<100) for(int step=0;step<parameter->getNumSteps();++step) {float v=static_cast<float>(step)/static_cast<float>(juce::jmax(1,parameter->getNumSteps()-1));std::cout<<"  "<<v<<" = "<<parameter->getText(v,100).toStdString()<<"\n";}}
}
}
#include "Validation.h"
#include "OfflineRender.h"
#include "Response.h"
int main(int argc,char** argv)
{
    juce::ScopedJuceInitialiser_GUI init;
    try
    {
        if(argc>1&&juce::String(argv[1])=="--validate")return validation::run();
        if(argc>2&&juce::String(argv[1])=="--scan")
        {
            juce::VST3PluginFormat format;juce::OwnedArray<juce::PluginDescription> types;
            format.findAllTypesForFile(types,argv[2]);
            require(!types.isEmpty(),"No VST3 plugins found at requested path");
            for(auto* d:types) {std::cout<<d->name<<" | "<<d->uniqueId<<"\n";if(d->name.containsIgnoreCase("CLA-76")||argc>3){std::cout<<d->createXml()->toString().toStdString()<<"\n";if(argc>3){const auto file=juce::File(juce::String(argv[3])).getChildFile(juce::File::createLegalFileName(d->name)+".xml");d->createXml()->writeTo(file);}}}
            return 0;
        }
        if(argc>2&&juce::String(argv[1])=="--reference")
        {
            auto xml=juce::XmlDocument::parse(juce::File(juce::String(argv[2])));require(xml!=nullptr,"Cannot read plugin description");
            juce::PluginDescription d;require(d.loadFromXml(*xml),"Invalid description");
            juce::VST3PluginFormat format;std::unique_ptr<juce::AudioPluginInstance> instance;juce::String error;bool done=false;
            static_cast<juce::AudioPluginFormat&>(format).createPluginInstanceAsync(d,48000,128,[&](std::unique_ptr<juce::AudioPluginInstance> p,const juce::String& e){instance=std::move(p);error=e;done=true;});
            const auto start=juce::Time::getMillisecondCounter();
            while(!done&&juce::Time::getMillisecondCounter()-start<30000)juce::MessageManager::getInstance()->runDispatchLoopUntil(20);
            require(instance!=nullptr,error.isEmpty()?"Reference load timeout":error.toRawUTF8());
            dumpParameters(*instance);
            if(argc>3&&(juce::String(argv[3])=="--response"||juce::String(argv[3])=="--calibrate"))
            {
                float level=1e-5f;
                for(int a=4;a<argc;++a){auto pair=juce::StringArray::fromTokens(argv[a],"=","");if(pair.size()==2){if(pair[0]=="level")level=pair[1].getFloatValue();else{int index=pair[0].getIntValue();if(juce::isPositiveAndBelow(index,instance->getParameters().size()))instance->getParameters()[index]->setValueNotifyingHost(pair[1].getFloatValue());}}}
                require(level>0&&level<=1,"Invalid response level");return juce::String(argv[3])=="--calibrate"?calibrateInput(*instance):settledResponse(*instance,level);
            }
            if(argc>3&&juce::String(argv[3])=="--external-key-test")
            {validation::hostedExternalSidechain(*instance);return 0;}
            if(argc>3&&(juce::String(argv[3])=="--measure"||juce::String(argv[3])=="--spectrum"||juce::String(argv[3])=="--render-audio"))
            {
                // Further arguments are index=normalisedValue, taken from the dumped public parameter metadata.
                const bool render=juce::String(argv[3])=="--render-audio";
                require(!render||argc>5,"Audio rendering requires input and output paths");
                for(int a=render?6:4;a<argc;++a){auto pair=juce::StringArray::fromTokens(argv[a],"=","");if(pair.size()==2){int index=pair[0].getIntValue();if(juce::isPositiveAndBelow(index,instance->getParameters().size()))instance->getParameters()[index]->setValueNotifyingHost(pair[1].getFloatValue());}}
                std::cout<<"APPLIED PARAMETERS\n";dumpParameters(*instance);
                if(render)return renderAudioFile(*instance,juce::File(juce::String(argv[4])),juce::File(juce::String(argv[5])));
                if(juce::String(argv[3])=="--spectrum")validation::spectrum(*instance);else measure(*instance);
            }
            return 0;
        }
        FET67Processor p;
        if(argc>1&&juce::String(argv[1])=="--calibrate-fet")return calibrateInput(p);
        if(argc>1&&juce::String(argv[1])=="--response-fet")
        {
            float level=1e-5f;
            for(int a=2;a<argc;++a){auto pair=juce::StringArray::fromTokens(argv[a],"=","");if(pair.size()==2){if(pair[0]=="level")level=pair[1].getFloatValue();else if(p.apvts.getParameter(pair[0])!=nullptr)set(p,pair[0],pair[1].getFloatValue());}}
            require(level>0&&level<=1,"Invalid response level");return settledResponse(p,level);
        }
        if(argc>3&&juce::String(argv[1])=="--render-fet")
        {
            for(int a=4;a<argc;++a)if(juce::String(argv[a])=="externalBus=1")
            {auto layout=p.getBusesLayout();layout.inputBuses.set(1,juce::AudioChannelSet::stereo());require(p.setBusesLayout(layout),"Cannot enable external bus");}
            for(int a=4;a<argc;++a){auto pair=juce::StringArray::fromTokens(argv[a],"=","");if(pair.size()==2&&p.apvts.getParameter(pair[0])!=nullptr)set(p,pair[0],pair[1].getFloatValue());}
            return renderAudioFile(p,juce::File(juce::String(argv[2])),juce::File(juce::String(argv[3])));
        }
        if(argc>1&&juce::String(argv[1])=="--fetmeasure")
        {
            for(int a=2;a<argc;++a){auto pair=juce::StringArray::fromTokens(argv[a],"=","");if(pair.size()==2&&p.apvts.getParameter(pair[0])!=nullptr)set(p,pair[0],pair[1].getFloatValue());}
            dumpParameters(p);measure(p);return 0;
        }
        p.prepareToPlay(48000,128);
        juce::AudioBuffer<float> b(2,128);juce::MidiBuffer midi;
        set(p,"bypass",1);p.reset();
        for(int i=0;i<128;++i){b.setSample(0,i,0.2f);b.setSample(1,i,-0.3f);}p.processBlock(b,midi);
        for(int block=0;block<20;++block){for(int i=0;i<128;++i){b.setSample(0,i,0.2f);b.setSample(1,i,-0.3f);}p.processBlock(b,midi);}
        require(b.getSample(0,50)==0.2f&&b.getSample(1,50)==-0.3f,"Latency-aligned bypass not transparent");
        set(p,"bypass",0);set(p,"input",0);p.reset();sine(p,0.5f);
        require(p.currentOutDb.load()<-90,"Input OFF leaks signal");
        set(p,"input",4.5f);p.reset();
        for(int r=0;r<5;++r){set(p,"ratio",static_cast<float>(r));p.reset();const auto low=sine(p,0.001f),high=sine(p,0.8f);require(high>low&&p.currentGrDb.load()>1,"Compression response invalid");std::cout<<"Ratio "<<r<<" low "<<low<<" high "<<high<<" GR "<<p.currentGrDb.load()<<"\n";}
        juce::MemoryBlock state;p.editorWidth.store(1710);p.getStateInformation(state);FET67Processor restored;restored.setStateInformation(state.getData(),static_cast<int>(state.getSize()));require(restored.editorWidth.load()==1710,"Size state did not round-trip");require(restored.apvts.getRawParameterValue("ratio")->load()==4,"Parameter state did not round-trip");
        // Link must preserve equal attenuation when only the left channel drives the detector.
        set(p,"link",1);p.reset();
        for(int block=0;block<2000;++block){for(int i=0;i<128;++i){const float s=std::sin(juce::MathConstants<double>::twoPi*1000*(block*128+i)/48000.0);b.setSample(0,i,s*0.5f);b.setSample(1,i,s*0.005f);}p.processBlock(b,midi);}
        const float linkedRight=b.getRMSLevel(1,0,128);
        set(p,"link",0);p.reset();
        for(int block=0;block<2000;++block){for(int i=0;i<128;++i){const float s=std::sin(juce::MathConstants<double>::twoPi*1000*(block*128+i)/48000.0);b.setSample(0,i,s*0.5f);b.setSample(1,i,s*0.005f);}p.processBlock(b,midi);}
        require(b.getRMSLevel(1,0,128)>linkedRight*2,"Dual mono did not release quiet channel independently");
        for(double sr:{44100.0,96000.0,192000.0})
        {
            FET67Processor mono;juce::AudioProcessor::BusesLayout layout;layout.inputBuses.add(juce::AudioChannelSet::mono());layout.inputBuses.add(juce::AudioChannelSet::disabled());layout.outputBuses.add(juce::AudioChannelSet::mono());
            require(mono.setBusesLayout(layout),"Mono layout rejected");mono.prepareToPlay(sr,511);set(mono,"input",10);set(mono,"output",10);mono.reset();
            juce::AudioBuffer<float> m(1,511);
            for(int block=0;block<50;++block){for(int i=0;i<511;++i)m.setSample(0,i,std::sin(static_cast<float>(block*511+i)*0.17f));mono.processBlock(m,midi);for(int i=0;i<511;++i)require(std::isfinite(m.getSample(0,i))&&std::abs(m.getSample(0,i))<2,"Extreme drive unstable");}
        }
        if(argc>2&&juce::String(argv[1])=="--render")
        {
            FET67Processor previewProcessor;
            auto editor=std::unique_ptr<juce::AudioProcessorEditor>(previewProcessor.createEditor());
            const int renderWidth=argc>3?juce::String(argv[3]).getIntValue():kBaseWidth;
            editor->setSize(renderWidth,juce::roundToInt(static_cast<double>(renderWidth)*kBaseHeight/kBaseWidth));
            const auto image=editor->createComponentSnapshot(editor->getLocalBounds());
            juce::FileOutputStream stream{juce::File(juce::String(argv[2]))};
            require(stream.openedOk(),"Cannot open screenshot output");stream.setPosition(0);stream.truncate();
            juce::PNGImageFormat png;require(png.writeImageToStream(image,stream),"Screenshot write failed");
        }
        std::cout<<"PASS: bypass, input mute, all ratios, finite output, stereo link / dual mono, state persistence, mono 44.1/96/192 kHz extreme drive\n";
    }
    catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}
    return 0;
}
