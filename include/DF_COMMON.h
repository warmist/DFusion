#ifndef DF_COMMON_H
#define DF_COMMON_H
#include "mytypes.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <map>
#include <windows.h>
#include "hexsearch.h"


#include <vector>
#include <fstream>
#include <list>
#include <time.h>

const size_t cr_size=2056;//7E0 real size
using namespace std;
typedef unsigned char byte;

template <typename T>
struct DF_Vector
{
    T *Start;
    T *End;
    T *Alloc_End;
    byte junk[4];
    size_t Size()
    {
        return ((int)End-(int)Start)/sizeof(T);
    }
    size_t Size_Abs()
    {
        return ((int)End-(int)Start);
    }
    size_t Capacity_Abs()
    {
        return ((int)Alloc_End-(int)Start);
    }
    /*T &at(int n)
    {
        return *(Start+n);
    }*/
    bool Insert(T *p)
    {
        if (End+1<=Alloc_End)
        {
            End++;
            pokearb(size_t(End-1),p,sizeof(T));
            return true;
        }
        return false;
    }
    void Load(int n,T *p)
    {
        peekarb(size_t(Start+n),p,sizeof(T));
    }
    void Save(int n,T *p)
    {
        pokearb(size_t(Start+n),p,sizeof(T));
    }
    bool IsValid()
    {
        int is,ie,iae;
        is=(int) Start;
        ie=(int) End;
        iae=(int)Alloc_End;
        return ((is%4==0)&&(ie%4==0)&&(iae%4==0)&&(is<ie)&&(iae>=ie));
    }
};
template <class T>
class Hak_vector
{

    public:
        DF_Vector <T> data;
        size_t my_pos;
        Hak_vector(size_t pos):my_pos(pos){};
        void Load(){peekarb(my_pos,&data,sizeof(data));};
        void Save(){pokearb(my_pos,&data,sizeof(data));};

};
struct DF_String
{
    byte junk[4];
    union
    {
        char *data_ptr;
        char data[16];
    };
    long size;
    unsigned long capacity;
    char *c_str()
    {
        //if(capacity>=16)
        //    return data_ptr;
        //else
        return data;
    }
    string LoadString()
    {

        if (capacity<16)
            return string(data,size);
        else
        {
            if(data_ptr!=0)
            {

            char *p=new char[size];//maybe a leak?
            peekarb((size_t)data_ptr,p,size);
            string ret(p,size);
            delete p;
            return ret;
            }
            return "";
        }
    }
    void SetString(string name)
    {
        if (capacity<16)
        {
            for(unsigned i=0;i<name.size();i++)
            data[i]=name[i];
            size=name.size();
        }
        else
        {
            if(capacity<name.size())return;
            size_t pos=(size_t)data_ptr;
            for(size_t i=0;i<name.size();i++)
                pokeb(pos,name[i]);
            size=name.size();
        }
    }
};
template <typename T>
struct Flag_vec
{
    T data;
    bool IsSet(int n)
    {
        return data & (1<<n);
    };
    void Flip(int n)
    {
        data=data^(1<<n);
    };
    void Set(int n,bool state)
    {
        if (state!=IsSet(n))
        {
            Flip(n);
        }
    }
    void Dump()
    {
        for (int i=0;i<sizeof(T)*8;i++)
            cout<<setw(2)<<i<<" ";
        cout<<endl;
        for (int i=0;i<sizeof(T)*8;i++)
            cout<<setw(2)<<IsSet(i)<<" ";
        cout<<endl;
    }

};
/*
    struct DF_Creature
    {
        DF_Var <long,100> Id;
        DF_Var <DF_String,10> name;
        buffer char ccc[size];
    };

*/
template <typename T,size_t offset>
struct DF_Var
{
    T GetInstant(size_t start)
    {
        T a;
        peekarb(start+offset,&a,sizeof(T));
        return a;
    };
    void SetInstant(T const &val,size_t start)
    {
        pokearb(start+offset,&val,sizeof(T));
    };
    T &Get() //warning black magic!
    {
        return *((T*)((size_t)this+offset));
    }
    void Set(T const &val)
    {
        memcpy(this+offset,&val,sizeof(T));
    }
};
template <int size>
struct Flag_vec_ex
{
    unsigned char data[size];
    Flag_vec_ex()
    {
        for(int i=0;i<size;i++)
        {
            data[i]=0;
        }
    }
    bool IsSet(int n)
    {

        return data[n/8] & (1<<(n%8));
    };
    void Flip(int n)
    {
        data[n/8]=data[n/8]^(1<<(n%8));
    };
    void Set(int n,bool state)
    {
        if (state!=IsSet(n))
        {
            Flip(n);
        }
    }
    void Dump()
    {
        //for (int i=0;i<size*8;i++)
         //   cout<<setw(2)<<i<<" ";
        //cout<<endl;
        for (int i=0;i<size*8;i++)
        {
            cout<<setw(1)<<IsSet(i);//<<" ";
            if((i%16)==15)
                cout<<endl;
        }

        cout<<endl;
    }

};
template <int SIZE>
struct junk_fill
{
    byte data[SIZE];
    void Dump()
    {
        cout<<hex;
        for (int i=0;i<SIZE;i++)
            cout<<setw(2)<<i<<" ";
        cout<<endl;
        for (int i=0;i<SIZE;i++)
        {
            cout<<setw(2)<<(int)data[i]<<" ";
            if ((i%32)==32-1)
                cout<<endl;
        }
        cout<<endl;
    }
};
template <class DataType>
class DF_wrapper
{
    protected:
    size_t my_pos;
    public:
        DataType data;
        DF_wrapper(size_t pos):my_pos(pos){};
        virtual ~DF_wrapper(){};
        void Save(){pokearb(my_pos,&data,sizeof(DataType));};
        void Load(){peekarb(my_pos,&data,sizeof(DataType));};
        void SetPos(size_t pos){my_pos=pos;};
        size_t GetPos(){return my_pos;};
};

class iDF_Offset
{
    public:
    iDF_Offset();
    virtual ~iDF_Offset(){};
    virtual string GetName()=0;
    virtual size_t GetOffset(){return offset;};
    virtual bool FindOff()=0;
    virtual void SetOffset(size_t off){offset=off;};
    virtual bool isLazy(){return false;};
    protected:
        uint32_t offset;

};
class DF_Offsets
{
    public:
        ~DF_Offsets();
        void Register(iDF_Offset *t);
        void Save();
        void Load();
        bool Search();
        void Print();
        size_t GetOff(string s);
        bool FindOff(string s);
        static DF_Offsets &Get();
    private:
        typedef std::map <string,iDF_Offset *> vOff;
        vOff myvec;
        DF_Offsets();
        static DF_Offsets *me;
        bool updated;

};
class DF_Offset_static:public iDF_Offset
{
    string name;
    public:
    DF_Offset_static(string name, uint32_t offset):name(name)
    {
        this->offset=offset;
        DF_Offsets::Get().Register(this);
    }
    string GetName(){return name;};
    bool FindOff(){return true;};
};
template <class DF_class,unsigned offset>
class DF_vec_wrap
{

    DF_Vector <size_t> clVec;
    size_t pos;
    public:
        DF_vec_wrap():pos(offset+df_memory_base){Update();};
        DF_vec_wrap(size_t pos,bool isStatic=true){
            if(isStatic)
                this->pos=pos+df_memory_base;
            else
                this->pos=pos;
            Update();
            };

        DF_class Load(int n)
        {
            size_t pos;
            clVec.Load(n,&pos);
            DF_class cl(pos);
            return cl;
        };
        void Update(){peekarb(pos,&clVec,sizeof(clVec));};
        size_t GetPos(int n)
        {
            size_t pos;
            clVec.Load(n,&pos);
            return pos;
        }
        size_t Size()
        {
            return clVec.Size();
        }
};

class DF_allvecs
{
    std::map <uint32_t,int> posmap;
    DF_allvecs();
    static DF_allvecs *me;
public:
    std::map <uint32_t,int> VectorDensityMap(){return posmap;};
    static DF_allvecs &Get()
    {
        if(!me)me=new DF_allvecs;
        return *me;
    }
};
struct DF_note
{
    char symbol;
    byte fill;
    unsigned short col1;
    unsigned short col2;
    unsigned short fill2;
    DF_String text;
    unsigned short x;
    unsigned short y;
    unsigned short z;

};
enum EFFECT_TYPE
{
    EFF_MIASMA=0,EFF_WATER,EFF_WATER2,EFF_BLOOD,EFF_DUST,EFF_MAGMA,EFF_SMOKE,EFF_DRAGONFIRE,EFF_FIRE,EFF_WEBING,EFF_BOILING,EFF_OCEANWAVE
};
struct DF_effect //size 40
{
    unsigned short type;
    unsigned short subtype;
    unsigned short unk2; //was FFFF for mist
    short lifetime;
    unsigned short x;
    unsigned short y;
    unsigned short z; //14
    short xdir;
    short ydir;
    junk_fill <12> unk4;
    byte canCreateNew;//??
    byte isHidden;


};
typedef DF_wrapper <DF_effect> Effect;
//typedef DF_vec_wrap <Effect,EFFECTS_VEC> Effects;
size_t GetUnusedEffect();
/*class EffectManager
{
    static EffectManager *me;
    unsigned long last
    public:
    static EffectManager &Get();
    void Reset();
};*/
struct DF_Name
{
    uint32_t name[7];
    uint16_t nametypes[7];
};
struct DF_Civ_ex
{
    union{
        DF_Var <uint16_t,0> status;
        //DF_Var <uint16_t,2> flags;//??
        DF_Var <size_t,4> ID;
        DF_Var <size_t,8> somepointer;//??
        //DF_Var <size_t[7],76> name;
        //DF_Var <uint16_t[7],104> nametypes;
        DF_Var <DF_Name,76> name;
        DF_Var <uint16_t,128> race;
        uint8_t data[2644]; //real size unknown
        //off:136 some list?
        //off:148 some vec ptrs to:(24~16 size)
        //off:172 some vec ptrs to:(~10 size)
        //off:196 vec to prob creature ids
    };
    void Dump()
    {
        cout<<dec;
        cout<<"\tID:    "<<ID.Get()<<endl;
        cout<<"\tStatus:"<<status.Get()<<endl;
        cout<<"\tRace:  "<<race.Get()<<endl;
    }
};
struct DF_Civ
{
    uint16_t ishidden;//2
    uint16_t unk;//4
    size_t ID;//8
    size_t somepointer; // starts with DF_STRING for biome
        //20


    //off 76: 7*4 name
    //off 104: 7*2 nametypes
    //off 128: Race
    //off 148: vector to structs*28bytes

};
typedef DF_wrapper <DF_Civ_ex> CivEntry;
//typedef DF_vec_wrap <CivEntry,CIV_VEC> CivList;
struct DF_info
{
    size_t vtable;
    size_t ID;
    string InfoType();
};

class InfoManager
{
    DF_Vector <size_t> infovec;
    public:
        InfoManager(DF_Vector <size_t> infovec);
        size_t FindOfType(string type);
        void Dump();
};
struct DF_item_info//?? size:24
{
    size_t item_ptr;
};

class Messages
{
    DF_Vector <size_t> pointers;
    public:
        void Update();
        size_t Size();
        string GetString(size_t s);
        void OverwriteString(size_t n,string s);
};
struct DF_projectile //size 104
{
    size_t vtable;
    size_t ListPtr;
    size_t ID;//??
    size_t OwnerPtr;
    size_t coords[16];//??
    junk_fill <20> fill;
    size_t ItemPtr;
};
typedef DF_wrapper <DF_projectile> Projectile;
struct DF_Creat_Gloss
{
    union
    {
    DF_Var<DF_String,0>   token;
    DF_Var<DF_String,28>  name;
    DF_Var<DF_String,56>  name_plural;
    DF_Var<DF_String,84>  adj;
    DF_Var<DF_String,112> remain;
    DF_Var<DF_String,140> remains;
    DF_Var<uint8_t,0x5d8> size;
    uint8_t data[0x5D9];
    };

};

struct DF_WordEntry
{
    //size_t ptr_next;
    DF_String word[10];

};
struct DF_WordCooked
{
    string word[10];
};


class DF_lang
{
    DF_Vector<size_t> wordpos;
    vector <DF_WordCooked> cwords;
public:
    DF_lang()
    {
      //  peekarb(WORDS_VEC+df_memory_base,&wordpos,sizeof(wordpos));
        //cout<<wordpos.Size()<<endl;

    }
    string GetWord(int a,int b)
    {
        DF_WordEntry we;
        size_t pos;
        wordpos.Load(a,&pos);
        peekarb(pos,&we,sizeof(we));
        return we.word[b].LoadString();
    }
    string GetWordCK(int a,int b)
    {
        return cwords[a].word[b];
    }
    void Cook()
    {
        for (unsigned i=0;i<wordpos.Size();i++)
        {
            DF_WordCooked cw;
            for (int j=0;j<10;j++)
            {
                cw.word[j]=GetWord(i,j);
            }
            cwords.push_back(cw);
        }
    }
    void Find(string word, int &a,int &b)
    {
        for (unsigned i=0;i<wordpos.Size();i++)
        {

            for (int j=0;j<10;j++)
            {
                if (word==cwords[i].word[j])
                {
                    a=i;
                    b=j;
                    return;
                }
            }
        }
        return ;
    }
};

typedef DF_wrapper <DF_Creat_Gloss> Creature_Gloss;
//typedef DF_vec_wrap <Creature_Gloss,CREAT_GLOSS> Races;
//void WriteToData(void *trg, void *src,size_t size)
void ScanVectors(size_t start,size_t end);


unsigned short GetCurCreature();
bool GetPaused();
void SetPaused(bool b);
size_t NormalizePtr(size_t ptr);
double Dist(double x,double y,double x2,double y2);
double Dist(double x,double y,double z,double x2,double y2,double z2);


#endif // DF_COMMON_H
