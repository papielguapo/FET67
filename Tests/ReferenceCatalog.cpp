#include <windows.h>
#include <pluginterfaces/base/ipluginbase.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
int main(int argc,char** argv)
{
    if(argc<2)return 1;
    auto module=LoadLibraryA(argv[1]);if(!module){std::cerr<<GetLastError();return 2;}
    auto init=reinterpret_cast<bool(*)()>(GetProcAddress(module,"InitDll"));if(init)init();
    auto get=reinterpret_cast<Steinberg::IPluginFactory*(*)()>(GetProcAddress(module,"GetPluginFactory"));if(!get)return 3;
    auto* factory=get();if(!factory)return 4;
    std::cout<<"Classes "<<factory->countClasses()<<std::endl;
    for(int i=0;i<factory->countClasses();++i)
    {
        Steinberg::PClassInfo info{};factory->getClassInfo(i,&info);
        if(!std::strstr(info.name,"CLA-76"))continue;
        std::uint32_t hash=0;for(auto c:info.cid)hash=hash*31+static_cast<std::uint32_t>(c);
        std::cout<<info.name<<" | "<<info.category<<" | "<<std::hex<<hash<<std::dec<<std::endl;
        if(argc>2&&std::strstr(info.category,"Audio Module"))
        {
            std::ofstream out(std::string(argv[2])+"/"+info.name+".xml");
            out<<"<PLUGIN name=\""<<info.name<<"\" descriptiveName=\""<<info.name<<"\" format=\"VST3\" category=\"Fx\" manufacturer=\"Waves\" version=\"14.12\" file=\""<<argv[1]<<"\" uniqueId=\"0\" uid=\""<<std::hex<<hash<<"\" isInstrument=\"0\" numInputs=\"2\" numOutputs=\"2\"/>";
        }
    }
    factory->release();
    return 0;
}
