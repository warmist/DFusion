#ifndef PLUGMANAGER_H
#define PLUGMANAGER_H
#include "Debuger.h"
#include "hexsearch.h"
#include <vector>
#include <algorithm>
#include <string>
#include <string.h>
const char PlugSign[]="plugMNG";
const unsigned char cver=0;
struct PlugsHeader
{
    char signature[8];
    unsigned int ver;
    unsigned int count;
};

struct PlugEntry
{
    uint32_t ptr;
    char name[32];
};
typedef std::vector <PlugEntry> vPlugins;
class PlugManager
{
    public:
        PlugManager();
        PlugManager(uint32_t ptr);
        virtual ~PlugManager();

        void Install(uint32_t ptr);

        vPlugins GetList();
        uint32_t AddNewPlug(std::string name,uint32_t size,uint32_t loc=0);
        void GetHead();
        void SetHead();
        uint32_t FindPlugin(std::string name);

        static PlugManager &GetInst(){return *inst;};
    protected:
    private:
        static PlugManager *inst;
        uint32_t ptr;
        PlugsHeader ph;
};

#endif // PLUGMANAGER_H
