#ifndef ANIMATE_H_
#define ANIMATE_H_

void animate_move (
    int player,
    int orig_type,
    int orig_arg,
    int dest_type,
    int dest_arg
);

void do_animate (
    int ox,
    int oy,
    int dx,
    int dy
);

static
void draw_square (
    int x1,
    int y1,
    int x2,
    int y2
);

static
void compute_position (
    int player,
    int type,
    int arg,
    int xp,
    int yp
);

#endif
