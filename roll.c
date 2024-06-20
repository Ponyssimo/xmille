/*
 *	This routine rolls ndie nside-sided dice.
 *
 * @(#)roll.c	1.1 (Berkeley) 4/1/82
 *
 */

#include <stdlib.h>

# define	reg	register

# if (! defined ultrix && ! defined mips && ! defined vax)
# define	MAXRAND	32767L

int roll(ndie, nsides)
int	ndie, nsides; {

	reg long	tot;
	reg long	work;
	reg unsigned	n;

	tot = 0;
	n = ndie;
#ifndef sun
	while (n--) tot += rand();
	work = ((tot * (long) nsides) / ((long) MAXRAND + 1)) + ndie;
#else
	while (n--) tot += random();
	work = tot;
#endif
	return abs(work) % nsides + 1;
}

# else

roll(ndie, nsides)
reg int	ndie, nsides; {

	reg int		tot, r;
	reg double	num_sides;

	num_sides = nsides;
	tot = 0;
	while (ndie--)
		tot += (r = rand()) * (num_sides / 017777777777) + 1;
	return tot;
}
# endif
