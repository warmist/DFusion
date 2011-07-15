#ifndef DEBUGER_H
#define DEBUGER_H
//#define __DEBUG
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x600
//#define _WIN32_WINNT 0x502
#endif

#include "windows.h"
#include "mytypes.h"
#include <tlhelp32.h>
#include <map>
#include <vector>
void PrintStack(size_t size);
class DF_Stack
{
    uint32_t pos;
    public:
        DF_Stack(uint32_t pos);
        uint32_t GetArg(size_t num);
};

class Breakpoint //shared pointer n stuff needed here
{
    bool reset;
    bool autodeact;
    public:
        virtual ~Breakpoint();
        Breakpoint(DWORD pos);
        void Activate();
        bool isActive();
        void Deactivate();
        virtual void Hit(){};
        bool isReseting(){return reset;};
        void setReseting(bool s){reset=s;};
        bool isAutoDeactivate(){return autodeact;};
        void setAutoDeactivate(bool s){autodeact=s;};
    private:
        BYTE replaced;
        DWORD pos;
        bool active;
};

BOOL CALLBACK HRemAll(DWORD inp);
struct BPState
{
    std::vector <Breakpoint *> active;
};
class Breakpoint_manager
{
    public:
        ~Breakpoint_manager();
        static Breakpoint_manager &Get();
        Breakpoint *Lookup(DWORD ataddress);
        void AddPoint(Breakpoint *p,DWORD atpos);
        void Remove(DWORD atpos);
        void CleanUp();
        void PushState();
        void PopState();

    private:
    Breakpoint_manager(){SetConsoleCtrlHandler(&HRemAll,true);};
    typedef std::map<DWORD,Breakpoint*> bMap;
    std::vector <BPState> bpStack;
    bMap mymap;
    static Breakpoint_manager *bpmanager;
};
enum REGENUM
{
    REG_EAX,REG_EBX,REG_ECX,REG_EDX,REG_ESI,REG_LAST
};
class Debuger
{
    public:
        Debuger();
        Debuger(DWORD pid);
        virtual ~Debuger();
        bool Tick(DWORD timeout);
        void Attach();
        void Break();
        void Continue();
        void ContinueOneStep();
        void SingleStep();
        void AdvanceAndPrint();
        void StopOnBreak(bool state);

        uint32_t GetRegister(REGENUM t);

        DWORD AllocSpace(size_t size); //VirtualAllocEx
        void UnAllocSpace(DWORD pos); //VirtualFreeEx

        uint32_t GetStackPos();
        static Debuger *GetActiveDebuger();

        bool SuspendThreads();
        bool ResumeThreads();
        bool isSuspended();
        void SetSS(){sson=true;SingleStep();};
        void TransferDebugger();
    protected:
    private:
        int susLevel;
        static Debuger *curDebuger;
        DWORD threadid;
        bool sson;
        DWORD pid;
        void* tid;
        Breakpoint *curBr;
        bool ResetCur;
        bool SOB;
        int skipsteps;
};
void DebuggerTest();
uint32_t EncodeDword(uint32_t t);
#endif // DEBUGER_H
