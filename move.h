#ifndef MOVE_H_
#define MOVE_H_

void domove();

void check_go();

char *playcard(reg PLAY *pp);

char *sprint (char *string, char *arg);

bool haspicked(reg PLAY *pp);

void account(reg CARD card);

void sort(reg CARD *hand);

#endif
