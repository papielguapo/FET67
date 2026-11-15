#include "PluginEditor.h"
#include <cmath>

namespace
{
juce::Colour colour(juce::uint32 hex) { return juce::Colour(hex); }
juce::Font font(const juce::String& name,float size,int style)
{
#if JUCE_MAJOR_VERSION >= 8
    return juce::Font(juce::FontOptions(name,size,style));
#else
    return juce::Font(name,size,style);
#endif
}
void text(juce::Graphics& g, const juce::String& s, float x, float y, float w, float h, float size = 10, bool bold = true)
{
    g.setFont(font("Arial", size, bold ? juce::Font::bold : juce::Font::plain));
    g.drawText(s, juce::Rectangle<float>(x,y,w,h), juce::Justification::centred, false);
}
void screw(juce::Graphics& g, float x, float y)
{
    g.setColour(juce::Colours::black.withAlpha(0.3f)); g.fillEllipse(x-6,y-5,14,14);
    g.setGradientFill(juce::ColourGradient(colour(0xff73777a),x-5,y-6,colour(0xff111315),x+5,y+6,false));
    g.fillEllipse(x-6,y-6,12,12);
    g.setColour(colour(0xff151719)); g.fillEllipse(x-4.8f,y-4.8f,9.6f,9.6f);
    juce::Path hex;
    for (int i=0;i<6;++i) { const float a=static_cast<float>(i)*juce::MathConstants<float>::twoPi/6; const float px=x+2.9f*std::cos(a),py=y+2.9f*std::sin(a); if(i==0)hex.startNewSubPath(px,py);else hex.lineTo(px,py); }
    hex.closeSubPath(); g.setColour(juce::Colours::black);g.fillPath(hex);
    g.setColour(juce::Colours::white.withAlpha(0.2f));g.strokePath(hex,juce::PathStrokeType(0.5f));
}
void lamp(juce::Graphics& g, juce::Rectangle<float> r, bool on)
{
    const auto c=r.getCentre();
    if(on) { g.setGradientFill(juce::ColourGradient(colour(0xff9b26ff).withAlpha(0.35f),c.x,c.y,juce::Colours::transparentBlack,c.x+r.getWidth()*0.85f,c.y,true));g.fillEllipse(r.expanded(r.getWidth()*0.4f)); }
    g.setGradientFill(juce::ColourGradient(colour(0xff77757d),r.getX(),r.getY(),colour(0xff111217),r.getRight(),r.getBottom(),false));g.fillEllipse(r);
    auto inner=r.reduced(2);
    if(on) { g.setGradientFill(juce::ColourGradient(colour(0xfff0ccff),c.x-3,c.y-4,colour(0xff3c0078),c.x+11,c.y+6,true));g.fillEllipse(inner);g.setColour(colour(0xff9b26ff).withAlpha(0.4f));g.drawEllipse(inner.reduced(2),2); }
    else { g.setColour(colour(0xff240046));g.fillEllipse(inner); }
    g.setColour(juce::Colours::white.withAlpha(on?0.65f:0.12f));g.fillEllipse(r.getX()+6,r.getY()+4,r.getWidth()*0.35f,r.getHeight()*0.18f);
}
}

void FET67LookAndFeel::drawRotarySlider(juce::Graphics& g,int x,int y,int width,int height,float pos,float start,float end,juce::Slider&)
{
    const float radius=static_cast<float>(juce::jmin(width,height))*0.46f;
    const float cx=static_cast<float>(x)+static_cast<float>(width)*0.5f,cy=static_cast<float>(y)+static_cast<float>(height)*0.5f;
    g.setColour(juce::Colours::black.withAlpha(0.45f));g.fillEllipse(cx-radius+2,cy-radius+4,radius*2,radius*2);
    g.setGradientFill(juce::ColourGradient(colour(0xff8a1cf6),cx-radius*0.6f,cy-radius*0.7f,colour(0xff240046),cx+radius,cy+radius,true));g.fillEllipse(cx-radius,cy-radius,radius*2,radius*2);
    g.setColour(colour(0xffd48fff).withAlpha(0.6f));g.drawEllipse(cx-radius+0.8f,cy-radius+0.8f,radius*2-1.6f,radius*2-1.6f,0.8f);
    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::rotation(start+pos*(end-start)).translated(cx,cy));
    juce::Path flutes;
    flutes.startNewSubPath(-radius*0.18f,-radius*0.72f);
    flutes.cubicTo(-radius*0.8f,-radius*0.9f,-radius*0.96f,radius*0.28f,-radius*0.24f,radius*0.65f);
    flutes.cubicTo(-radius*0.56f,radius*0.1f,-radius*0.42f,-radius*0.35f,-radius*0.18f,-radius*0.72f);
    flutes.closeSubPath();g.setColour(colour(0xff2a0052));g.fillPath(flutes);g.fillPath(flutes,juce::AffineTransform::scale(-1,1));
    juce::Path wing;
    wing.startNewSubPath(0,-radius*0.90f);
    wing.cubicTo(radius*0.16f,-radius*0.90f,radius*0.18f,-radius*0.5f,radius*0.22f,-radius*0.08f);
    wing.cubicTo(radius*0.30f,radius*0.45f,radius*0.22f,radius*0.82f,0,radius*0.82f);
    wing.cubicTo(-radius*0.22f,radius*0.82f,-radius*0.30f,radius*0.45f,-radius*0.22f,-radius*0.08f);
    wing.cubicTo(-radius*0.18f,-radius*0.5f,-radius*0.16f,-radius*0.90f,0,-radius*0.90f);wing.closeSubPath();
    g.setGradientFill(juce::ColourGradient(colour(0xffc470ff),-radius*0.23f,0,colour(0xff38006b),radius*0.18f,0,false));g.fillPath(wing);
    g.setColour(colour(0xffc470ff).withAlpha(0.65f));g.strokePath(wing,juce::PathStrokeType(0.65f));
    g.setColour(juce::Colours::white);g.fillRect(-1.4f,-radius*0.92f,2.8f,radius*0.42f);
}
void FET67LookAndFeel::drawToggleButton(juce::Graphics& g,juce::ToggleButton& b,bool hover,bool down)
{
    auto r=b.getLocalBounds().toFloat().reduced(2,4);const auto c=r.getCentre();
    g.setColour(juce::Colours::black.withAlpha(0.3f));g.fillRoundedRectangle(r.translated(1,2),r.getHeight()*0.5f);
    g.setGradientFill(juce::ColourGradient(colour(0xff65696e),r.getX(),r.getY(),colour(0xff151719),r.getX(),r.getBottom(),false));g.fillRoundedRectangle(r,r.getHeight()*0.5f);
    g.setColour(colour(0xff060708));g.fillRoundedRectangle(r.reduced(3),r.getHeight()*0.4f);
    const bool right=b.getToggleState() != static_cast<bool>(b.getProperties().getWithDefault("reverse",false));
    const float tip=c.x+(right?10.0f:-10.0f);
    g.setGradientFill(juce::ColourGradient(colour(0xfff6f7fa),c.x,c.y-4,colour(0xff686d74),c.x,c.y+5,false));
    g.fillRoundedRectangle(juce::jmin(c.x,tip)-3,c.y-3,std::abs(tip-c.x)+6,6,3);
    g.setGradientFill(juce::ColourGradient(colour(0xfff0f1f4),tip-2,c.y-5,colour(0xff6a7078),tip+3,c.y+5,false));g.fillEllipse(tip-4,c.y-5,8,10);
    if(hover||down||b.hasKeyboardFocus(true)) {g.setColour(colour(0xff9b26ff).withAlpha(0.65f));g.drawRoundedRectangle(r.expanded(1),7,1);}
}
FaceplateComponent::FaceplateComponent(FET67Processor& p):processor(p)
{
    setLookAndFeel(&look);
    const char* ids[]{"input","output","attack","release","ratio"};
    const char* names[]{"Input","Output","Attack","Release","Ratio"};
    for(size_t i=0;i<knobs.size();++i)
    {
        auto& s=knobs[i];addAndMakeVisible(s);s.setName(names[i]);s.setTitle(names[i]);
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);
        s.setRotaryParameters(juce::MathConstants<float>::pi*7/6,juce::MathConstants<float>::pi*17/6,true);
        s.setPopupDisplayEnabled(true,true,this);s.setMouseDragSensitivity(180);s.setScrollWheelEnabled(true);
        s.setDoubleClickReturnValue(true,i==0?4.5:i==1?5.0:i==2?3.0:i==3?4.0:0.0);
        s.textFromValueFunction=[i](double v)
        {
            if(i==4) {const char* labels[]{"4:1","8:1","12:1","20:1","ALL"};return juce::String(labels[juce::jlimit(0,4,juce::roundToInt(v))]);}
            if(i==2)return juce::String(20*std::pow(40.0,(v-1)/8),1)+" us";
            if(i==3)return juce::String(50*std::pow(22.0,(v-1)/8),1)+" ms";
            if(i==0&&v<=0)return juce::String("OFF");
            return juce::String(v,2);
        };
        knobAttachments[i]=std::make_unique<SA>(processor.apvts,ids[i],s);
    }
    const char* switchIds[]{"power","link","meterMode","bypass"};
    const char* tips[]{"Power: on / off (dry passthrough)","Link: stereo linked / dual mono","Meter: gain reduction / output (0 = -18 dBFS RMS)","Bypass: compressor in / dry out"};
    for(size_t i=0;i<switches.size();++i) {auto& b=switches[i];addAndMakeVisible(b);b.setName(switchIds[i]);b.setTitle(tips[i]);b.setTooltip(tips[i]);b.setClickingTogglesState(true);if(i<2)b.getProperties().set("reverse",true);switchAttachments[i]=std::make_unique<BA>(processor.apvts,switchIds[i],b);}
    startTimerHz(60);
}
FaceplateComponent::~FaceplateComponent() {stopTimer();setLookAndFeel(nullptr);}
void FaceplateComponent::resized()
{
    for(int i=0;i<5;++i)knobs[static_cast<size_t>(i)].setBounds(198+97*i,49,54,54);
    switches[0].setBounds(131,84,40,24);switches[1].setBounds(710,86,40,24);
    switches[2].setBounds(784,86,40,24);switches[3].setBounds(858,86,40,24);
}
void FaceplateComponent::timerCallback()
{
    shownGr=processor.currentGrDb.load(std::memory_order_relaxed);
    shownOut=processor.currentOutDb.load(std::memory_order_relaxed);
    repaint();
}
void FaceplateComponent::paintMeter(juce::Graphics& g)
{
    const juce::Rectangle<float> r(670,34,255,38);
    g.setColour(colour(0xff747a80));g.fillRoundedRectangle(r.expanded(1),3);
    g.setGradientFill(juce::ColourGradient(colour(0xff030405),670,34,colour(0xff27272a),670,72,false));g.fillRoundedRectangle(r,2);
    constexpr float values[]{-21,-18,-16,-14,-12,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,-0.5f,0,0.5f,1,2,3,4,6};
    const char* labels[]{"-21","-18","-16","-14","-12","-10","-9","-8","-7","-6","-5","-4","-3","-2","-1","-.5","0",".5","1","2","3","4","CLIP"};
    const bool outputMode=processor.apvts.getRawParameterValue("meterMode")->load()>0.5f;
    const bool powered=processor.apvts.getRawParameterValue("power")->load()>0.5f;
    for(int i=0;i<23;++i)
    {
        const float x=675+static_cast<float>(i)*10.6f+(i==22?2.0f:0.0f);
        const bool on=powered&&(outputMode ? shownOut+18>=values[i] : i<=16&&(i==16||shownGr>=-values[i]));
        if(on) {g.setColour(colour(0xffff2a2a).withAlpha(0.17f));g.fillRoundedRectangle(x-1,46,10,15,2);}
        g.setColour(colour(on?0xffff2a2a:0xff4a1818));g.fillRect(x,49.0f,7.0f,9.0f);
        if(on){g.setColour(colour(0xffffb1a1).withAlpha(0.55f));g.fillRect(x+1,49.5f,5.0f,1.0f);}
        g.setColour(juce::Colours::white.withAlpha(0.92f));text(g,labels[i],x-2,36,i==22?16.0f:11.0f,10, i==22?5.7f:6.7f,false);text(g,labels[i],x-2,60,i==22?16.0f:11.0f,10,i==22?5.7f:6.7f,false);
    }
    g.setColour(juce::Colours::white.withAlpha(0.06f));g.fillRect(672.0f,35.0f,251.0f,7.0f);
}
void FaceplateComponent::paint(juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(colour(0xffd9dde0),0,0,colour(0xffbcc1c6),0,156,false));g.fillAll();
    for(int y=0;y<156;y+=2){g.setColour(juce::Colours::white.withAlpha(0.08f));g.fillRect(0,y,1140,1);g.setColour(juce::Colours::black.withAlpha(0.03f));g.fillRect(0,y+1,1140,1);}
    g.setColour(juce::Colours::white.withAlpha(0.65f));g.drawHorizontalLine(0,0,1140);g.setColour(colour(0xff777c81));g.drawHorizontalLine(155,0,1140);
    for(int x:{6,1116}) {g.setGradientFill(juce::ColourGradient(colour(0xff363a3d),static_cast<float>(x),0,colour(0xff111315),static_cast<float>(x+18),0,false));g.fillRoundedRectangle(static_cast<float>(x),12,18,132,3);for(int y:{32,112}){g.setColour(colour(0xff858a8f));g.drawRoundedRectangle(static_cast<float>(x+4),static_cast<float>(y),10,17,5,1);g.setColour(colour(0xff050607));g.fillRoundedRectangle(static_cast<float>(x+4),static_cast<float>(y),10,17,5);}}
    for(auto pt:{juce::Point<float>(104,34),{104,122},{945,34},{945,122}})screw(g,pt.x,pt.y);
    g.setColour(colour(0xff141618));g.setFont(font("Arial Black",15.5f,juce::Font::bold));g.drawText("FET 67",26,68,76,22,juce::Justification::centred);
    g.setColour(colour(0xffcbd0d4));g.fillRect(27.0f,78.0f,75.0f,1.6f);
    lamp(g,{138,34,26,26},processor.apvts.getRawParameterValue("power")->load()>0.5f);
    g.setColour(colour(0xff141618));text(g,"POWER",126,66,50,16,10);text(g,"ON        OFF",117,111,68,12,8);
    const char* titles[]{"INPUT","OUTPUT","ATTACK","RELEASE","RATIO"};
    const char* left[]{"OFF","MIN","FAST","FAST","GENTLE"};const char* right[]{"10","MAX","SLOW","SLOW","SLAM"};
    for(int i=0;i<5;++i)
    {
        const float cx=225+97.0f*static_cast<float>(i);
        const int steps=i==4?4:i<2?10:8;
        for(int tick=0;tick<=steps;++tick)
        {
            const float a=juce::MathConstants<float>::pi*(7.0f/6.0f+(5.0f/3.0f)*static_cast<float>(tick)/static_cast<float>(steps));
            const auto point=[cx,a](float r){return juce::Point<float>(cx+std::sin(a)*r,76-std::cos(a)*r);};
            auto p1=point(30),p2=point(33);g.setColour(colour(0xff34383b));g.drawLine({p1,p2},0.7f);
            auto p=point(40);juce::String label;
            if(i==4){const char* l[]{"4:1","8:1","12:1","20:1","ALL"};label=l[tick];}
            else label=juce::String(i<2?tick:tick+1);
            text(g,label,p.x-12,p.y-5,24,10,i==4?7.0f:7.5f,false);
        }
        g.setColour(colour(0xff141618));text(g,left[i],cx-44,111,42,12,7.5f);text(g,right[i],cx+3,111,42,12,7.5f);text(g,titles[i],cx-42,132,84,14,11);
    }
    paintMeter(g);
    g.setColour(colour(0xff141618));text(g,"ON        OFF",700,77,60,9,7);text(g,"GR       OUT",774,77,60,9,7);text(g,"IN       OUT",848,77,60,9,7);
    text(g,"LINK",705,114,50,14,9);text(g,"METER",779,114,50,14,9);text(g,"BYPASS",853,114,50,14,9);
    const bool linked=processor.apvts.getRawParameterValue("link")->load()>0.5f;
    if(linked){g.setColour(colour(0xffb84dff).withAlpha(0.18f));g.fillEllipse(691,91,13,13);}
    g.setColour(colour(linked?0xffb84dff:0xff382442));g.fillEllipse(695,95,5,5);
    {
        juce::Graphics::ScopedSaveState save(g);
        g.addTransform(juce::AffineTransform(1,-0.19438f,15,0,1,0));
        g.setFont(font("Segoe Script",22,juce::Font::bold));
        g.setColour(juce::Colours::white.withAlpha(0.28f));g.drawText("Papielguapo",966,53,145,45,juce::Justification::centred);
        g.setColour(colour(0xff111315));g.drawText("Papielguapo",965,52,145,45,juce::Justification::centred);
    }
    juce::Path flourish;flourish.startNewSubPath(982,89);flourish.quadraticTo(1044,88,1102,81);flourish.lineTo(1050,91);
    g.setColour(juce::Colours::white.withAlpha(0.28f));g.strokePath(flourish,juce::PathStrokeType(1.5f),juce::AffineTransform::translation(1,1));g.setColour(colour(0xff111315));g.strokePath(flourish,juce::PathStrokeType(1.5f));
}
FET67Editor::FET67Editor(FET67Processor& p):AudioProcessorEditor(p),processor(p),faceplate(p)
{
    addAndMakeVisible(faceplate);setResizable(true,true);setResizeLimits(570,78,2850,390);
    getConstrainer()->setFixedAspectRatio(static_cast<double>(kBaseWidth)/kBaseHeight);
    const int w=juce::jlimit(570,2850,processor.editorWidth.load());setSize(w,juce::roundToInt(static_cast<double>(w)*kBaseHeight/kBaseWidth));
}
void FET67Editor::resized()
{
    const float sx=static_cast<float>(getWidth())/kBaseWidth,sy=static_cast<float>(getHeight())/kBaseHeight;
    faceplate.setBounds(0,0,kBaseWidth,kBaseHeight);faceplate.setTransform(juce::AffineTransform::scale(juce::jmin(sx,sy)));
    processor.editorWidth.store(getWidth());processor.editorHeight.store(getHeight());
}
