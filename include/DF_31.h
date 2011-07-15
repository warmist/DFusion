#ifndef DF_31_H
#define DF_31_H
#include "DF_COMMON.h"

class offset_creat_gloss:public iDF_Offset
{
    public:
        offset_creat_gloss();
        string GetName(){return "CreatureGloss";};
        bool FindOff();
};
class offset_reaction_vec:public iDF_Offset
{
    public:
        offset_reaction_vec();
        string GetName(){return "ReactionVector";};
        bool FindOff();
};

class offset_cur_race:public iDF_Offset//-12 == cur civ
{
    public:
        offset_cur_race();
        string GetName(){return "CurrentRace";};
        bool FindOff();
};
class offset_starting_dwarves:public iDF_Offset
{
    public:
        offset_starting_dwarves();
        string GetName(){return "StartDwarfs";};
        bool FindOff();
};
class offset_cur_civ:public iDF_Offset//TODO
{
    public:
        offset_cur_civ();
        string GetName(){return "CurrentCiv";};
        bool FindOff();
};
namespace DF_31
{
struct __InitOffsets
{
    __InitOffsets(){

            //new offset_creat_gloss;
            //new offset_reaction_vec;

            //new offset_starting_dwarves;
            //new offset_cur_race;
              //  new offset_cur_civ;
    };
    static __InitOffsets a;
};
}

#endif // DF_31_H
