#ifndef DISPATCH_H_
#define DISPATCH_H_

void bindEvent (
    Window window,
    unsigned long eventMask,
    int (*func)()
);

void unbindEvent (
    Window window,
    unsigned long eventMask
);

void sendEvent (
    XAnyEvent *rep
);

void dispatch ();

struct eventGroup *allocGroup ();

void freeGroup (
    struct eventGroup *g
);

#endif
