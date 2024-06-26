/*
 * prompted.c
 *
 * get a string of text
 */

# include	<X11/Xlib.h>
# include	<X11/Xutil.h>
# include	"control.h"
# include	"co_class.h"
# include	<sys/time.h>
# include	<assert.h>
# include	<string.h>
# include	"init.h"
# include	"button.h"
# include	"dispatch.h"

/*
 *	prompted window on screen
 */

# define PROMPTED_Y	350
# define PROMPTED_HP	50	/* padding */
# define MIN_PROMPTED_W	(TEXTBOX_X + TEXTBOX_W + 20)
# define TEXTBOX_X	(10)
# define TEXTBOX_Y	(PROMPTED_HP)
# define TEXTBOX_W	150
# define TEXTBOX_VP	(10)
# define LABEL_Y	25

# define OK_X		(10)
# define OK_Y		(PROMPTED_HP * 2)

# define TEXT_OFF	5

extern int	co_inited;
extern	GC	co_fore_gc, co_back_gc;
static char	*label;
static int	prompted_h, prompted_w, label_x;
static Window	prompted, textbox, ok;
static char	returnbuffer[256];
static char	*retpointer;

static int	prompted_done;

char *
co_prompted(title, parent)
char *title;
Window	parent;
{
	XAnyEvent	rep;
	int		state, prompted_x;
	int		label_w;
	int		pco_OKstate (), co_promptedEvent (), CmanageButton (), textbox_event ();
	int		mask;
	int		height;

	if (!co_inited)
		co_init();
	label = title;
	label_w = XTextWidth(co_font, label, strlen(label));
	height = co_font->ascent;/* + co_font->descent;*/
	
	if (MIN_PROMPTED_W+10 >= label_w)
		prompted_w = MIN_PROMPTED_W+10;
	else
		prompted_w = label_w + 10;

	label_x = prompted_w / 2 - label_w / 2;
	prompted_h = OK_Y + 60;
	prompted_x = 512 - prompted_w/2;
	prompted = XCreateSimpleWindow (dpy, parent,
					prompted_x, PROMPTED_Y,
					prompted_w, prompted_h, 1, co_border,
					co_background);
	
	ok = CmapButton (prompted, OK_X, OK_Y, co_OK, pco_OKstate);
	textbox = XCreateSimpleWindow (dpy, prompted, TEXTBOX_X, TEXTBOX_Y,
				       TEXTBOX_W, height + TEXTBOX_VP * 2,
				       1, co_border, co_background);
	bindEvent (prompted, ExposureMask, co_promptedEvent);
	bindEvent (ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|
		   Button1MotionMask|Button2MotionMask|Button3MotionMask|LeaveWindowMask,
			CmanageButton);
	bindEvent (textbox, ExposureMask|KeyPressMask, textbox_event);
	bindEvent (prompted, KeyPressMask, textbox_event);
	XMapWindow (dpy, textbox);
	XMapWindow (dpy, prompted);

	prompted_done = 0;
	retpointer = returnbuffer;
	*retpointer = '\0';
	while (!prompted_done) {
		dispatch ();
	}
	
	unbindEvent (prompted, ExposureMask);
	unbindEvent (textbox, ExposureMask|KeyPressMask);
	unbindEvent (prompted, KeyPressMask);
	unbindEvent (ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|
		     Button1MotionMask|Button2MotionMask|Button3MotionMask|LeaveWindowMask);
	XUnmapWindow (dpy, prompted);
	CunmapButton (ok);
	XDestroyWindow (dpy, textbox);
	XDestroyWindow (dpy, prompted);
	*retpointer = '\0';
	return returnbuffer;
}

void predisplayLabel ()
{
	XDrawImageString (dpy, prompted, co_fore_gc, label_x, LABEL_Y, label, strlen (label));
	XFlush (dpy);
}

int co_promptedEvent (rep)
XAnyEvent	*rep;
{
	switch (rep->type) {
	case Expose:
		predisplayLabel ();
	}
}

void draw_string (string, on)
char	*string;
int		on;
{
	GC	my;

	my = on ? co_fore_gc : co_back_gc;
	XDrawImageString (dpy, textbox, my, 0, TEXTBOX_VP, string, strlen(string));
}

void redisplayText ()
{
	draw_string (returnbuffer, 1);
}

int pco_OKstate (int n)
{
	prompted_done = 1;
}

int compute_width (string)
char	*string;
{
	return XTextWidth (co_font, string, strlen(string));
}

int draw_char (int pos, char ch, int on)
{
	GC	my;

	my = on ? co_fore_gc : co_back_gc;
	XDrawImageString (dpy, textbox, my, pos, TEXTBOX_VP, &ch, 1);
}

void handle_char (char c)
{
	switch (c) {
	case '\n':
	case '\r':
		*retpointer = '\0';
		prompted_done = 1;
		break;
	case '\b':
	case '\177':
		if (retpointer > returnbuffer) {
			c = *--retpointer;
			*retpointer = '\0';
			draw_char (compute_width (returnbuffer), c, 0);
		}
		break;
	case '\030':
	case '\025':
		draw_string (returnbuffer, 0);
		retpointer = returnbuffer;
		*retpointer = '\0';
		break;
	default:
		*retpointer = '\0';
		draw_char (compute_width (returnbuffer), c, 1);
		*retpointer++ = c;
		*retpointer = '\0';
		break;
	}
}

int textbox_event (rep)
XAnyEvent	*rep;
{
	char	buffer[20], *pbuf;
	int		count;

	pbuf = buffer;
	switch (rep->type) {
	case Expose:
		redisplayText ();
		break;
	case KeyPress:
		count = XLookupString ((XKeyEvent*) rep, pbuf, 20, 0, 0);
		
		while (!prompted_done && count--) {
			handle_char (*pbuf++);
		}
		break;
	}
}
