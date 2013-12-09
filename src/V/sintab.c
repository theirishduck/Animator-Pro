/* sintab.c - 2.14 signed fixed point (1 sign bit, 1 bit left of
   decimal, 14 to right actually) trig table and some associated
   2.14 math routines. */

#include "jimk.h"

static short sin_tab[] =
	{
	     0,    101,    201,    302,    402,    503,    603,    703, 
	   804,    904,   1005,   1105,   1205,   1305,   1406,   1506, 
	  1606,   1706,   1806,   1906,   2006,   2105,   2205,   2304, 
	  2404,   2503,   2603,   2702,   2801,   2900,   2999,   3098, 
	  3196,   3295,   3393,   3491,   3590,   3688,   3786,   3883, 
	  3981,   4078,   4176,   4273,   4370,   4466,   4563,   4660, 
	  4756,   4852,   4948,   5044,   5139,   5235,   5330,   5425, 
	  5519,   5614,   5708,   5802,   5896,   5990,   6083,   6177, 
	  6270,   6362,   6455,   6547,   6639,   6731,   6823,   6914, 
	  7005,   7096,   7186,   7276,   7366,   7456,   7545,   7634, 
	  7723,   7812,   7900,   7988,   8075,   8163,   8250,   8336, 
	  8423,   8509,   8595,   8680,   8765,   8850,   8934,   9018, 
	  9102,   9186,   9269,   9351,   9434,   9516,   9597,   9679, 
	  9760,   9840,   9920,  10000,  10080,  10159,  10237,  10316, 
	 10394,  10471,  10548,  10625,  10701,  10777,  10853,  10928, 
	 11003,  11077,  11151,  11224,  11297,  11370,  11442,  11514, 
	 11585,  11656,  11726,  11796,  11866,  11935,  12004,  12072, 
	 12139,  12207,  12274,  12340,  12406,  12471,  12536,  12601, 
	 12665,  12728,  12791,  12854,  12916,  12978,  13039,  13099, 
	 13159,  13219,  13278,  13337,  13395,  13453,  13510,  13566, 
	 13623,  13678,  13733,  13788,  13842,  13895,  13948,  14001, 
	 14053,  14104,  14155,  14205,  14255,  14305,  14353,  14401, 
	 14449,  14496,  14543,  14589,  14634,  14679,  14724,  14767, 
	 14811,  14853,  14896,  14937,  14978,  15019,  15059,  15098, 
	 15137,  15175,  15212,  15249,  15286,  15322,  15357,  15392, 
	 15426,  15460,  15493,  15525,  15557,  15588,  15619,  15649, 
	 15678,  15707,  15736,  15763,  15790,  15817,  15843,  15868, 
	 15893,  15917,  15940,  15963,  15986,  16007,  16029,  16049, 
	 16069,  16088,  16107,  16125,  16143,  16160,  16176,  16192, 
	 16207,  16221,  16235,  16248,  16261,  16273,  16284,  16295, 
	 16305,  16315,  16324,  16332,  16340,  16347,  16353,  16359, 
	 16364,  16369,  16373,  16376,  16379,  16381,  16383,  16384, 
	 16384, 
	 };

static int s, c;


static
get_sc(theta)
register short theta;
{
theta &=0x3ff;
switch (theta>>8)
	{
	case 0:
		s = sin_tab[theta];
		c = sin_tab[256-theta];
		break;
	case 1:
		s = sin_tab[512-theta];
		c = -sin_tab[theta-256];
		break;
	case 2:
		s = -sin_tab[theta-512];
		c = -sin_tab[3*256-theta];
		break;
	case 3:
		s = -sin_tab[1024-theta];
		c = sin_tab[theta-3*256];
		break;
	}
}

isin(t)
short t;
{
get_sc(t);
return(s);
}

icos(t)
short t;
{
get_sc(t);
return(c);
}

itmult(trig, x)
WORD trig,x;
{
long result;

result = trig;
result *= x;
#ifdef LATER
if (result >= 0)
	result += 1<<13;
else
	result -= 1<<13;
#endif /* LATER */
return(result/(1<<14));
}

polar(theta, rad, xy)
short theta, rad, *xy;
{
get_sc(theta);
xy[0] = itmult(c,rad);
xy[1] = itmult(s,rad);
}

rotate_points(theta, cx, cy, spt, dpt, count)
int theta;
int cx, cy;
WORD *spt, *dpt;
int count;
{
int x,y;

get_sc(theta);
while (--count >= 0)
	{
	x = *spt++ - cx;
	y = *spt++ - cy;
	*dpt++ = itmult(c,x) - itmult(s,y) + cx;
	*dpt++ = itmult(s,x) + itmult(c,y) + cy;
	}
}

