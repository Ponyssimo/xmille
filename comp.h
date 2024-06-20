#ifndef COMP_H_
#define COMP_H_

int onecard(reg PLAY *pp);

int canplay(
    reg PLAY *pp,
    reg PLAY *op,
    reg CARD card
);

void calcmove();

#endif