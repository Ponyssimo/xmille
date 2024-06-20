#ifndef DRAWCARD_H_
#define DRAWCARD_H_

void bw_init_cards();

void init_cards ();

void displayCard (
    int	card,
    int	x,
    int y
);

void cardDisplay (
    struct card	*c,
    int x,
    int y
);

void drawIm (
    struct card	*c,
    int	x,
    int y
);

void cardRedisplay (
    int x,
    int y,
    int w,
    int h
);

void redisplaybelow (
    struct displayed *d,
    int x,
    int y,
    int w,
    int h
);

void cardEraseAll ();

#endif
