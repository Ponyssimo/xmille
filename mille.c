# include	"mille.h"

/*
 * @(#)mille.c	1.3 (Berkeley) 5/10/83
 */

/*
 *	Routine to trap rubouts, and make sure they really want to quit.
 */
void rub() {

	signal(SIGINT, (void *) 0);
	if (getyn("Really? "))
		die();
	signal(SIGINT, rub);
}


char	_sobuf[BUFSIZ];

main(ac, av)
reg int		ac;
reg char	*av[]; {

	reg bool	restore;
#ifdef pdp11
	double		avs[3];
#endif

	if (strcmp(av[0], "a.out") == 0) {
		outf = fopen("q", "w");
		setbuf(outf, 0);
		Debug = TRUE;
	}
	restore = FALSE;
# ifdef pdp11
	if (geteuid() != ARNOLD) {
		loadav(avs);
		if (avs[2] > 9.0) {
			printf("Sorry.  The load average is too high.\n");
			printf("Please try again later\n");
			exit(1);
		}
	}
# endif
	switch (ac) {
	  case 2:
		rest_f(av[1]);
		restore = TRUE;
	  case 1:
		break;
	  default:
		printf("usage: milles [ restore_file ]\n");
		exit(1);
		/* NOTREACHED */
	}
	setbuf(stdout, _sobuf);
	Play = PLAYER;
	init_ui ();
# ifndef PROF
#ifdef sun
	srandom(getpid());
#else
	srand(getpid());
#endif
# else
	srand(0);
# endif
	signal(SIGINT, rub);
	for (;;) {
		if (!restore || (Player[PLAYER].total >= 5000 || Player[COMP].total >= 5000)) {
			if (Player[COMP].total < Player[PLAYER].total)
				Player[PLAYER].games++;
			else if (Player[COMP].total > Player[PLAYER].total)
				Player[COMP].games++;
			Player[COMP].total = 0;
			Player[PLAYER].total = 0;
		}
		do {
			if (!restore)
				Handstart = Play = other(Handstart);
			if (!restore || On_exit) {
				shuffle();
				init();
			}
			if (Debug)
			printf ("main: Before newboard\n");
			newboard();
			if (Debug)
			printf ("main: After newboard\n");
			if (restore)
				Error (Initstr);
			if (Debug)
			printf ("main: Before prboard\n");
			prboard();
			if (Debug)
			printf ("main: After prboard\n");
			do {
				if (Debug)
				printf ("main: Before domove\n");
				domove();
				if (Debug)
				printf ("main: After domove\n");
				if (Finished)
					if (Debug)
					printf ("main: Before newscore\n");
					newscore();
					if (Debug)
					printf ("main: After newscore\n");
					if (Debug)
				printf ("main: Before prboard\n");
				prboard();
					if (Debug)
				printf ("main: After prboard(2)\n");
			} while (!Finished);
					if (Debug)
			printf ("main: Before check_more\n");
			check_more();
					if (Debug)
			printf ("main: After check_more\n");
			restore = On_exit = FALSE;
		} while (Player[COMP].total < 5000
		    && Player[PLAYER].total < 5000);
	}
}


/*
 *	Time to go beddy-by
 */
die() {

	signal(SIGINT, (void *)0);
	if (outf)
		fflush(outf);
	finish_ui ();
	exit(1);
}
