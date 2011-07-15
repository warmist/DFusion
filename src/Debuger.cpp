#include "Debuger.h"

#include "windows.h"
#include "hexsearch.h"
#include <iostream>
#include <conio.h>
Debuger *Debuger::curDebuger=0;
void PrintStack(size_t size)
{

    CONTEXT cntx;
    cntx.ContextFlags=CONTEXT_CONTROL;
    GetThreadContext(df_h_thread,&cntx);
    DWORD pos=cntx.Esp;
    for(int i=0;i<size;i++)
    {
        std::cout<<std::hex<<peekd(pos)<<std::endl;
        pos+=4;
    }

}

DF_Stack::DF_Stack(uint32_t pos):pos(pos)
{

}
uint32_t DF_Stack::GetArg(size_t num)
{
    return peekd(pos+num*4);
}
Breakpoint_manager *Breakpoint_manager::bpmanager=0;
Breakpoint_manager &Breakpoint_manager::Get()
{
    //static Breakpoint_manager b;
    if(!Breakpoint_manager::bpmanager)
        Breakpoint_manager::bpmanager=new Breakpoint_manager;
    return *Breakpoint_manager::bpmanager;
}
Breakpoint *Breakpoint_manager::Lookup(DWORD ataddress)
{
    bMap::iterator i=mymap.find(ataddress);
    if(i!=mymap.end())
    {
        return i->second;
    }
    return 0;
}
void Breakpoint_manager::AddPoint(Breakpoint *p,DWORD atpos)
{
    mymap[atpos]=p;
}
void Breakpoint_manager::Remove(DWORD atpos)
{
    bMap::iterator i=mymap.find(atpos);
    if(i!=mymap.end())
    {
        if(i->second->isActive())
            i->second->Deactivate();
        mymap.erase(i);
    }
}
void Breakpoint_manager::PushState()
{
    Debuger::GetActiveDebuger()->SuspendThreads();
    BPState cstate;
    bMap::iterator i=mymap.begin();
    while(i!=mymap.end())
    {
        if(i->second&&i->second->isActive())
        {
            i->second->Deactivate();
            cstate.active.push_back(i->second);
        }
        i++;
    }
    bpStack.push_back(cstate);
    Debuger::GetActiveDebuger()->ResumeThreads();
}
void Breakpoint_manager::PopState()
{
    Debuger::GetActiveDebuger()->SuspendThreads();
    if(bpStack.size()<1) return ;//throw an error!
     BPState cstate;
     cstate=bpStack[bpStack.size()-1];
     bpStack.pop_back();
     for(size_t i=0;i<cstate.active.size();i++)
     {
         cstate.active[i]->Activate();
     }
    Debuger::GetActiveDebuger()->ResumeThreads();
}
void Breakpoint_manager::CleanUp()
{
    bMap::reverse_iterator i=mymap.rbegin();
    while(i!=mymap.rend())
    {

        if(i->second)
        {
            i->second->Deactivate();
            //delete i->second; //will auto delete when quits
        }

        i++;
    }
    mymap.clear();
}
Breakpoint_manager::~Breakpoint_manager()
{
    CleanUp();
}

Debuger::Debuger():sson(0)
{
#ifdef __DEBUG
    std::cout<<"<<DEBUGER created"<<std::endl;
#endif
    pid=0;
    curBr=0;
    ResetCur=false;
    SOB=false;
    skipsteps=0;
    //ctor
    curDebuger=this;
    susLevel=0;
}
Debuger::Debuger(DWORD pid):pid(pid),sson(0)
{
    //pid=0;
#ifdef __DEBUG
    std::cout<<"<<DEBUGER created"<<std::endl;
#endif
    curBr=0;
    ResetCur=false;
    SOB=false;
    skipsteps=0;
    curDebuger=this;
    susLevel=0;
}
Debuger::~Debuger()
{
#ifdef __DEBUG
    std::cout<<"<<DEBUGER destroyed"<<std::endl;
#endif
    if(sson)sson=false;
    Continue();
    if(pid)
        DebugActiveProcessStop(pid);
    //dtor
}
void Debuger::TransferDebugger()
{
    if(!susLevel)
        SuspendThreads();
    if(pid)
        DebugActiveProcessStop(pid);
    std::cout<<"Stoped process and debug, press enter."<<std::endl;
    char x;
    std::cin>>std::noskipws>>x;
    ResumeThreads();
}
void Debuger::Attach()
{
    /*HWND df_main_wh = FindWindow(NULL,"Dwarf Fortress");
    if(df_main_wh ==0)
        throw "YEah error!";

    */

    threadid = df_main_win_tid;
    if(threadid==0)
    {
        std::cout<<"Thread not found?"<<std::endl;
    }
    if(!pid)
    {
        open_dwarf_fortress();
        pid=df_pid;
    }
    if(!DebugActiveProcess(pid))
    {
        std::cout<<GetLastError()<<std::endl;
        throw "Debug failed!";
    }
    DebugSetProcessKillOnExit(false);
#ifdef __DEBUG
    std::cout<<"<<DEBUGER attach"<<std::endl;
#endif
}


    //tid = OpenThread(THREAD_ALL_ACCESS, FALSE, threadid);
void FlipTrap(CONTEXT &cnt)
{
    cnt.EFlags^=1<<8;
}
void SetTrap(CONTEXT &cnt,bool state)
{
    if((cnt.EFlags^(1<<8))&(1<<8))
    //{
        FlipTrap(cnt);
        //std::cout<<"Flipping!"<<std::endl;
   // }


}
void Debuger::SingleStep()
{
    SuspendThread(df_h_thread);
    CONTEXT cntx;
    cntx.ContextFlags=CONTEXT_FULL;
    GetThreadContext(df_h_thread,&cntx);
    //std::cout<<"SS on :"<<std::hex<<cntx.Eip<<endl;
    SetTrap(cntx,true);

    SetThreadContext(df_h_thread,&cntx);
    ResumeThread(df_h_thread);
}
void Debuger::StopOnBreak(bool state)
{
    SOB=state;
}
bool Debuger::Tick(DWORD timeout)
{
    DEBUG_EVENT lDebugEv;
    LPDEBUG_EVENT DebugEv=&lDebugEv;
    DWORD dwContinueStatus = DBG_CONTINUE; // exception continuation


   // Wait for a debugging event to occur. The second parameter indicates
   // that the function does not return until a debugging event occurs.

      if(!WaitForDebugEvent(DebugEv, timeout))
      {
#ifdef __DEBUG
        std::cout<<"No event!"<<std::endl;
#endif
          return false;
      }
#ifdef __DEBUG
        std::cout<<"Debug event! type("<<DebugEv->dwDebugEventCode<<":"<<DebugEv->u.Exception.ExceptionRecord.ExceptionCode<<")"<<std::endl;
#endif
   // Process the debugging event code.

      switch (DebugEv->dwDebugEventCode)
      {
         case EXCEPTION_DEBUG_EVENT:
         // Process the exception code. When handling
         // exceptions, remember to set the continuation
         // status parameter (dwContinueStatus). This value
         // is used by the ContinueDebugEvent function.
            //std::cout<<"Debug<-> event! type("<<DebugEv->u.Exception.ExceptionRecord.ExceptionCode<<")"<<std::endl;
            switch(DebugEv->u.Exception.ExceptionRecord.ExceptionCode)
            {
               case EXCEPTION_ACCESS_VIOLATION:
               // First chance: Pass this on to the system.
               // Last chance: Display an appropriate error.
               std::cout<<"!!Access violation!!:"<<std::hex<<(DWORD)DebugEv->u.Exception.ExceptionRecord.ExceptionAddress<<std::endl;
               std::cout<<"Registers:"<<std::hex<<std::endl;
               CONTEXT cntx;
                cntx.ContextFlags=CONTEXT_FULL;
                GetThreadContext(df_h_thread,&cntx);
                std::cout<<"EAX:"<<cntx.Eax<<"\tEBX:"<<cntx.Ebx<<"\tECX:"<<cntx.Ecx<<"\tEDX:"<<cntx.Edx<<std::endl;
                std::cout<<"ESP:"<<cntx.Esp<<"\tEBP:"<<cntx.Ebp<<"\tEIP:"<<cntx.Eip<<"\tESI:"<<cntx.Esi<<std::endl;
               //return true;
               SuspendThreads();
                  break;

               case EXCEPTION_BREAKPOINT:
               {

                DWORD pos=(DWORD)DebugEv->u.Exception.ExceptionRecord.ExceptionAddress;

                Breakpoint *p=Breakpoint_manager::Get().Lookup(pos);
#ifdef __DEBUG
                    std::cout<<"BP At:"<<std::hex<<pos<<" is ";
                if(peekb(pos)==0xCC)
                    std::cout<<"real"<<std::endl;
                else
                    std::cout<<"virtual"<<std::endl;
#endif
                //CONTEXT cntx;
                //cntx.ContextFlags=CONTEXT_FULL;
                //GetThreadContext(df_h_thread,&cntx);
                //std::cout<<"At:"<<cntx.Eip<<endl;
                //FlipTrap(cntx);
                //SetThreadContext(df_h_thread,&cntx);

                if(p)
                {
                    //std::cout<<"BP known:"<<std::hex<<pos<<std::endl;
                    curBr=p;

                    //PrintStack(9);
                    if(!SOB)
                    {

                       //std::cout<<"BP Continue:"<<std::hex<<pos<<std::endl;
                        //Continue();
                        //return true;
                        CONTEXT cntx;
                        cntx.ContextFlags=CONTEXT_CONTROL;
                        GetThreadContext(df_h_thread,&cntx);
                        if(p->isActive()&&p->isAutoDeactivate())
                        {
                        #ifdef __DEBUG
                                    std::cout<<"Rewinding and deactivating BP"<<std::endl;
                        #endif
                            cntx.Eip--;
                            SetThreadContext(df_h_thread,&cntx);
                            p->Deactivate();
                            if(p->isReseting())
                            {
                                skipsteps=2;
                                SingleStep();
                            }
                        }
                        curBr->Hit();
                    }


                }
                else
                {
                    curBr=0;

                }
                if(SOB) //stoping on break
                {
                    //std::cout<<"Stoping on break!"<<std::endl;
                    return true;
                }
                //std::cout<<"Resuming unknown bp:"<<std::hex<<pos<<std::endl;
                //Continue();

                //return ;
               // First chance: Display the current
               // instruction and register values.
                //return;
                  break;
               }

               case EXCEPTION_DATATYPE_MISALIGNMENT:
               // First chance: Pass this on to the system.
               // Last chance: Display an appropriate error.
                  break;

               case EXCEPTION_SINGLE_STEP:
               {
                DWORD pos=(DWORD)DebugEv->u.Exception.ExceptionRecord.ExceptionAddress;
#ifdef __DEBUG
                std::cout<<"SS @:"<<std::hex<<pos<<std::endl;
#endif
                if(sson)
                {
                    //cout<<"SS on!"<<endl;
                    SingleStep();
                }

                if(skipsteps)
                {
                    //std::cout<<"Skips left:"<<skipsteps<<endl;
                    //std::cout<<"Moar singlesteps"<<endl;
                    SingleStep();
                    skipsteps--;
                    if(skipsteps==0)
                     if(curBr)
                        {
                            curBr->Activate();
                        }
                }
                if(SOB)
                {
                    SingleStep();
                    return true;
                }
                else
                    Continue();
                return true;
               // First chance: Update the display of the
               // current instruction and register values.
                  break;
               }


               case DBG_CONTROL_C:
               // First chance: Pass this on to the system.
               // Last chance: Display an appropriate error.
                  break;

               default:
               // Handle other exceptions.
                  break;
            }

            break;

         case CREATE_THREAD_DEBUG_EVENT:
         // As needed, examine or change the thread's registers
         // with the GetThreadContext and SetThreadContext functions;
         // and suspend and resume thread execution with the
         // SuspendThread and ResumeThread functions.

            //dwContinueStatus = OnCreateThreadDebugEvent(DebugEv);
            break;

         case CREATE_PROCESS_DEBUG_EVENT:
         // As needed, examine or change the registers of the
         // process's initial thread with the GetThreadContext and
         // SetThreadContext functions; read from and write to the
         // process's virtual memory with the ReadProcessMemory and
         // WriteProcessMemory functions; and suspend and resume
         // thread execution with the SuspendThread and ResumeThread
         // functions. Be sure to close the handle to the process image
         // file with CloseHandle.

            //dwContinueStatus = OnCreateProcessDebugEvent(DebugEv);
            break;

         case EXIT_THREAD_DEBUG_EVENT:
         // Display the thread's exit code.

            //dwContinueStatus = OnExitThreadDebugEvent(DebugEv);
            break;

         case EXIT_PROCESS_DEBUG_EVENT:
         // Display the process's exit code.
            std::cout<<"Exit process code:"<<std::hex<<DebugEv->u.ExitProcess.dwExitCode<<std::endl;

            //dwContinueStatus = OnExitProcessDebugEvent(DebugEv);
            break;

         case LOAD_DLL_DEBUG_EVENT:
         // Read the debugging information included in the newly
         // loaded DLL. Be sure to close the handle to the loaded DLL
         // with CloseHandle.

            //dwContinueStatus = OnLoadDllDebugEvent(DebugEv);
            break;

         case UNLOAD_DLL_DEBUG_EVENT:
         // Display a message that the DLL has been unloaded.

            //dwContinueStatus = OnUnloadDllDebugEvent(DebugEv);
            break;

         case OUTPUT_DEBUG_STRING_EVENT:
         // Display the output debugging string.

            //dwContinueStatus = OnOutputDebugStringEvent(DebugEv);
            break;

         case RIP_EVENT:
            //dwContinueStatus = OnRipEvent(DebugEv);
            break;
      }

   // Resume executing the thread that reported the debugging event.
  // std::cout<<"Continue Proc:"<<DebugEv->dwProcessId<<" Thread:"<<DebugEv->dwThreadId<<endl;
   ContinueDebugEvent(DebugEv->dwProcessId,
                      DebugEv->dwThreadId,
                      dwContinueStatus);
    return true;
}
void Debuger::Break()
{
    bool ret=DebugBreakProcess(df_h_process);
    if(ret)
    {
        std::cout<<"Break ok"<<std::endl;

        return;
    }
    std::cout<<"Fail:"<<GetLastError()<<std::endl;
}
void Debuger::ContinueOneStep()
{
    SOB=true;
    SingleStep();
    ContinueDebugEvent(pid,threadid,DBG_CONTINUE);
    //std::cout<<"Continue Manual:"<<pid<<" Thread:"<<threadid<<endl;
}

void Debuger::Continue()
{
    CONTEXT cntx;
    cntx.ContextFlags=CONTEXT_CONTROL;
    GetThreadContext(df_h_thread,&cntx);
    Breakpoint *p=Breakpoint_manager::Get().Lookup(cntx.Eip-1);
#ifdef __DEBUG
    std::cout<<"Continue @:"<<std::hex<<cntx.Eip<<std::endl;
#endif
    if(p!=0)
    {
        if(p->isActive()&&p->isAutoDeactivate())
        {
#ifdef __DEBUG
            std::cout<<"Rewinding and deactivating BP"<<std::endl;
#endif
            cntx.Eip--;
            SetThreadContext(df_h_thread,&cntx);
            p->Deactivate();
            if(p->isReseting())
            {
                skipsteps=2;
                SingleStep();
            }
        }

    }
#ifdef __DEBUG
    std::cout<<"Continue Manual:"<<pid<<" Thread:"<<threadid<<std::endl;
#endif
    ContinueDebugEvent(pid,threadid,DBG_CONTINUE);

}
void Debuger::AdvanceAndPrint()
{
    curBr=0;
    //SOB=true;
    skipsteps=1;

    SingleStep();
    Continue();
    //for(int i=0;i<100;i++)
    Tick(100);
    CONTEXT cntx;
    cntx.ContextFlags=CONTEXT_CONTROL;
    GetThreadContext(df_h_thread,&cntx);
    std::cout<<"Pos:"<<std::hex<<cntx.Eip<<std::endl;
    //SOB=false;
}
uint32_t Debuger::GetStackPos()
{
    CONTEXT cntx;
    cntx.ContextFlags=CONTEXT_CONTROL;
    GetThreadContext(df_h_thread,&cntx);
    return cntx.Esp;
}
Breakpoint::Breakpoint(DWORD pos):pos(pos),active(0),reset(0)
{
    #ifdef __DEBUG
    std::cout<<"Breakpoint::Setting="<<std::hex<<pos<<std::endl;
    #endif
    Breakpoint_manager::Get().AddPoint(this,pos);
    autodeact=true;
}
void Breakpoint::Activate()
{
    if(!active)
    {
        #ifdef __DEBUG
        std::cout<<"Breakpoint::Activating="<<std::hex<<pos<<std::endl;
        #endif
        replaced=peekb(pos);
        pokeb(pos,0xCC);// int 3
        FlushInstructionCache(df_h_process,0,NULL);
        active=true;
    }
}
bool Breakpoint::isActive()
{
    return active;
}
void Breakpoint::Deactivate()
{
    if(active)
    {
        #ifdef __DEBUG
        std::cout<<"Breakpoint::Deactivate="<<std::hex<<pos<<std::endl;
        #endif
        pokeb(pos,replaced);
        FlushInstructionCache(df_h_process,0,NULL);
        active=false;
    }
}
Breakpoint::~Breakpoint()
{
    Deactivate();
    Breakpoint_manager::Get().Remove(pos);
    #ifdef __DEBUG
    std::cout<<"Breakpoint::destroyed="<<std::hex<<pos<<std::endl;
    #endif
}

DWORD Debuger::AllocSpace(size_t size) //VirtualAllocEx
{
    return (DWORD)VirtualAllocEx(df_h_process,0,size,MEM_COMMIT | MEM_RESERVE,PAGE_EXECUTE_READWRITE);//EXECUTE?? ow well more fun
}
void Debuger::UnAllocSpace(DWORD pos) //VirtualFreeEx
{
    VirtualFreeEx(df_h_process,(void*)pos,0,MEM_RELEASE);//size not important
}
void DebuggerTest()
{
    Debuger dbg;
    dbg.Attach();


    //dbg.Break();

   // Breakpoint bp(0x456000+df_memory_base);
    class brk:public Breakpoint
    {
        Debuger *m;
        public:
            brk(Debuger *d):Breakpoint(0x1faeb0+df_memory_base),m(d){};
            void Hit(){
                std::cout<<"msg!"<<std::endl;
                //DF_Stack stk(m->GetStackPos());
                //for(int i=0;i<6;i++)
                //std::cout<<"Arg:"<<std::dec<<i<<" val:"<<std::hex<<stk.GetArg(i)<<std::endl;
                std::cout<<"ECX:"<<m->GetRegister(REG_ECX)<<std::endl;

                };
    }bp(&dbg);
    bp.Activate();
    bp.setReseting(true);
    bool run=true;
    while(run)
    {
        while(!dbg.Tick(100));
        //getch();
        //dbg.AdvanceAndPrint();
        if(kbhit())
        {
            if(getch()=='q')
                run=false;
        }
    }

}
Debuger *Debuger::GetActiveDebuger(){
    if(!curDebuger)
        curDebuger=new Debuger();
    return curDebuger;
    };
BOOL HRemAll(DWORD inp)
{
    if((inp==CTRL_C_EVENT)||(inp==CTRL_BREAK_EVENT)||(inp==CTRL_CLOSE_EVENT))
        Breakpoint_manager::Get().CleanUp();
    return false;
}
uint32_t Debuger::GetRegister(REGENUM t)
{
    CONTEXT cntx;
    cntx.ContextFlags=CONTEXT_FULL;
    GetThreadContext(df_h_thread,&cntx);
    switch(t)
    {
        case REG_EAX:
            return cntx.Eax;
        case REG_EBX:
            return cntx.Ebx;
        case REG_ECX:
            return cntx.Ecx;
        case REG_EDX:
            return cntx.Edx;
        case REG_ESI:
            return cntx.Esi;
    }
}
uint32_t EncodeDword(uint32_t t)
{
    //0xFFFF FFFF
    /*std::cout<<std::hex;
    std::cout<<t<<std::endl;
    std::cout<<"="<<std::endl;
    std::cout<<(t<<24)<<std::endl;
    std::cout<<"="<<std::endl;
    std::cout<<((t&0xFF00)<<8)<<std::endl;
    std::cout<<"="<<std::endl;
    std::cout<<((t&0xff0000)>>8)<<std::endl;
    std::cout<<"="<<std::endl;
    std::cout<<((t&0xff000000)>>24)<<std::endl;*/
    return (t<<24)|((t&0xFF00)<<8)|((t&0xff0000)>>8)|(t&0xff000000)>>24;
}
bool Debuger::isSuspended()
{
    return susLevel>0;
}
bool Debuger::SuspendThreads()
{
    #ifdef __DEBUG
    std::cout<<"XXX Suspending threads XXX"<< "\nXXXsuspend level:"<<susLevel<<"XXX"<<std::endl;
    #endif
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
  THREADENTRY32 te32;

  // Take a snapshot of all running threads
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
  if( hThreadSnap == INVALID_HANDLE_VALUE )
    return( FALSE );

  // Fill in the size of the structure before using it.
  te32.dwSize = sizeof(THREADENTRY32 );

  // Retrieve information about the first thread,
  // and exit if unsuccessful
  if( !Thread32First( hThreadSnap, &te32 ) )
  {
    //printError( "Thread32First" );  // Show cause of failure
    CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
    return( FALSE );
  }

  // Now walk the thread list of the system,
  // and display information about each thread
  // associated with the specified process
  do
  {
    if( te32.th32OwnerProcessID == df_pid )
    {
        HANDLE p=OpenThread(THREAD_ALL_ACCESS,FALSE,te32.th32ThreadID);
        SuspendThread(p);
        CloseHandle(p);
      //printf( "\n\n     THREAD ID      = 0x%08X", te32.th32ThreadID );
     // printf( "\n     base priority  = %d", te32.tpBasePri );
     // printf( "\n     delta priority = %d", te32.tpDeltaPri );
    }
  } while( Thread32Next(hThreadSnap, &te32 ) );

//  Don't forget to clean up the snapshot object.
  CloseHandle( hThreadSnap );
   susLevel++;
  return( TRUE );

}
bool Debuger::ResumeThreads()
{
    #ifdef __DEBUG
    std::cout<<"XXXResuming threadsXXX"<<std::endl;
    #endif
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
  THREADENTRY32 te32;

  // Take a snapshot of all running threads
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
  if( hThreadSnap == INVALID_HANDLE_VALUE )
    return( FALSE );

  // Fill in the size of the structure before using it.
  te32.dwSize = sizeof(THREADENTRY32 );

  // Retrieve information about the first thread,
  // and exit if unsuccessful
  if( !Thread32First( hThreadSnap, &te32 ) )
  {
   // printError( "Thread32First" );  // Show cause of failure
    CloseHandle( hThreadSnap );     // Must clean up the snapshot object!
    return( FALSE );
  }

  // Now walk the thread list of the system,
  // and display information about each thread
  // associated with the specified process
  do
  {
    if( te32.th32OwnerProcessID == df_pid )
    {
        HANDLE p=OpenThread(THREAD_ALL_ACCESS,FALSE,te32.th32ThreadID);
        ResumeThread(p);
        CloseHandle(p);
      //printf( "\n\n     THREAD ID      = 0x%08X", te32.th32ThreadID );
     // printf( "\n     base priority  = %d", te32.tpBasePri );
     // printf( "\n     delta priority = %d", te32.tpDeltaPri );
    }
  } while( Thread32Next(hThreadSnap, &te32 ) );

//  Don't forget to clean up the snapshot object.
  CloseHandle( hThreadSnap );
   if(susLevel>0)susLevel--;
  return( TRUE );


}
