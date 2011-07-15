#ifndef FUNCTION_H
#define FUNCTION_H

#include <iostream>
#include <vector>
#include "DF_COMMON.h"
#include "Debuger.h"
#include "hexsearch.h"

//#define F_ENTRY_POINT 0x129BB0//0x127616
class Funct_Entry:public Breakpoint
{
    bool r;
    static Funct_Entry *me;
    Funct_Entry(size_t entry):Breakpoint(entry+df_memory_base),r(false)
    {
       setReseting(false);
       setAutoDeactivate(false);
    }
    public:
    static Funct_Entry *Get();
    void Hit()
    {
        r=true;
        #ifdef _DEBUG
        std::cout<<"---Entry Hit!---"<<std::endl;
        #endif
        Debuger::GetActiveDebuger()->SuspendThreads();

        //Deactivate();
        //SuspendThread(df_h_thread);
    }
    void Reset()
    {
        #ifdef _DEBUG
        std::cout<<"---Entry Reset---"<<std::endl;
        #endif
        if(isActive())
        {
            //throw an error
            std::cout<<"---Entry Reset error: Already active---"<<std::endl;

            return;
        }
        r=false;
        Activate();
    }
    bool IsReady()
    {
        return r;
    }

};
struct RegData
{
    uint32_t data;
    int num;
    RegData(){};
    RegData(int num,uint32_t data):data(data),num(num){};
};
typedef std::vector <DWORD> DWvec;
typedef std::vector <RegData> RegVec;
class F_bp;
class FunctionCaller
{
    DWvec arg;
    RegVec rarg;
    DWvec res;
    RegVec rres;
    DWORD resEax;
    DWORD funct;
    CONTEXT state;

    F_bp *bp;
    uint32_t space;
    private:
        void PushState();
        void PopState();
        void PrepArguments();
        void ExtractResults();
    public:
    bool isrunning;
    bool WasSuspended;
        FunctionCaller();

        void SetFunction(DWORD fadd);
        void Run(bool safe); //Must be paused if safe, runs the function.

        void SetArgs(DWvec arg);
        void SetArgsRegister(RegVec arg);
        void GetResults(DWvec &res);
        void GetResultsRegister(RegVec &res);
        DWORD GetResult();
        void BP_HIT();
};
class F_bp:public Breakpoint
{
FunctionCaller *t;
public:
    F_bp(FunctionCaller *t,DWORD pos):Breakpoint(pos),t(t){setReseting(false);};
    void Hit(){Deactivate();t->BP_HIT();};
};
void TestFunctCall();
/*
    //Calling funtion the safe way.
    push registers;
    push flags;
    //breakpoint
    push arguments;// and prep registers... in prog
    call function; //in prog?
    //breakpoint -> get results if you need them
    pop results;<-- find em
    pop flags;
    pop registers;
    //breakpoint
    return to pos;
    //way 2:
    find a safe place.
    call function
    pop results


*/



#endif // FUNCTION_H
