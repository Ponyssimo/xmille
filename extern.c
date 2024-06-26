# include	"mille.h"

/*
 * @(#)extern.c	1.1 (Berkeley) 4/1/82
 */

bool	Debug,			/* set if debugging code on		*/
	Finished,		/* set if current hand is finished	*/
	Next,			/* set if changing players		*/
	On_exit,		/* set if game saved on exiting		*/
	Order,			/* set if hand should be sorted		*/
	Saved;			/* set if game just saved		*/

char	*Fromfile = NULL;	/* startup file for game		*/

int	Card_no,		/* Card number for current move		*/
	End,			/* End value for current hand		*/
	Handstart = COMP,	/* Player who starts hand		*/
	Movetype,		/* Current move type			*/
	Play,			/* Current player			*/
	Numgos,			/* Number of Go cards used by computer	*/
	WIndow = W_SMALL,	/* Current window wanted		*/
	Numseen[NUM_CARDS],	/* Number of cards seen in current hand	*/
	Value[NUM_MILES] = {	/* Value of mileage cards		*/
		25, 50, 75, 100, 200
	},
	Numcards[NUM_CARDS] = {	/* Number of cards in deck		*/
		10,	/* C_25 */
		10,	/* C_50 */
		10,	/* C_75 */
		12,	/* C_100 */
		4,	/* C_200 */
		2,	/* C_EMPTY */
		2,	/* C_FLAT */
		2,	/* C_CRASH */
		4,	/* C_STOP */
		3,	/* C_LIMIT */
		6,	/* C_GAS */
		6,	/* C_SPARE */
		6,	/* C_REPAIRS */
		14,	/* C_GO */
		6,	/* C_END_LIMIT */
		1,	/* C_GAS_SAFE */
		1,	/* C_SPARE_SAFE */
		1,	/* C_DRIVE_SAFE */
		1,	/* C_RIGHT_WAY */
		0	/* C_INIT */
	},
	Numneed[NUM_CARDS] = {	/* number of cards needed per hand	*/
		0,	/* C_25 */
		0,	/* C_50 */
		0,	/* C_75 */
		0,	/* C_100 */
		0,	/* C_200 */
		2,	/* C_EMPTY */
		2,	/* C_FLAT */
		2,	/* C_CRASH */
		4,	/* C_STOP */
		3,	/* C_LIMIT */
		2,	/* C_GAS */
		2,	/* C_SPARE */
		2,	/* C_REPAIRS */
		10,	/* C_GO */
		3,	/* C_END_LIMIT */
		1,	/* C_GAS_SAFE */
		1,	/* C_SPARE_SAFE */
		1,	/* C_DRIVE_SAFE */
		1,	/* C_RIGHT_WAY */
		0	/* C_INIT */
	};

CARD	Discard,		/* Top of discard pile			*/
	*Topcard,		/* Pointer to next card to be picked	*/
	OppositeCard[NUM_CARDS] = {	/* Opposites of each card		*/
		C_25, C_50, C_75, C_100, C_200, C_GAS, C_SPARE,
		C_REPAIRS, C_GO, C_END_LIMIT, C_EMPTY, C_FLAT, C_CRASH,
		C_STOP, C_LIMIT, C_EMPTY, C_FLAT, C_CRASH, C_STOP, C_INIT
	},
	Deck[DECK_SZ] = {	/* Current deck				*/
		C_25, C_25, C_25, C_25, C_25, C_25, C_25, C_25, C_25, C_25,
		C_50, C_50, C_50, C_50, C_50, C_50, C_50, C_50, C_50, C_50,
		C_75, C_75, C_75, C_75, C_75, C_75, C_75, C_75, C_75, C_75,
		C_100, C_100, C_100, C_100, C_100, C_100, C_100, C_100, C_100,
		C_100, C_100, C_100,
		C_200, C_200, C_200, C_200,
		C_EMPTY, C_EMPTY,
		C_FLAT, C_FLAT,
		C_CRASH, C_CRASH,
		C_STOP, C_STOP, C_STOP, C_STOP,
		C_LIMIT, C_LIMIT, C_LIMIT,
		C_GAS, C_GAS, C_GAS, C_GAS, C_GAS, C_GAS,
		C_SPARE, C_SPARE, C_SPARE, C_SPARE, C_SPARE, C_SPARE,
		C_REPAIRS, C_REPAIRS, C_REPAIRS, C_REPAIRS, C_REPAIRS,
			C_REPAIRS,
		C_END_LIMIT, C_END_LIMIT, C_END_LIMIT, C_END_LIMIT, C_END_LIMIT,
			C_END_LIMIT,
		C_GO, C_GO, C_GO, C_GO, C_GO, C_GO, C_GO, C_GO, C_GO, C_GO,
			C_GO, C_GO, C_GO, C_GO,
		C_GAS_SAFE, C_SPARE_SAFE, C_DRIVE_SAFE, C_RIGHT_WAY
	};

FILE	*outf;

PLAY	Player[2];		/* Player descriptions			*/
