#include "types.h"
#include "defs.h"
#include "graphbase.h"
#include "guilayout.h"
#include "events.h"
#include "eventpackage.h"
#include "message_queue.h"
#include "message.h"
#include "mouse.h"

uchar typicalPointEvent(dom* which ,uint x,uint y,uint msgType)
{
    if (which->pid==-1)
        return 1;
    MouseMsg* mm = (MouseMsg*)kalloc();
    mm->msg_type = MOUSE_MESSAGE;
    mm->x = x;
    mm->y = y;
    mm->mouse_event_type = msgType;
    mm->dom_id = which->_id;
    if (msgType & LEFT_BTN_UP)
        setABSFocus(which);

    enqueue(which->pid, mm);
    return 1;
}

uchar typicalFocusEvent(dom* which,void* pkg)
{
    ((KBDMsg*)pkg)->dom_id=which->_id;
    enqueue(which->pid, pkg);
    return 1;
}
