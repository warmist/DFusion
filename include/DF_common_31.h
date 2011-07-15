#ifndef DF_COMMON_31_H_INCLUDED
#define DF_COMMON_31_H_INCLUDED
#include "DF_COMMON.h"
#define CREATURE_GLOSS_31 0x12B0EAC
namespace DF_31
{
    const long ptr_cur_race=0x1071FBC;
    const long ptr_cur_civ =0x1071FB0;
    //creatures vectors
    const long ptr_crt_1   =0x126FD00;//012F5230
    const long ptr_crt_2   =0x126FD78;
    const long ptr_crt_3   =0x126FD18;
    const long ptr_civ_vec =0x125C44C;

    const long ptr_sth_vec =0x12AFE5C; //history figures?

struct DF_Creature
{
    union
    {
    DF_Var<unsigned short,288>  civ;
    uint8_t data[300];
    };
};
typedef DF_wrapper <DF_Creature> Creature;
typedef DF_vec_wrap <Creature,ptr_crt_1> Creatures;
typedef DF_vec_wrap <Creature,ptr_crt_2> Creatures_2;
typedef DF_vec_wrap <Creature,ptr_crt_3> Creatures_3;
// ALL OK WITH OLD CIV
typedef DF_wrapper <DF_Civ_ex> Civ;
typedef DF_vec_wrap <Civ,ptr_civ_vec> Civ_vec;


};
#endif // DF_COMMON_31_H_INCLUDED
