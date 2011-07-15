#include "Function.h"
#include <conio.h>

Funct_Entry *Funct_Entry::me=0;
Funct_Entry *Funct_Entry::Get()
{

    if(!me)
    {
        size_t ent=DF_Offsets::Get().FindOff("FunctionEntry");
        me=new Funct_Entry(ent);
    }
    return me;
}
void FunctionCaller::PushState()
{
    state.ContextFlags=CONTEXT_FULL;
    GetThreadContext(df_h_thread,&state);
    #ifdef _DEBUG
    std::cout<<"Saving state"<<std::endl;
    #endif
}
void FunctionCaller::PopState()
{
    state.ContextFlags=CONTEXT_FULL;
    SetThreadContext(df_h_thread,&state);
    #ifdef _DEBUG
    std::cout<<"Restoring state"<<std::endl;
    std::cout<<"Eip:"<<state.Eip<<std::endl;
    #endif
}
void FunctionCaller::PrepArguments()
{


    CONTEXT cnt;
    cnt.ContextFlags=CONTEXT_FULL;
    GetThreadContext(df_h_thread,&cnt);
    #ifdef _DEBUG
    std::cout<<"EIP Was:"<<std::hex<<cnt.Eip<<std::endl;
    std::cout<<"ESP Was:"<<std::hex<<cnt.Esp<<std::endl;
    #endif
   // PrintStack(arg.size()+1);
    cnt.Esp-=4*arg.size()+4;
    //std::cout<<"ESP Willbe:"<<std::hex<<cnt.Esp<<std::endl;
    for(size_t i=0;i<arg.size();i++)
        poked(cnt.Esp+(i+1)*4,arg[i]);
    poked(cnt.Esp,space);//state.Eip); //return address
    SetThreadContext(df_h_thread,&cnt);
    cnt.ContextFlags=CONTEXT_INTEGER;
    GetThreadContext(df_h_thread,&cnt);
    //std::cout<<"Ecx:"<<cnt.Ecx<<std::endl;



    for(size_t i=0;i<rarg.size();i++)
    {
        if(rarg[i].num==REG_EAX)
            cnt.Eax=rarg[i].data;
        if(rarg[i].num==REG_EBX)
            cnt.Ebx=rarg[i].data;
        if(rarg[i].num==REG_ECX)
            cnt.Ecx=rarg[i].data;
        if(rarg[i].num==REG_EDX)
            cnt.Edx=rarg[i].data;
    }
    SetThreadContext(df_h_thread,&cnt);
#ifdef _DEBUG
    std::cout<<"Stack after arguments:"<<std::endl;
    PrintStack(arg.size()+1);
    #endif
}
void FunctionCaller::ExtractResults()
{
    CONTEXT cnt;
    cnt.ContextFlags=CONTEXT_FULL;
    GetThreadContext(df_h_thread,&cnt);
    //TODO fix this
    resEax=cnt.Eax;
    //DWORD diff=state.Esp-cnt.Esp;
    //std::cout<<"Esp Diff:"<<diff<<std::endl;
    //std::cout<<"Eax:"<<std::hex<<cnt.Eax<<std::endl;
    //std::cout<<"Eip is now:"<<cnt.Eip<<std::endl;
}
DWORD FunctionCaller::GetResult()
{
    return resEax;
}
FunctionCaller::FunctionCaller()
{

}

void FunctionCaller::SetFunction(DWORD fadd)
{
    funct=fadd;
}
void FunctionCaller::Run(bool safe)
{
    WasSuspended=Debuger::GetActiveDebuger()->isSuspended();
    //bool ready;
    //Entry r1(ready);
    //r1.Activate();
    if(!safe)
    {
        #ifdef _DEBUG
        std::cout<<"Waiting for entry..."<<std::endl;
        #endif
    Funct_Entry::Get()->Reset();

    if(WasSuspended)
        Debuger::GetActiveDebuger()->ResumeThreads();
    while(!Funct_Entry::Get()->IsReady())
        Debuger::GetActiveDebuger()->Tick(100);
        #ifdef _DEBUG
        std::cout<<"Ok, entry found."<<std::endl;
        #endif
    }

    //r1.Deactivate();


    //Debuger::GetActiveDebuger()->SuspendThreads();
    space=Debuger::GetActiveDebuger()->AllocSpace(100);
    poked(space,0xcccccccc);
    //FlushInstructionCache(df_h_process,0,NULL);
    #ifdef _DEBUG
    std::cout<<"Running function"<<std::endl;
    #endif
    PushState();
    if(!safe)
        state.Eip--;
    #ifdef _DEBUG
    std::cout<<"Was @:"<<std::hex<<state.Eip<<std::endl;
    std::cout<<"Setting up bp @:"<<std::hex<<space<<std::endl;
    #endif
    bp=new F_bp(this,space);//state.Eip); //finish....//funct-1
    bp->Activate();
    CONTEXT cnt;
    cnt.ContextFlags=CONTEXT_FULL;
    GetThreadContext(df_h_thread,&cnt);
    cnt.Eip=funct;
    SetThreadContext(df_h_thread,&cnt);

    if(!safe)
    {
        Funct_Entry::Get()->Deactivate();
    }
    #ifdef _DEBUG
    std::cout<<"Setting up funct @:"<<std::hex<<funct<<std::endl;
    #endif
    PrepArguments();

    GetThreadContext(df_h_thread,&cnt);
    //std::cout<<"Eip is now:"<<cnt.Eip<<std::endl;
    //std::cout<<"Ecx is now:"<<cnt.Ecx<<std::endl;
    //ResumeThread(df_h_thread);
    //Debuger::GetActiveDebuger()->Continue();
    isrunning=true;
    Debuger::GetActiveDebuger()->ResumeThreads();
}
void FunctionCaller::BP_HIT()
{
    #ifdef _DEBUG
    std::cout<<"=======Hit End=========="<<std::endl;
    #endif
    ExtractResults();
    PopState();

    //CONTEXT cnt;
    //cnt.ContextFlags=CONTEXT_FULL;
    //GetThreadContext(df_h_thread,&cnt);
    //cnt.Eip--;
    //SetThreadContext(df_h_thread,&cnt);
    delete bp;

    //dbg.Continue();

    isrunning=false;
    Debuger::GetActiveDebuger()->UnAllocSpace(space);
    if(WasSuspended)
        Debuger::GetActiveDebuger()->SuspendThreads();
}
void FunctionCaller::SetArgs(DWvec arg)
{
    this->arg=arg;
}
void FunctionCaller::SetArgsRegister(RegVec arg)
{
    rarg=arg;
}
void FunctionCaller::GetResults(DWvec &res)
{
    res=this->res;
}
void FunctionCaller::GetResultsRegister(RegVec &res)
{
    res=rres;
}
void TestFunctCall()
{
    Debuger dbg;
    dbg.Attach();
    while(dbg.Tick(10));

    //DWORD ptr=0xde0000;
    DWORD ptr=dbg.AllocSpace(25);

    pokearb(ptr,"It workz?!\0",10);


    std::cout<<"Alloc:"<<std::hex<<ptr<<std::endl;

    //std::cout<<"Encoded:"<<std::hex<<EncodeDword(ptr)<<std::endl;
    //std::cout<<"Fptr:"<<std::hex<<0x0152AEB0<<" Is:"<<F_MSG+df_memory_base<<std::endl;
    DWvec args;
    RegVec rargs;
    args.push_back(2000);
    args.push_back(rand()%7);
    args.push_back(0x01);
    rargs.push_back(RegData(REG_ECX,ptr));
                                  //0x00100000



    FunctionCaller fm;
    //fm.SetFunction(F_MSG+df_memory_base);
    fm.SetArgs(args);
    fm.SetArgsRegister(rargs);
    //dbg.Break();
    fm.Run(false);
    fm.isrunning=true;
    //dbg.Continue();
    Debuger::GetActiveDebuger()->SetSS();
    dbg.Continue();
     //CONTEXT cnt;

    //cnt.ContextFlags=CONTEXT_FULL;
    while(fm.isrunning)
    {
        dbg.Tick(100);
        //GetThreadContext(df_h_thread,&cnt);
        //std::cout<<"Eip is now:"<<cnt.Eip<<std::endl;
    }
    //std::cin>>ptr;
      // Sleep(100);
    dbg.UnAllocSpace(ptr);
    std::cout<<"finish!"<<std::endl;



    //while(1)
    //{
    //    dbg.Tick(100);
    //    GetThreadContext(df_h_thread,&cnt);
    //    std::cout<<"Eip is now:"<<cnt.Eip<<std::endl;
    //}
}
