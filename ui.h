#ifndef UI_H_
#define UI_H_

# include	"color.h"
# include	<X11/Xlib.h>
# include	"control/control.h"

extern	Display	*dpy;
extern	int	screen;
extern	Window	xwindow;
extern	XFontStruct	*font;
extern	Pixmap	fill;
extern	GC	black_gc, text_gc, blue_gc, cheap_gc;

struct card {
	Pixmap	bits;
};

extern struct card	cards[];
extern struct card	backside;
extern struct card	eraseCard;


struct safety_offset {
	int	x;
	int	y;
};

extern struct safety_offset	safety_offsets[4];

# define PAD_CARD	(5)
# define MILE_OFFSET	(5)
# define PAD_TEXT	(20)
# define DIST_HEIGHT	(15)
# define DIST_WIDTH	((WIDTH + PAD_CARD) * 5 - PAD_CARD)
# define DIST_MARK	(4)

# define COMP_HAND_X	(PAD_CARD)
# define COMP_HAND_Y	(-HEIGHT + font->descent)
# define COMP_DIST_TX	(PAD_CARD + (WIDTH + PAD_CARD) * 2)
# define COMP_DIST_TY	(PAD_CARD)
# define COMP_DIST_MX	(COMP_DIST_TX)
# define COMP_DIST_MY	(COMP_DIST_TY + PAD_TEXT)
# define COMP_DIST_X	(COMP_DIST_MX)
# define COMP_DIST_Y	(COMP_DIST_MY + DIST_MARK + 1)

# define COMP_PLAY_X	PAD_CARD
# define COMP_PLAY_Y	(COMP_DIST_Y + DIST_HEIGHT + PAD_CARD)

# define COMP_SAFE_X	(COMP_PLAY_X + ((WIDTH + PAD_CARD) * 7))
# define COMP_SAFE_Y	COMP_PLAY_Y

# define COMP_CARD_TX	PAD_CARD
# define COMP_CARD_TY	(COMP_PLAY_Y + HEIGHT + 6 * MILE_OFFSET + PAD_CARD)
# define COMP_CARD_X	COMP_CARD_TX
# define COMP_CARD_Y	(COMP_CARD_TY + PAD_TEXT)

# define MESS_X		(PAD_CARD)
# define MESS_Y		(COMP_PLAY_Y + HEIGHT + 6 * MILE_OFFSET + PAD_CARD + PAD_TEXT)
# define MESS_W		(200)
# define MESS_H		(font->ascent + font->descent)
# define PROMPT_X	MESS_X
# define PROMPT_Y	(MESS_Y + PAD_TEXT)
# define PROMPT_W	(MESS_W)
# define PROMPT_H	(MESS_H)
# define ERROR_X	PROMPT_X
# define ERROR_Y	(PROMPT_Y + PAD_TEXT)
# define ERROR_W	(MESS_W)
# define ERROR_H	(MESS_H)

# define QUIT_X		(MESS_X)
# define QUIT_Y		(ERROR_Y + PAD_TEXT + PAD_CARD)
# define SAVE_X		(QUIT_X + 75)
# define SAVE_Y		(QUIT_Y)

# define DISCARD_TX	(MESS_X + MESS_W + PAD_CARD)
# define DISCARD_TY	(MESS_Y - PAD_TEXT)
# define DISCARD_X	(DISCARD_TX)
# define DISCARD_Y	(DISCARD_TY + PAD_TEXT)

# define DECK_TX	(DISCARD_X + WIDTH + PAD_CARD)
# define DECK_TY	(DISCARD_TY)
# define DECK_X		(DECK_TX)
# define DECK_Y		(DISCARD_Y)

# define SCORE_W	(120)
# define SCORE_H	(font->ascent + font->descent)
# define SCORE_N	13
# define SCORE_X	(DECK_X + WIDTH + PAD_CARD + SCORE_W)
# define SCORE_Y	(DECK_TY)

# define HUM_DIST_TX	(COMP_DIST_TX)
# define HUM_DIST_TY	(SCORE_Y + SCORE_N * SCORE_H + PAD_CARD)
# define HUM_DIST_MX	(HUM_DIST_TX)
# define HUM_DIST_MY	(HUM_DIST_TY + PAD_TEXT)
# define HUM_DIST_X	(HUM_DIST_MX)
# define HUM_DIST_Y	(HUM_DIST_MY + DIST_MARK + 1)

# define HUM_PLAY_X	PAD_CARD
# define HUM_PLAY_Y	(HUM_DIST_Y + DIST_HEIGHT + PAD_CARD)
# define HUM_SAFE_X	(HUM_PLAY_X + ((WIDTH + PAD_CARD) * 7))
# define HUM_SAFE_Y	(HUM_PLAY_Y)

# define HUM_HAND_X	PAD_CARD
# define HUM_HAND_Y	(HUM_PLAY_Y + HEIGHT + 6 * MILE_OFFSET + PAD_CARD)

# define WINDOW_WIDTH	(HUM_SAFE_X + (WIDTH + PAD_CARD) * 2)
# define WINDOW_HEIGHT	(HUM_HAND_Y + HEIGHT + PAD_CARD)

void Message (
	char *string
);

void Error (
	char *string
);

void Prompt (
	char *string
);

void debug (
	int pos,
	char *string,
	int a0,
	char * a1
//	int a2
);

void ComputerStatus (
	char *string
);

void ComputerCard (
	int	type
);

void ComputerDistance (
	int distance
);

void EraseComputerDistance (
);

void RedisplayComputerDistance (
);

void ComputerSpeed (
	int type
);

void ComputerBattle (
	int type
);

void ComputerMiles (
	int type,
	int index,
	int count
);

void EraseComputerMiles (
);

void ComputerSafety (
	int type,
	int index
);

void DisplayDiscard (
	int type
);

void DisplayDeck (
	int numberLeft
);

void HumanDistance (
	int distance
);

void EraseHumanDistance (
);

void RedisplayHumanDistance (
);

void HumanSpeed (
	int type
);

void HumanBattle (
	int type
);

void HumanMiles (
	int type,
	int index,
	int count
);

void EraseHumanMiles (
);

void HumanSafety (
	int type,
	int index
);

void HumanHand (
	int	type,
	int index
);

void displayDistance (
	int x,
	int y,
	int value,
	int width,
	int height
);

void eraseDistance (
	int x,
	int y,
	int value,
	int width,
	int height
);

char *GetpromptedInput (
	char *string
);

void newboard(
);

void newscore(
);

void redraw_board (
);

void exposeBoard (
	XExposeEvent *rep
);

void redraw_region (
	int xpos,
	int ypos,
	int w,
	int h
);

void init_ui (
);

void finish_ui (
);

void update_ui (
);

void Beep (
);

int getyn(
	register char *prompt
);

void mouse_event (
	XButtonEvent *rep;
);

void key_event (
XKeyPressedEvent *rep;
);

void getmove(
);

void do_save (
);

void do_quit (
);

void prboard(
);

void InScore (
	int	line,
	int player,
	char *text
);

void prscore(
	register bool for_real
);

void FlushInput (
);

#endif
