#include "PlugManager.h"
#include <iostream>
PlugManager *PlugManager::inst=0;
PlugManager::PlugManager():ptr(0)
{
    inst=this;
    //ctor
}
PlugManager::PlugManager(uint32_t ptr):ptr(ptr)
{
    inst=this;
    //ctor
}
void PlugManager::Install(uint32_t ptr)
{
    uint32_t vpos=peekd(ptr);
    char vv[8];
    peekarb(vpos,vv,8);
    if(vv==std::string(PlugSign))
    {

        this->ptr=ptr;
        return;
    }
    uint32_t pp=Debuger::GetActiveDebuger()->AllocSpace(256);
    strncpy(ph.signature,PlugSign,7);
    ph.signature[7]=0;
    ph.ver=0;
    ph.count=0;
    SetHead();
    poked(ptr,pp);
    pokearb(pp,&ph,sizeof(ph));
    this->ptr=ptr;
}
vPlugins PlugManager::GetList()
{
    vPlugins mlist;
    GetHead();
    uint32_t tpos;
    uint32_t pp=peekd(ptr);
    for(unsigned i=0;i<ph.count;i++)
    {
        PlugEntry ent;
        tpos=pp+sizeof(ph)+sizeof(PlugEntry)*i;
        peekarb(tpos,&ent,sizeof(ent));
        mlist.push_back(ent);
    }
    return mlist;
}
void PlugManager::GetHead()
{
    uint32_t pp=peekd(ptr);
    peekarb(pp,&ph,sizeof(ph));
}
void PlugManager::SetHead()
{
    uint32_t pp=peekd(ptr);
    pokearb(pp,&ph,sizeof(ph));
}
uint32_t PlugManager::AddNewPlug(std::string name,uint32_t size,uint32_t loc)
{
    GetHead();
    if(FindPlugin(name)!=0)
        return -1;
    PlugEntry ent;
    strncpy(ent.name,name.substr(0,31).c_str(),31);
    //ent.name=name.substr(0,31);
    ent.name[31]=0;
    if(size!=0)
        ent.ptr=Debuger::GetActiveDebuger()->AllocSpace(size);
    else
        ent.ptr=loc;
    std::cout<<std::hex;
    std::cout<<ent.ptr<<" allocated\n";
    uint32_t tpos;
    uint32_t pp=peekd(ptr);
    tpos=pp+sizeof(ph)+sizeof(PlugEntry)*ph.count;
    std::cout<<tpos<<" allocated\n";
    std::cout<<"size:"<<sizeof(ph)<<" plugentyrsize:"<<sizeof(PlugEntry)<<" count:"<<ph.count<<"\n";
    pokearb(tpos,&ent,sizeof(ent));
    ph.count++;
    SetHead();
    return ent.ptr;
}
uint32_t PlugManager::FindPlugin(std::string name)
{
    vPlugins vpl=GetList();
    auto it=std::find_if(vpl.begin(),vpl.end(),[&name](PlugEntry &t){return t.name==name;});
    if(it!=vpl.end())
        return it->ptr;
    return 0;
}
PlugManager::~PlugManager()
{
    //dtor
}







