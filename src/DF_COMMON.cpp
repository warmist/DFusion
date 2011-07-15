#include "DF_COMMON.h"

void ScanVectors(size_t start,size_t end)
{
    //size_t start=0x01A00000;
    //size_t end=  0x01B00000;
    DF_Vector<size_t> vec;
    for (unsigned long off=start;off<end;off++)
    {
        //fstream ffile("data.txt",fstream::out|fstream::in|fstream::trunc);
        //ffile<<"START!"<<endl;
        peekarb(off,&vec,sizeof(vec));
        if (vec.IsValid())
            if (vec.Size_Abs()<100000)
                cout<<hex<<off<<" Size:"<<dec<<vec.Size()<<endl;
    }
}

void Messages::Update()
{
    //size_t pos=peekd(MESSAGE_VEC);
    //peekarb(MESSAGES_VEC+df_memory_base,&pointers,sizeof(pointers));
}
size_t Messages::Size()
{
    return pointers.Size();
}
string Messages::GetString(size_t s)
{
    string ret;
    size_t pos;
    pointers.Load(s,&pos);

    byte b=-1;
    b=peekb(pos);
    do
    {

        ret+=b;
        pos++;
        b=peekb(pos);
    }while(b!=0);
    return ret;
}
void Messages::OverwriteString(size_t n,string s)
{
    string r=GetString(n);
    if(r.size()<s.size())return ;
    size_t pos;
    pointers.Load(n,&pos);

    for(size_t i=0;i<r.size();i++)
    {
        if(i<s.size())
        pokeb(pos,s[i]);
        else
        pokeb(pos,' ');
        pos++;
    }
}

string DF_info::InfoType()
{

    switch(vtable-df_memory_base)
    {
        case 0x532774:
            return "Contained";
        case 0x5327a0:
            return "IsShot?";//catapult research.
        case 0x532684:
            return "HeldBy";
        case 0x532300:
            return "Artifact?"; //or legends or sth
        case 0x532990:
            return "StolenFrom?";// or civ index?
        case 0x532918:
            return "Building";
        case 0x5326C0:
            return "Creature";
        case 0x532468:
            return "ChainTrg";
        case 0x53242c:
            return "SparPartner";
        case 0x532738:
            return "Contains";
        case 0x53260c:
            return "Owner";
        default:
            return "";
    }
}

size_t NormalizePtr(size_t ptr)
{
    return ptr-df_memory_base;
}
InfoManager::InfoManager(DF_Vector <uint32_t> infovec):infovec(infovec)
{
}
 uint32_t InfoManager::FindOfType(string type)
 {
     uint32_t pos;
     for(size_t i=0;i<infovec.Size();i++)
     {
         infovec.Load(i,&pos);
         DF_info inf;
         peekarb(pos,&inf,sizeof(inf));
         if(inf.InfoType()==type)
         {
             return pos;
         }
     }
     return 0;
 }
 void InfoManager::Dump()
 {
     uint32_t pos;
      for(size_t i=0;i<infovec.Size();i++)
     {
         infovec.Load(i,&pos);
          DF_info inf;
         peekarb(pos,&inf,sizeof(inf));
         if(inf.InfoType()!="")
         cout<<inf.InfoType()<<":"<<inf.ID<<endl;
         else
         cout<<hex<<NormalizePtr(inf.vtable)<<dec<<":"<<inf.ID<<endl;
     }

 }

double Dist(double x,double y,double x2,double y2)
{
    return pow(pow(x2-x,2)+pow(y2-y,2),0.5);
}
double Dist(double x,double y,double z,double x2,double y2,double z2)
{
    return pow(pow(x2-x,2)+pow(y2-y,2)+pow(z2-z,2),0.5);
}


iDF_Offset::iDF_Offset():offset(0)
{
    //DF_Offsets::Get().Register(this); //pure virtual call :(
}
DF_Offsets *DF_Offsets::me=0;
DF_Offsets &DF_Offsets::Get()
{
    if(DF_Offsets::me==0)
        DF_Offsets::me =new DF_Offsets();
    return *me;
}
void DF_Offsets::Register(iDF_Offset *t)
{
    DF_Offset_static *p=dynamic_cast<DF_Offset_static*>(t);
    if(p!=0)
        updated=true;
    myvec[t->GetName()]=t;

}
void DF_Offsets::Print()
{
    for(vOff::iterator it=myvec.begin();it!=myvec.end();it++)
    {
        cout<<it->first<<" : 0x"<<hex<<it->second->GetOffset()<<std::endl;
    }
}
void DF_Offsets::Save()
{
    std::fstream fout("offsets.txt",std::fstream::out);
    for(vOff::iterator it=myvec.begin();it!=myvec.end();it++)
    {
        if(it->second->GetOffset()!=0)
        fout<<it->first<<" : 0x"<<hex<<it->second->GetOffset()<<std::endl;
    }
}
void DF_Offsets::Load()
{
    std::fstream fin("offsets.txt",std::fstream::in);
    while(!fin.eof())
    {
        string s;
        fin>>s;
        vOff::iterator it=myvec.find(s);
        if(it!=myvec.end())
        {
            if(fin.eof())
                    return;
            char c;
            fin>>c;
            if(c!=':')
                throw "Offset load failed!";
            uint32_t off;
            fin>>hex>>off;
            it->second->SetOffset(off);
        }
        else
        {
            if(fin.eof())
                    return;
            char c;
            fin>>c;
            if(c!=':')
                throw "Offset load failed!";
            uint32_t off;
            fin>>hex>>off;
            new DF_Offset_static(s,off); //auto registering :)
        }
            //fin.ignore(256,'\n');
    }
}
uint32_t DF_Offsets::GetOff(string s)
{
    vOff::iterator it=myvec.find(s);
    if(it!=myvec.end())
    {
        return it->second->GetOffset();
    }
    return 0;
}
bool DF_Offsets::FindOff(string s)
{
    vOff::iterator it=myvec.find(s);
    if(it!=myvec.end())
    {
        return it->second->FindOff();
    }
    return false;
}
bool DF_Offsets::Search()
{
    cout<<"Searching offsets:"<<endl;
    for(vOff::iterator it=myvec.begin();it!=myvec.end();it++)
    {
        cout<<"Offset \""<<it->first<<"\" ";
        if(it->second->isLazy())
        {
            cout<<"is lazy, skipping\n";
            continue;
        }
       if(!it->second->FindOff())
       {
           cout<<"Failed!"<<endl;
            return false;
       }

        cout<<"found:0x"<<hex<<it->second->GetOffset()<<endl;
    }
    cout<<"Done!"<<endl;
    return true;
}

DF_Offsets::DF_Offsets():updated(false)
{

}
DF_Offsets::~DF_Offsets()
{
    if(updated)
    {
        Save();
    }
}
DF_allvecs *DF_allvecs::me=0;
DF_allvecs::DF_allvecs()
{
    uint32_t pos;
    pos=0;
    set_hexsearch2_limits(0,-1);
    posmap.clear();
    do
    {
        uint32_t vadd;
        //pos=hexsearch2(0xa1,ANYDWORD,0x2b,0x05,ANYDWORD,0xc1,0xf8,0x02,EOL);
        pos=hexsearch2(0x8b,ANYBYTE,ANYDWORD,0x8b,ANYBYTE,ANYDWORD,0x2b,ANYBYTE,EOL); //DF 31.12 onwards
        vadd=peekd(pos+2);
        //cout<<"vadd:"<<hex<<vadd<<endl;
        if(verify2(pos+8,DWORD_,vadd-4,EOL))
        {
            //cout<<"Found pos:"<<hex<<pos<<" with vec @:"<<vadd-4<<endl;
            posmap[vadd-4]++;
        }
        //cout<<"Found:"<<hex<<pos<<" value:"<<peekd(pos+3)<<endl;
        set_hexsearch2_limits(pos+12,-1);
    }
    while(pos!=0);
}
