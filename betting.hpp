#ifndef BETTING_DEFINED
#define BETTING_DEFINED

#include "poker.hpp"

//General functions
int 		isPlayer(P_PLAYER player);
P_PLAYER 	nextPlayer();
P_PLAYER	currentPlayer();
P_PLAYER	moveSmallBlindPlayer();

//entering bets
int 	betRound();
int 	enterBetAI (P_PLAYER player, int minimumBet);
int 	betAI(P_PLAYER player);
int		betPlayer(P_PLAYER player);
void 	processMoneyBet(P_PLAYER player, int bet, int minBet);

//automatic bets
int 	smallBlindBet (P_PLAYER player);
int 	bigBlindBet (P_PLAYER player);

//mins and maxes for betting
int 	minimumBet(P_PLAYER player);
int 	maximumBet(P_PLAYER player);

//misc
void 	processEndOfRound(P_PLAYER winner);
int 	countPlayersInGame();
int 	countLines (const char* s);

#endif
