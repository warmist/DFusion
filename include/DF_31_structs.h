#ifndef DF_31_STRUCTS_H
#define DF_31_STRUCTS_H
#include "DF_COMMON.h"
#include "DF_31.h"
#include <iostream>
struct DF_Product
{
    union
    {
        DF_Var <uint32_t,0> vtable;
        DF_Var <uint16_t,32> type; //flags
        DF_Var <uint16_t,36> subtype; //flags?
    };
    char data[280];
};
typedef DF_wrapper <DF_Product> Product;
typedef DF_vec_wrap <Product,0> vProduct;
struct DF_Reaction
{
    union
    {
        DF_Var <DF_String,0> token;
        DF_Var <uint32_t,20> someval; //flags
        DF_Var <uint32_t,24> someval2; //flags?
        DF_Var <DF_String,28> name;
        DF_Var <DF_Vector<uint32_t>,76> reagents;
        DF_Var <vProduct,100> products;
        DF_Var <DF_Vector<uint32_t>,128> v3;
        DF_Var <DF_Vector<uint32_t>,152> v4;
        DF_Var <DF_Vector<uint32_t>,176> v5;
        DF_Var <DF_Vector<uint32_t>,200> v6;
        DF_Var <DF_Vector<uint32_t>,224> v7;
        DF_Var <DF_Vector<uint32_t>,248> v8;
       char data[280];
    };

};
typedef DF_wrapper <DF_Reaction> Reaction;
typedef DF_vec_wrap <Reaction,0> vReaction;

struct DF_Caste
{
    enum
    {
        FL_UNDERWATER=0,FL_UNDERWATER2=1 //2^x ... etc

    };
    union
    {
        DF_Var<DF_String,0>   token;
        DF_Var<DF_String,28>  name;
        DF_Var<DF_String,56>  name_plural;
        DF_Var<DF_String,84>  adj;
        DF_Var<uint32_t,0x510>  ptr_flags; //+7 -> can work?
        DF_Var<uint32_t,0x514>  flag_size;
        char data[0x520];
    };

};
//typedef DF_wrapper <DF_Caste> Caste;
class Caste:public DF_wrapper <DF_Caste>
{
    public:
        Caste(DWORD pos):DF_wrapper <DF_Caste>(pos){};
        void PrintFlags();
};
typedef DF_vec_wrap <Caste,0> vCaste;
struct DF_Race
{
    union
    {
    DF_Var<DF_String,0>   token;
    DF_Var<DF_String,28>  name;
    DF_Var<DF_String,56>  name_plural;
    DF_Var<DF_String,84>  adj;
    DF_Var<DF_Vector<uint32_t>,276>  likes;
    DF_Var<DF_Vector<uint32_t>,324>  castes;//this one? sth [+0x510]+4? //prob castes
    DF_Var<DF_Vector<uint32_t>,348>  v3;
    //DF_Var <DF_Vector<uint32_t>,1296??> v8;
    uint8_t data[0x16C];
    };
};
//typedef DF_wrapper <DF_Race> Race;
class Race:public DF_wrapper <DF_Race>
{
    public:
        Race(DWORD pos);
        ~Race(){};

        vCaste GetCastes();
};
typedef DF_vec_wrap <Race,0> vRace;

#endif // DF_31_STRUCTS_H
