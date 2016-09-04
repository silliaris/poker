#ifndef INIT_GAME_DEFINED
#define INIT_GAME_DEFINED

#include "poker.hpp"

//game init
void setNewTable();
void initializePlayers();

//-- UTILITY FUNCTIONS
void 	nullCardArrays(P_PLAYER player); //is it really needed?

//-- CARD DECK OPERATIONS
void 	shuffleDeck();
void 	createDeck();
void 	createLineup(int faceNumber);

//-- PLAYER CARD OPERATIONS
int 	dealCardsToPlayer(P_PLAYER player);
void 	dealCardToBoard();
void 	createCardDeck();

#endif
