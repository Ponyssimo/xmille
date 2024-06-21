/*
 * ui.c
 *
 * interface routines for mille
 */

# include	"mille.h"
# include	"ui.h"
# include	<X11/Xutil.h>
# include	"end.h"
# include	"save.h"
# include	"drawcard.h"
# include	"control/init.h"
# include	"control/affirm.h"
# include	"control/button.h"
# include	"control/dispatch.h"

#ifdef CTRL
# undef CTRL
#endif
#define CTRL(x)	(x - 'A' + 1)

# include	"card.h"

struct color colorMap[NUM_COLOR] = {
	"black",	0,
	"white",	0,
	"red",		0,
	"green",	0,
	"light gray",	0,
	"blue",		0,
};

char	*C_fmt = "%-18.18s";	/* format for printing cards		*/
char	Initstr[100];		/* initial string for error field	*/
char	*_cn[NUM_CARDS] = {	/* Card name buffer			*/
	"",
	"25",
	"50",
	"75",
	"100",
	"200",
	"Out of Gas",
	"Flat Tire",
	"Accident",
	"Stop",
	"Speed Limit", 
	"Gasoline",
	"Spare Tire",
	"Repairs",
	"Go",
	"End of Limit",
	"Extra Tank",
	"Puncture Proof",
	"Driving Ace",
	"Right of Way",
};
char	**C_name = &_cn[1];	/* Card names				*/

Display		*dpy;
int		screen;
Window		xwindow;
Pixmap		fill;
Button		QUIT, SAVE;
Window		qwindow, swindow;
XFontStruct	*font;
GC		text_gc;
GC		black_gc;
GC		blue_gc;
GC		cheap_gc;
Region		null, mine;

int	iscolor;

struct safety_offset safety_offsets[4] = {
	0,		0,
	WIDTH+PAD_CARD,	0,
	0,		HEIGHT+PAD_CARD,
	WIDTH+PAD_CARD,	HEIGHT+PAD_CARD,
};

static char	pbuf[512];
char *
prune (orig, max)
char	*orig;
int		max;
{
	int		len;

	len = strlen (orig);
	if( len == 0 )
		return(orig);
	if (XTextWidth (font, orig, len) < max)
		return orig;
	strcpy (pbuf, orig);
	do {
		pbuf[--len] = '\0';
	} while (len > 0 && XTextWidth (font, pbuf, len) >= max);
	return pbuf;
}

void Message (string)
char	*string;
{
	string = prune (string, MESS_W);
	XClearArea (dpy, xwindow, MESS_X, MESS_Y, MESS_W, MESS_H, False);
	XDrawImageString (dpy, xwindow, text_gc, MESS_X, MESS_Y + font->ascent,
			  string, strlen(string));
}

void Error (string, arg)
char *string;
void *arg;
{
	char	buf[512];
	char	*o;

	sprintf (buf, string, arg);
	o = prune (buf, ERROR_W);
	XClearArea (dpy, xwindow, ERROR_X, ERROR_Y, ERROR_W, ERROR_H, False);
	XDrawImageString (dpy, xwindow, text_gc, ERROR_X, ERROR_Y + font->ascent,
			  o, strlen(o));
}

void Prompt (string)
char *string;
{
	string = prune (string, PROMPT_W);
	XClearArea (dpy, xwindow, PROMPT_X, PROMPT_Y, PROMPT_W, PROMPT_H,
		    False);
	XDrawImageString (dpy, xwindow, text_gc, PROMPT_X, PROMPT_Y,
			  string, strlen(string));
}

void debug (pos, string, a0, a1)
int		pos;
char	*string;
int		a0;
char	*a1;
//int		a2;
{
}

void ComputerStatus (char *string)
{

	char	buffer[512];

	if (strlen (string) == 0)
		sprintf (buffer, "          ");
	else
		sprintf (buffer, "I %-10.10s", string);
	XDrawImageString (dpy, xwindow, text_gc, COMP_CARD_TX, COMP_CARD_TY,
			  buffer, strlen(buffer));

}

void ComputerCard (type)
int	type;
{
	/*	displayCard (type, COMP_CARD_X, COMP_CARD_Y);*/
}

static int	computer_distance = 0;

void ComputerDistance (int distance)
{
	displayDistance (COMP_DIST_X, COMP_DIST_Y, distance, DIST_WIDTH, DIST_HEIGHT);
	computer_distance = distance;
}

void EraseComputerDistance ()
{
	computer_distance = 0;
}

void RedisplayComputerDistance ()
{
	displayDistance (COMP_DIST_X, COMP_DIST_Y, computer_distance, DIST_WIDTH, DIST_HEIGHT);
}

void ComputerSpeed (int type)
{
	displayCard (type, COMP_PLAY_X, COMP_PLAY_Y);
}

void ComputerBattle (int type)
{
	displayCard (type, COMP_PLAY_X + WIDTH + PAD_CARD, COMP_PLAY_Y);
}

static int computer_miles_count[5];

void ComputerMiles (int type, int index, int count)
{
	while (computer_miles_count[index] < count) {
		displayCard (type, COMP_PLAY_X + (WIDTH + PAD_CARD) * (index + 2),
		    COMP_PLAY_Y + (MILE_OFFSET * computer_miles_count[index]));
		++computer_miles_count[index];
	}
}

void EraseComputerMiles ()
{
	int	i;

	for (i = 0; i < 5; i++)
		computer_miles_count[i] = 0;
}

void ComputerSafety (int type, int index)
{
	displayCard (type, COMP_SAFE_X + safety_offsets[index].x,
	    COMP_SAFE_Y + safety_offsets[index].y);
}

void DisplayDiscard (int type)
{
	displayCard (type, DISCARD_X, DISCARD_Y);
}

void DisplayDeck (int numberLeft)
{
	char	buffer[512];

	sprintf (buffer, "Cards: %3d  ", numberLeft);
	XDrawImageString (dpy, xwindow, text_gc, DECK_TX, DECK_TY,
			     buffer, strlen (buffer));
}

static int human_distance = 0;

void HumanDistance (int distance)
{
	displayDistance (HUM_DIST_X, HUM_DIST_Y, distance, DIST_WIDTH, DIST_HEIGHT);
	human_distance = distance;
}

void EraseHumanDistance ()
{
	human_distance = 0;
}

void RedisplayHumanDistance ()
{
	displayDistance (HUM_DIST_X, HUM_DIST_Y, human_distance, DIST_WIDTH, DIST_HEIGHT);
}

void HumanSpeed (int type)
{
	displayCard (type, HUM_PLAY_X, HUM_PLAY_Y);
}

void HumanBattle (int type)
{
	displayCard (type, HUM_PLAY_X + WIDTH + PAD_CARD, HUM_PLAY_Y);
}

static int human_miles_count[5];

void HumanMiles (int type, int index, int count)
{
	while (human_miles_count[index] < count) {
		displayCard (type, HUM_PLAY_X + (WIDTH + PAD_CARD) * (index + 2),
		    HUM_PLAY_Y + (MILE_OFFSET * human_miles_count[index]));
		++human_miles_count[index];
	}
}

void EraseHumanMiles ()
{
	int	i;

	for (i = 0; i < 5; i++)
		human_miles_count[i] = 0;
}

void HumanSafety (int type, int index)
{
	displayCard (type, HUM_SAFE_X + safety_offsets[index].x,
	    HUM_SAFE_Y + safety_offsets[index].y);
}

void HumanHand (type, index)
int	type, index;
{
	displayCard (type, HUM_HAND_X + (WIDTH + PAD_CARD) * index, HUM_HAND_Y);
}

void displayDistance (int x, int y, int value, int width, int height)
{
	XFillRectangle (dpy, xwindow, blue_gc, x, y, (value * width) / 1000,
			height);
}

void eraseDistance (int x, int y, int value, int width, int height)
{
	XClearArea (dpy, xwindow, x, y, (value * width) / 1000, height, TRUE);
}

char *
GetpromptedInput (string)
char	*string;
{
	extern char	*co_prompted ();

	return co_prompted (string, xwindow);
}

void newboard()
{
	XClearWindow (dpy, xwindow);
	cardEraseAll ();
	EraseHumanMiles ();
	EraseComputerMiles ();
	EraseHumanDistance ();
	EraseComputerDistance ();
	cardDisplay (&backside, DECK_X, DECK_Y);
	redraw_board ();
}

void newscore()
{
	InScore (-1, 0, "You");
	InScore (-1, 1, "Computer");
	InScore (0, -1, "Milestones");
	InScore (1, -1, "Safeties");
	InScore (2, -1, "All 4 Safeties");
	InScore (3, -1, "Coup Fourre");
	InScore (4, -1, "Trip Complete");
	InScore (5, -1, "Safe Trip");
	InScore (6, -1, "Delayed Action");
	InScore (7, -1, "Extension");
	InScore (8, -1, "Shut Out");
	InScore (9, -1, "Hand Total");
	InScore (10, -1, "Overall Total");
	InScore (11, -1, "Games");
}

void redraw_board ()
{
	redraw_region (0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

int exposeBoard (rep)
XExposeEvent	*rep;
{
	XRectangle	rect;

	rect.x = rep->x;
	rect.y = rep->y;
	rect.width = rep->width;
	rect.height = rep->height;

	XUnionRectWithRegion(&rect, mine, mine);
	if( rep->count == 0 )
	{
		XClipBox(mine, &rect);
		XIntersectRegion(null, mine, mine);
		redraw_region (rect.x, rect.y, rect.width, rect.height);
		/*redraw_board();*/
	}
	return 0;
}

void redraw_region (int xpos, int ypos, int w, int h)
{
	int	x1, y1, x2, y2;
	int	i;
	int	ym1, ym2, x;
	char	buffer[20];
	int	width;

	XDrawImageString (dpy, xwindow, text_gc, DISCARD_TX, DISCARD_TY,
			     "Discard Pile", 12);
	newscore ();
	RedisplayHumanDistance ();
	RedisplayComputerDistance ();
	x1 = HUM_HAND_X - PAD_CARD/2;
	y1 = HUM_HAND_Y - PAD_CARD/2;
	x2 = HUM_HAND_X + (WIDTH + PAD_CARD) * 7 - PAD_CARD/2;
	y2 = HUM_HAND_Y + (HEIGHT + PAD_CARD) - PAD_CARD/2;
	XDrawLine (dpy, xwindow, black_gc, x1, y1, x2, y1);
	XDrawLine (dpy, xwindow, black_gc, x2, y1, x2, y2);
	XDrawLine (dpy, xwindow, black_gc, x2, y2, x1, y2);
	XDrawLine (dpy, xwindow, black_gc, x1, y2, x1, y1);
	for (i = 0; i <= 1000; i += 100) {
		sprintf (buffer, "%d", i);
		x = COMP_DIST_TX + (i * DIST_WIDTH) / 1000;
		y1 = COMP_DIST_TY + MESS_H;
		ym1 = COMP_DIST_MY + MESS_H;
		y2 = HUM_DIST_TY + MESS_H;
		ym2 = HUM_DIST_MY + MESS_H;
		width = XTextWidth (font, buffer, strlen(buffer));
		XDrawImageString (dpy, xwindow, text_gc, x - width / 2,
				     y1, buffer, strlen(buffer));
		XDrawLine (dpy, xwindow, black_gc, x, ym1, x, ym1 + DIST_MARK);
		XDrawImageString (dpy, xwindow, text_gc, x - width / 2,
				     y2, buffer, strlen(buffer));
		XDrawLine (dpy, xwindow, black_gc, x, ym2, x, ym2 + DIST_MARK);
	}
	for (i = 1; i <= 6 ; i ++) {
		sprintf (buffer, "%d", i);
		x = HUM_HAND_X + WIDTH / 2 + (i * DIST_WIDTH * 2) / 10 ;
		y1 = HUM_HAND_Y - PAD_CARD/2 - 10;
                XDrawImageString (dpy, xwindow, text_gc, x - width / 2,
                                  y1, buffer, strlen(buffer));
	}
	cardRedisplay (xpos, ypos, w, h);
}

void init_ui ()
{
	XColor	hardware_color, exact_color;
	XClassHint	xch;
	XWMHints	xwm;
	XSizeHints	xsh;
	unsigned long	background;
	unsigned long	border;
	int	i;
	int	do_quit (), do_save ();
	int	CmanageButton ();
	XGCValues	gcv;
	Colormap	def_cm;

	dpy = XOpenDisplay ((char *) 0);
	screen = DefaultScreen(dpy);
	def_cm = DefaultColormap(dpy, screen);
	
	for (i = 0; i < NUM_COLOR; i++) {
		XAllocNamedColor (dpy, def_cm, colorMap[i].name,
				  &hardware_color, &exact_color);
		colorMap[i].pixel = hardware_color.pixel;
	}

	if (DisplayCells(dpy, screen) > 2)
		iscolor = 1;
	else
		iscolor = 0;

	if( ! iscolor )
	{
		colorMap[BLUE_COLOR].pixel = BlackPixel(dpy, screen);
		colorMap[GREY_COLOR].pixel = WhitePixel(dpy, screen);
	}
	
	background = colorMap[GREY_COLOR].pixel;
	
	if (iscolor)
		border = colorMap[WHITE_COLOR].pixel;
	else
		border = BlackPixel(dpy, screen);
	
#ifdef DEFAULT_FONT
	gcv.font = XLoadFont(dpy, DEFAULT_FONT);
#else
	gcv.font = XLoadFont(dpy, "fixed");
#endif
	font = XQueryFont(dpy, gcv.font);
	xwindow = XCreateSimpleWindow (dpy, DefaultRootWindow(dpy), 50, 50,
				 WINDOW_WIDTH, WINDOW_HEIGHT, 1,
				 border, background);

	XStoreName(dpy, xwindow, "XMILLE -- Version 2.0");
	XSetIconName(dpy, xwindow, "XMILLE");
	
	xch.res_name = "xmille";
	xch.res_class = "XMille";
	XSetClassHint(dpy, xwindow, &xch);

	xwm.flags = InputHint | StateHint;
	xwm.input = True;
	xwm.initial_state = NormalState;
	XSetWMHints(dpy, xwindow, &xwm);

	xsh.flags = PPosition | PSize | PMinSize | PMaxSize;
	xsh.x = 50;
	xsh.y = 50;
	xsh.width = xsh.min_width = xsh.max_width = WINDOW_WIDTH;
	xsh.height = xsh.min_height = xsh.max_height =  WINDOW_HEIGHT;
	XSetNormalHints(dpy, xwindow, &xsh);
	
	if( iscolor )
	{
		gcv.background = colorMap[GREY_COLOR].pixel;
		gcv.foreground = colorMap[BLACK_COLOR].pixel;
	}
	else
	{
		gcv.background = colorMap[WHITE_COLOR].pixel;
		gcv.foreground = colorMap[BLACK_COLOR].pixel;
	}
	
	text_gc = XCreateGC(dpy, xwindow, GCBackground | GCForeground | GCFont,
			    &gcv);

	black_gc = text_gc;
	blue_gc = text_gc;
	
	cheap_gc = XCreateGC(dpy, xwindow,
			     GCBackground | GCForeground | GCFont, &gcv);
	
	if( iscolor )
	{	
		gcv.foreground = colorMap[BLUE_COLOR].pixel;
		blue_gc = XCreateGC(dpy, xwindow,
				    GCBackground | GCForeground | GCFont,
				    &gcv);
	}
	
	if (iscolor)	{
		init_cards ();
	}
	else	{
		bw_init_cards();
	}
	co_init ();
	if (iscolor)	{
		QUIT = CcreateButton ("Quit", 50, black_gc, font,
				      colorMap[GREY_COLOR].pixel, 1);
		SAVE = CcreateButton ("Save", 50, black_gc, font,
				      colorMap[GREY_COLOR].pixel, 1);
	}
	else	{
		QUIT = CcreateButton ("Quit", 50, black_gc, font,
				      WhitePixel(dpy, screen), 1);
		SAVE = CcreateButton ("Save", 50, black_gc, font,
				      WhitePixel(dpy, screen), 1);
	}
	qwindow = CmapButton (xwindow, QUIT_X, QUIT_Y, QUIT, do_quit);
	swindow = CmapButton (xwindow, SAVE_X, SAVE_Y, SAVE, do_save);
	bindEvent (qwindow, ExposureMask|ButtonPressMask|LeaveWindowMask|ButtonReleaseMask,
	    CmanageButton);
	bindEvent (swindow, ExposureMask|ButtonPressMask|LeaveWindowMask|ButtonReleaseMask,
	    CmanageButton);
	XMapWindow (dpy, xwindow);

	bindEvent (xwindow, ExposureMask, exposeBoard);
	mine = XCreateRegion();
	null = XCreateRegion();
}


void finish_ui ()
{
}

void update_ui ()
{
	XFlush (dpy);
}

void Beep ()
{
	XBell (dpy, 0);
}

/*
 *	Get a yes or no answer to the given question.  Saves are
 * also allowed.  Return TRUE if the answer was yes, FALSE if no.
 */

int getyn(prompt)
register char	*prompt;
{
	return co_affirm (prompt, xwindow);
}

static char	incharacter;

static int	getmove_done;

int mouse_event (rep)
XButtonEvent	*rep;
{
	int	x, y;

	x = rep->x;
	y = rep->y;
	if (HUM_HAND_Y <= y && y <= HUM_HAND_Y + HEIGHT &&
	    HUM_HAND_X <= x && x <= HUM_HAND_X + (WIDTH + PAD_CARD) * 7) {
		switch (rep->button & 0377) {
		case Button1:
			Movetype = M_PLAY;
			break;
		case Button2:
			Movetype = M_REASONABLE;
			break;
		case Button3:
			Movetype = M_DISCARD;
			break;
		}
		Card_no = (x - HUM_HAND_X) / (WIDTH + PAD_CARD);
		getmove_done = 1;
		return 0;
	}
	if (DECK_Y <= y && y <= DECK_Y + HEIGHT &&
	    DECK_X <= x && x <= DECK_X + WIDTH) {
		Movetype = M_DRAW;
		ComputerStatus ("");
		getmove_done = 1;
		return 0;
	}
	Beep ();
}

int key_event (rep)
XKeyPressedEvent	*rep;
{
	char buffer[20];
	int bufsize = 20;
	KeySym keysym;
	XComposeStatus compose;
	int charcount;
	static int use;
	static int discard;
	charcount = XLookupString(rep, buffer, bufsize, &keysym, &compose);
	if (! (isalnum (buffer[0]) || isspace (buffer[0]))) return 0;
	if (Debug)
	printf ("Got a KEYPRESS: %c, count: %d\n", buffer[0], charcount);
	switch (buffer[0]) {
		case 's':
			if (discard == 1 || use == 1) {
				discard = 0; use = 0; Beep ();
				getmove_done = 0; break;
			}
			do_save (); getmove_done = 0;
		break;
		case 'Q':
			if (discard == 1 || use == 1) {
				discard = 0; use = 0; Beep ();
				getmove_done = 0; break;
			}
			die ();
		break;
		case 'd':
			if (discard == 1 || use == 1) {
				discard = 0; use = 0; Beep ();
				getmove_done = 0; break;
			}
			discard=1; use=0;
		break;
		case 'p':
			if (discard == 1) {
				if (Debug)
				printf ("Discard\n");
				Movetype = M_DISCARD;
			}
			else if (use == 1) {
				if (Debug)
				printf ("Play card\n");
				Movetype = M_PLAY;
			}
			else {
				if (Debug)
				printf ("Draw card\n");
				ComputerStatus ("");
				Movetype = M_DRAW;
			}
			Card_no=0;
			getmove_done=1;
			use = 0; discard = 0;
		break;
		case 'u':
			if (discard == 1 || use == 1) {
				discard = 0; use = 0; Beep ();
				getmove_done = 0; break;
			}
			use=1; discard = 0; getmove_done = 0;
		break;
		case '1': case '2': case '3': case '4': case '5':  case '6':
			buffer[1] = '\0';
			if (discard == 1) {
				getmove_done = 1;
				Movetype = M_DISCARD;
				Card_no=atoi (buffer);
				if (Debug)
				printf ("Discard %d\n", Card_no);
			}
			else if (use == 1) {
				getmove_done = 1;
				Movetype = M_PLAY;
				Card_no=atoi (buffer);
				if (Debug)
				printf ("Use %d\n", Card_no);
			}
			else {
				Beep ();  getmove_done = 0;
			}
			discard = 0; use = 0;
		break;
		default:
			discard = 0; use = 0; Beep ();
                        getmove_done = 0; break;
		break;
	}
	return 0;
}

void getmove()
{

	getmove_done = 0;
	bindEvent (xwindow, ButtonPressMask, mouse_event);
	bindEvent (xwindow, KeyPressMask, key_event);
	while (!getmove_done) {
		dispatch ();
	}
	unbindEvent (xwindow, ButtonPressMask|KeyReleaseMask);
}


int do_save ()
{
	save ();
}

int do_quit ()
{
	rub();
}

# define	COMP_STRT	20
# define	CARD_STRT	2

void prboard() {

	register PLAY	*pp;
	register int		i, k;

	for (k = 0; k < 2; k++) {
		pp = &Player[k];
		for (i = 0; i < NUM_SAFE; i++)
			if (pp->safety[i] == S_PLAYED) {
				if (k == 0) {
					HumanSafety (i + S_CONV, i);
				} else {
					ComputerSafety (i + S_CONV, i);
				}
			}
		if (k == 0) {
			HumanBattle (pp->battle);
			HumanSpeed (pp->speed);
		} else {
			ComputerBattle (pp->battle);
			ComputerSpeed (pp->speed);
		}
		for (i = C_25; i <= C_200; i++) {
			register int		end;

			end = pp->nummiles[i];
			if (k == 0)
				HumanMiles (i, C_200-i, end);
			else
				ComputerMiles (i, C_200-i, end);
		}
	}
	prscore(TRUE);
	pp = &Player[PLAYER];
	for (i = 0; i < HAND_SZ; i++) {
		HumanHand (pp->hand[i], i);
	}
	DisplayDeck (Topcard - Deck);
	DisplayDiscard (Discard);
}

void InScore (line, player, text)
int	line, player;
char	*text;
{
	if (player > 0)
	XDrawImageString (dpy, xwindow, text_gc, SCORE_X + player * SCORE_W/2,
			  SCORE_Y + SCORE_H * (line + 1), text, strlen (text));
	else 
	XDrawImageString (dpy, xwindow, text_gc, SCORE_X + player * SCORE_W,
			  SCORE_Y + SCORE_H * (line + 1), text, strlen (text));
}

void prscore(for_real)
register bool	for_real;
{

	register PLAY	*pp;
	register char	*Score_fmt = "%4d  ";
	char		buffer[512];

	ComputerDistance (Player[1].mileage);
	HumanDistance (Player[0].mileage);

	for (pp = Player; pp < &Player[2]; pp++) {
		sprintf (buffer, Score_fmt, pp->mileage);
		InScore (0, pp - Player, buffer);
		sprintf (buffer, Score_fmt, pp->safescore);
		InScore (1, pp - Player, buffer);
		if (pp->safescore == 400)
			InScore (2, pp - Player, " 300 ");
		else
			InScore (2, pp - Player, "   0 ");
		sprintf (buffer, Score_fmt, pp->coupscore);
		InScore (3, pp - Player, buffer);
#ifdef EXTRAP
		if (for_real)
			finalscore(pp);
		else
			extrapolate(pp);
#else
		finalscore(pp);
#endif
		sprintf (buffer, Score_fmt, pp->hand_tot);
		InScore (9, pp - Player, buffer);
		sprintf (buffer, Score_fmt, pp->total);
		InScore (10, pp - Player, buffer);
		sprintf (buffer, Score_fmt, pp->games);
		InScore (11, pp - Player, buffer);
	}
}

void FlushInput ()
{
}
