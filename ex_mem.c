#include "types.h"
#include "defs.h"
#include "ex_mem.h"

void* ex_alloc(uint size)
{
    if (size>1024*1024*4)
        return 0;
    void* indexes=kalloc();
    uint blockneeded=(size>>12)+1;
    void** iterate=(void**)indexes;
    uint i;
    for (i=0;i<blockneeded;i++)
    {
        *iterate=kalloc();
        iterate++;
    }
    return indexes;
}
void* getPointer(void* res, uint pos)
{
    uint block=pos>>12;
    pos&=0x00000fff;
    return *(((void**)res)+block)+pos;
}
