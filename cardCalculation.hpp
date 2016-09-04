#ifndef CARD_CALCULATION
#define CARD_CALCULATION
#include "poker.hpp"
#include "UI.hpp"

#include <stdio.h>
#include <algorithm>
#include <string.h>

#define PAIR_VALUE 100
#define TWO_PAIRS_VALUE 200
#define TRIPLE_VALUE 300
#define STRAIGHT_VALUE 1000
#define FLUSH_VALUE 2000
#define FULL_HOUSE_VALUE 5000
#define QUADRIPLE_VALUE 8000
#define STRAIGHT_FLUSH_VALUE 10000

//-- CARD VALUE CALCULATIONS
int 	calculateHandValue(P_PLAYER player);
void 	handAndBoardCards(P_PLAYER player);

int 	getHighHand(P_PLAYER player);
int 	findPairs(P_PLAYER player);
int 	findFlush(P_PLAYER player);
int 	findStraight (P_PLAYER player);

const char* cardCombinationName(P_PLAYER player);

//-- WINNER CALCULATION
P_PLAYER calculateWinner();
int cmpfunc(const void * a, const void * b);

#endif
