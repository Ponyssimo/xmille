#include	"mille.h"
#include	"ui.h"
#include	"misc.h"
#include	"types.h"
#include	"comp.h"
#include	"animate.h"

/*
 * @(#)move.c	1.3 (Berkeley) 7/2/83
 */

extern CARD OppositeCard[];

#ifdef CTRL
#undef CTRL
#endif
#define	CTRL(c)		(c - 'A' + 1)

// #define ANIMATE 1

/*extern FILE *outf;*/

char	*Movenames[] = {
		"M_DISCARD", "M_DRAW", "M_PLAY", "M_ORDER"
	};

char	*playcard (), *sprint ();

/*
 * return whether or not the player has picked
 */
bool haspicked(pp)
reg PLAY	*pp; {

	reg int	card;

	if (Topcard <= Deck)
		return TRUE;
	switch (pp->hand[Card_no]) {
	  case C_GAS_SAFE:	case C_SPARE_SAFE:
	  case C_DRIVE_SAFE:	case C_RIGHT_WAY:
		card = 1;
		break;
	  default:
		card = 0;
		break;
	}
	return (pp->hand[card] != C_INIT);
}

void account(card)
reg CARD	card; {

	reg CARD	oppos;

	if (card == C_INIT)
		return;
	++Numseen[card];
	if (Play == COMP)
		switch (card) {
		  case C_GAS_SAFE:
		  case C_SPARE_SAFE:
		  case C_DRIVE_SAFE:
			oppos = OppositeCard[card];
			Numgos += Numcards[oppos] - Numseen[oppos];
			break;
		  case C_CRASH:
		  case C_FLAT:
		  case C_EMPTY:
		  case C_STOP:
			Numgos++;
			break;
		}
}

void sort(hand)
reg CARD	*hand;
{
	reg CARD	*cp, *tp;
	reg CARD	temp;

	cp = hand;
	hand += HAND_SZ;
	for ( ; cp < &hand[-1]; cp++)
		for (tp = cp + 1; tp < hand; tp++)
			if (*cp > *tp) {
				temp = *cp;
				*cp = *tp;
				*tp = temp;
			}
}

/*
 *	Check and see if either side can go.  If they cannot,
 * the game is over
 */
void check_go() {

	reg CARD	card;
	reg PLAY	*pp, *op;
	reg int		i;

	for (pp = Player; pp < &Player[2]; pp++) {
		op = (pp == &Player[COMP] ? &Player[PLAYER] : &Player[COMP]);
		for (i = 0; i < HAND_SZ; i++) {
			card = pp->hand[i];
			if (issafety(card) || canplay(pp, op, card)) {
				if (Debug) {
					fprintf(outf, "CHECK_GO: can play %s (%d), ", C_name[card], card);
					fprintf(outf, "issafety(card) = %d, ", issafety(card));
					fprintf(outf, "canplay(pp, op, card) = %d\n", canplay(pp, op, card));
				}
				return;
			}
			else if (Debug)
				fprintf(outf, "CHECK_GO: cannot play %s\n",
				    C_name[card]);
		}
	}
	Finished = TRUE;
}

void domove()
{
	reg PLAY	*pp;
	reg int		i, j;
	reg bool	goodplay;
	char		*foo;

	pp = &Player[Play];
	if (Debug)
	printf ("Got here 1\n");
	if (Play == PLAYER)
		getmove();
	else
		calcmove();
	Next = FALSE;
	goodplay = TRUE;
	if (Debug)
	printf ("Got here 2\n");
	switch (Movetype) {
	  case M_DISCARD:
		if (Debug)
		printf ("domove: Discard\n");
trydiscard:	;
		if (haspicked(pp)) {
			if (pp->hand[Card_no] == C_INIT)
				if (Card_no == 6)
					Finished = TRUE;
				else
					error("no card there");
			else {
				Discard = pp->hand[Card_no];
				pp->hand[Card_no] = C_INIT;
#ifdef ANIMATE
				animate_move (Play, ANIMATE_HAND, Card_no,
 						    ANIMATE_DISC, Discard);
#endif
				Next = TRUE;
				if (Play == PLAYER)
					account(Discard);
			}
		}
		else
			error("must pick first");
		break;
	  case M_REASONABLE:
	  case M_PLAY:
		if (Debug)
		printf ("Reasonable or play\n");
		foo = playcard(pp);
		if (foo) {
			if (Movetype == M_REASONABLE)
				goto trydiscard;
			error (foo);
			goodplay = 0;
		} else
			goodplay = 1;
		break;
	  case M_DRAW:
	if (Debug)
		printf ("Got here 3\n");
		Card_no = 0;
		if (Topcard <= Deck)
			error("no more cards");
		else if (haspicked(pp))
			error("already picked");
		else {
			pp->hand[0] = *--Topcard;
#ifdef ANIMATE
			animate_move (Play, ANIMATE_DECK, pp->hand[0], ANIMATE_HAND, 0);
#endif
			if (Debug)
			printf ("Got here 4\n");
			if (Debug)
				fprintf(outf, "DOMOVE: Draw %s\n", C_name[*Topcard]);
			if (Debug)
			printf ("Got here 5\n");
acc:
			if (Play == COMP) {
				account(*Topcard);
				if (issafety(*Topcard))
					pp->safety[*Topcard-S_CONV] = S_IN_HAND;
			}
			if (pp->hand[1] == C_INIT && Topcard > Deck) {
				Card_no = 1;
				pp->hand[1] = *--Topcard;
				if (Debug)
					fprintf(outf, "DOMOVE: Draw %s\n", C_name[*Topcard]);
				goto acc;
			}
			pp->new_battle = FALSE;
			pp->new_speed = FALSE;
		}
		break;

	  case M_ORDER:
		break;
	}
	/*
	 * move blank card to top by one of two methods.  If the
	 * computer's hand was sorted, the randomness for picking
	 * between equally valued cards would be lost
	 */
	if (Order && Movetype != M_DRAW && goodplay && pp == &Player[PLAYER])
		sort(pp->hand);
	else
		for (i = 1; i < HAND_SZ; i++)
			if (pp->hand[i] == C_INIT) {
				for (j = 0; pp->hand[j] == C_INIT; j++)
					if (j >= HAND_SZ) {
						j = 0;
						break;
					}
				pp->hand[i] = pp->hand[j];
				pp->hand[j] = C_INIT;
			}
	if (Topcard <= Deck)
		check_go();
	if (Next)
		nextplay();
}

char *
playcard(pp)
reg PLAY	*pp;
{
	reg int		v;
	reg CARD	card;

	/*
	 * check and see if player has picked
	 */
	switch (pp->hand[Card_no]) {
	  default:
		if (!haspicked(pp))
mustpick:
			return ("must pick first");
	  case C_GAS_SAFE:	case C_SPARE_SAFE:
	  case C_DRIVE_SAFE:	case C_RIGHT_WAY:
		break;
	}

	card = pp->hand[Card_no];
	if (Debug)
		fprintf(outf, "PLAYCARD: Card = %s\n", C_name[card]);
	Next = FALSE;
	switch (card) {
	  case C_200:
		if (pp->nummiles[C_200] == 2)
			return ("only two 200's per hand");
	  case C_100:	case C_75:
		if (pp->speed == C_LIMIT)
			return ("limit of 50");
	  case C_50:
		if (pp->mileage + Value[card] > End)
			return sprint ("puts you over %d", End);
	  case C_25:
		if (!pp->can_go)
			return ("cannot move now");
#ifdef ANIMATE
		animate_move (Play, ANIMATE_HAND, Card_no, ANIMATE_MILES, card);
#endif
		pp->nummiles[card]++;
		v = Value[card];
		pp->total += v;
		pp->hand_tot += v;
		if ((pp->mileage += v) == End)
			check_ext(FALSE);
		break;

	  case C_GAS:	case C_SPARE:	case C_REPAIRS:
		if (pp->battle != OppositeCard[card])
			return sprint ("can't play \"%s\"", C_name[card]);
#ifdef ANIMATE
		animate_move (Play, ANIMATE_HAND, Card_no, ANIMATE_BATTLE, card);
#endif
		pp->battle = card;
		if (pp->safety[S_RIGHT_WAY] == S_PLAYED)
			pp->can_go = TRUE;
		break;

	  case C_GO:
		if (pp->battle != C_INIT && pp->battle != C_STOP
		    && !isrepair(pp->battle))
			return sprint ("cannot play \"Go\"on a \"%s\"",
			    C_name[pp->battle]);
#ifdef ANIMATE
		animate_move (Play, ANIMATE_HAND, Card_no, ANIMATE_BATTLE, card);
#endif
		pp->battle = C_GO;
		pp->can_go = TRUE;
		break;

	  case C_END_LIMIT:
		if (pp->speed != C_LIMIT)
			return ("not limited");
#ifdef ANIMATE
		animate_move (Play, ANIMATE_HAND, Card_no, ANIMATE_SPEED, card);
#endif
		pp->speed = C_END_LIMIT;
		break;

	  case C_EMPTY:	case C_FLAT:	case C_CRASH:
	  case C_STOP:
		pp = &Player[other(Play)];
		if (!pp->can_go)
			return ("opponent cannot go");
		else if (pp->safety[safety(card) - S_CONV] == S_PLAYED)
protected:
			return ("opponent is protected");
#ifdef ANIMATE
		animate_move (Play, ANIMATE_HAND, Card_no, ANIMATE_OBATTLE, card);
#endif
		pp->battle = card;
		pp->new_battle = TRUE;
		pp->can_go = FALSE;
		pp = &Player[Play];
		break;

	  case C_LIMIT:
		pp = &Player[other(Play)];
		if (pp->speed == C_LIMIT)
			return ("opponent has limit");
		if (pp->safety[S_RIGHT_WAY] == S_PLAYED)
			goto protected;
#ifdef ANIMATE
		animate_move (Play, ANIMATE_HAND, Card_no, ANIMATE_OSPEED, card);
#endif
		pp->speed = C_LIMIT;
		pp->new_speed = TRUE;
		pp = &Player[Play];
		break;

	  case C_GAS_SAFE:	case C_SPARE_SAFE:
	  case C_DRIVE_SAFE:	case C_RIGHT_WAY:
		if (pp->battle == OppositeCard[card]
		    || (card == C_RIGHT_WAY && pp->speed == C_LIMIT)) {
			if (!(card == C_RIGHT_WAY && !isrepair(pp->battle))) {
				pp->battle = C_GO;
				pp->can_go = TRUE;
			}
			if (card == C_RIGHT_WAY && pp->speed == C_LIMIT)
				pp->speed = C_INIT;
			if (pp->new_battle
			    || (pp->new_speed && card == C_RIGHT_WAY)) {
				pp->coups[card - S_CONV] = TRUE;
				pp->total += SC_COUP;
				pp->hand_tot += SC_COUP;
				pp->coupscore += SC_COUP;
			}
		}
		/*
		 * if not coup, must pick first
		 */
		else if (pp->hand[0] == C_INIT && Topcard > Deck)
			goto mustpick;
#ifdef ANIMATE
		animate_move (Play, ANIMATE_HAND, Card_no, ANIMATE_SAFETY, card);
#endif
		pp->safety[card - S_CONV] = S_PLAYED;
		pp->total += SC_SAFETY;
		pp->hand_tot += SC_SAFETY;
		if ((pp->safescore += SC_SAFETY) == NUM_SAFE * SC_SAFETY) {
			pp->total += SC_ALL_SAFE;
			pp->hand_tot += SC_ALL_SAFE;
		}
		if (card == C_RIGHT_WAY) {
			if (pp->speed == C_LIMIT)
				pp->speed = C_INIT;
			if (pp->battle == C_STOP || pp->battle == C_INIT) {
				pp->can_go = TRUE;
				pp->battle = C_INIT;
			}
			if (!pp->can_go && isrepair(pp->battle))
				pp->can_go = TRUE;
		}
		Next = -1;
		break;

	  case C_INIT:
		Next = -1;
		return ("no card there");
		break;
	}
	if (pp == &Player[PLAYER])
		account(card);
	pp->hand[Card_no] = C_INIT;
	Next = (Next == -1 ? FALSE : TRUE);
	return 0;
}

	static char	spbuf[512];
char *
sprint (char *string, char *arg)
{

	sprintf (spbuf, string, arg);
	return spbuf;
}
