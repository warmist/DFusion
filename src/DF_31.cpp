#include "DF_31.h"
bool offset_creat_gloss::FindOff()
{

    std::map <uint32_t,int> posmap;
    posmap=DF_allvecs::Get().VectorDensityMap();
    set_hexsearch2_limits(0,-1);

    std::map<uint32_t,int>::iterator cpos=posmap.end();
    for(std::map<uint32_t,int>::iterator it=posmap.begin(); it!=posmap.end(); it++)
    {
        uint32_t ppos;
        //cout<<hex<<it->first<<" "<<dec<<it->second<<" Vec size:"<<(peekd(it->first+4)-peekd(it->first))/4<<endl;
        ppos=it->first;
        ppos=peekd(ppos);
        ppos=peekd(ppos);
        if(verify2(ppos,'T','O','A','D',0,EOL))
        {
            cout<<"POS:"<<hex<<it->first<<" passed TOAD test"<<endl;
            cpos=it;
        }
    }
    if(cpos!=posmap.end())
    {
        offset=cpos->first-df_memory_base;
        return true;
    }
    else
        return false;

}
offset_creat_gloss::offset_creat_gloss()
{
 DF_Offsets::Get().Register(this);
}
offset_reaction_vec::offset_reaction_vec()
{
     DF_Offsets::Get().Register(this);
}
bool offset_reaction_vec::FindOff()
{
    //uint32_t pos=-1;
    std::map <uint32_t,int> posmap;
    posmap=DF_allvecs::Get().VectorDensityMap();
    set_hexsearch2_limits(0,-1);

    std::map<uint32_t,int>::iterator cpos=posmap.end();
    for(std::map<uint32_t,int>::iterator it=posmap.begin(); it!=posmap.end(); it++)
    {
        uint32_t ppos;
        //cout<<hex<<it->first<<" "<<dec<<it->second<<" Vec size:"<<(peekd(it->first+4)-peekd(it->first))/4<<endl;
        ppos=it->first;
        ppos=peekd(ppos);
        ppos=peekd(ppos);
        if(verify2(ppos,'T','A','N','_','A','_','H','I','D','E',EOL))
        {
            cout<<"POS:"<<hex<<it->first<<" passed TAN_A_HIDE test"<<endl;
            cpos=it;
        }
    }

    if(cpos!=posmap.end())
    {
        offset=cpos->first-df_memory_base;
        return true;
    }
    else
        return false;

}
offset_cur_race::offset_cur_race()
{
    DF_Offsets::Get().Register(this);
}
bool offset_cur_race::FindOff()
{
    uint32_t pos=0;
 std::map <uint32_t,int> posmap;
    //CUR RACE OFFSET:

    set_hexsearch2_limits(0,-1);
    do
    {
        pos=hexsearch2(0x0f,0xbf,ANYBYTE,ADDRESS,EOL);
        //cout<<"Found:"<<hex<<pos<<" value:"<<peekd(pos+3)<<endl;
        posmap[peekd(pos+3)]++;
        set_hexsearch2_limits(pos+5,-1);
    }
    while(pos!=0);
    cout<<"Address\tcount\n";

    for(std::map<uint32_t,int>::iterator it=posmap.begin(); it!=posmap.end(); it++)
    {
        if((it->second>20))
            cout<<hex<<it->first<<" "<<dec<<it->second<<" Value @ loc:"<<peekd(it->first)<<endl;
    }
    std::map<uint32_t,int>::iterator maxpos=posmap.begin();
    for(std::map<uint32_t,int>::iterator it=posmap.begin(); it!=posmap.end(); it++)
    {
        if((maxpos->second<it->second)&&(peekd(it->first)<1000))
            maxpos=it;
    }
    cout<<"Most prob @:"<<hex<<maxpos->first<<" with:"<<dec<<maxpos->second<<endl;
    offset=maxpos->first-df_memory_base;
    return true;
}
offset_starting_dwarves::offset_starting_dwarves()
{
     DF_Offsets::Get().Register(this);
}
bool offset_starting_dwarves::FindOff()
{
    uint32_t pos=0;
    set_hexsearch2_limits(0,-1);
    pos=hexsearch2(0x24,0x14,0x07,0,0,0,0xeb,0x08,0x8d,EOL);
    if(pos!=0)
    {
        offset=pos+2-df_memory_base;
        return true;
    }
    return false;
}
offset_cur_civ::offset_cur_civ()
{
    DF_Offsets::Get().Register(this);
}
bool offset_cur_civ::FindOff()
{
    if(DF_Offsets::Get().GetOff("CurrentRace")!=0)
    {
        offset=DF_Offsets::Get().GetOff("CurrentRace")-12;
        return true;
    }
    return false;
}
DF_31::__InitOffsets DF_31::__InitOffsets::a;
