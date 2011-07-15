#include "DF_31_structs.h"
Race::Race(DWORD pos):DF_wrapper<DF_Race>(pos)
{

}

vCaste Race::GetCastes()
{
    vCaste ret(324+my_pos,false);//TODO no consts!
    return ret;
}


void Caste::PrintFlags()
{
    uint32_t s=data.flag_size.Get();
    uint32_t flg_str=data.ptr_flags.Get();
    //std::cout<<"Size:"<<s<<" "<<" flg_ptr:"<<std::hex<<flg_str<<std::endl;
    for(unsigned t=0;t<s;t++)
    {
        std::cout<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)peekb(flg_str+t)<<" ";
    }
    std::cout<<endl;
}
