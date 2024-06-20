#ifndef AFFIRM_H_
#define AFFIRM_H_

int co_affirm(
    char *title,
    Window parent
);

void redisplayLabel ();

int co_affirmEvent (XEvent *rep);

int aco_OKstate (int n);

int co_CANCELstate (int n);

#endif