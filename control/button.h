#ifndef BUTTON_H_
#define BUTTON_H_

struct button;

Button CcreateButton (
    char *text,
    int	width,
    GC gc,
    XFontStruct	*font,
    long backcolor,
    int	borderwidth
);

void buttonText (
    Window w,
    struct button *b,
    GC gc
);

void buttonOn (
    Window w,
    struct button *b
);

void buttonOff (
    Window w,
    struct button *b
);

Window CmapButton (
    Window	parent,
    int	x,
    int y,
    Button	button,
    int	(*notify)()
);

int CunmapButton (
    Window w
);

int CmanageButton (
    XAnyEvent *rep
);

int CredrawButton (
    Window w;
);

#endif