#ifndef _PATTERN_DYN_
#define _PATTERN_DYN_
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <set>
#include "board.h"

#define MAXDIAMETER 7
#define MAXPATTERN 100

using namespace std;

static int totalPattern = 0;


enum stoneType{ //type of stones in pattern
	empty = 0,
	white,
	black,
	border,
	notWhite,
	notBlack,
	arbitrary
};

enum patternType{ //type of patterns
	null = 0, //not a pattern
	hane,
	cut1,
	cut2
};

struct typeTrans{
	int x, y;
	int pat;
	int transx, transy;
	int ex;
	int clock;
};

bool onBoard(int centerx, int centery){//center:1-13
	return ((centerx>=0) && (centery>=0) && (centerx<=14) && (centery<=14));
}
//4 types of transformations, size:3-5
void exchangeDyn(int orig[], int size);
void clockwiseDyn(int orig[], int size);
void symmetryXDyn(int orig[], int size);
void symmetryYDyn(int orig[], int size);

bool samePattern(int x[], int existed[], int size, typeTrans *tt); 
//if is the same pattern, it will modify the type of x

bool equalStoneType(int x, int pat);

int matchPatternDyn(board *brd, int color, int x, int y, int size, int radius, typeTrans tt[]);

#endif