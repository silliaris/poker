#include "initGame.hpp"
#include "betting.hpp"

#include <iostream>
#include <string.h>

//-----------------------------------
// GAME INIT
//-----------------------------------

void setNewTable() {

	shuffleDeck();

	game.posInDeck = 0;
	game.posOnBoard = 0;
	game.bet.smallBlindPlayer = moveSmallBlindPlayer();
	game.flow.pCurrentPlayer = game.bet.smallBlindPlayer;
	
	for (int i = 0; i<MAX_PLAYERS; i++) {
		pPlayers[i]->moneyBet = 0;
		nullCardArrays(pPlayers[i]);
		pPlayers[i]->isInGame = 1;
		pPlayers[i]->handValue = 0;
		pPlayers[i]->allIn = 0;

		//zero hand and board for each players
		pPlayers[i]->hand[0] = {0,0,""};
		pPlayers[i]->hand[1] = {0,0,""};
		for (int card = 0; card<HAND_AND_BOARD_SIZE; card++)
			pPlayers[i]->hand[card] = {0,0,""};
	}
}

void initializePlayers() {
	const char* playerName = "Tarantule";
	const char* names[MAX_PLAYERS] = {
		playerName,
		"Ugly Bot",
		"Nervous Bot",
		"All Stars Bot"
	};

	for (int i = 0; i<MAX_PLAYERS; i++) {
		pPlayers[i]->name = names[i];
		pPlayers[i]->AIAgressivity = 5; //value 0-10
		pPlayers[i]->moneyTotal = 500;

		nullCardArrays(pPlayers[i]);
		pPlayers[i]->isInGame = 1;
	}
}

//-----------------------------------
// CARD DECK OPERATIONS
//-----------------------------------

void createCardDeck() {	
	for (int i=0;i<4;i++) {
		createLineup(i);
	}

	for (int i = 0; i < 52; i++)
		printf("%d: %s \n", i, game.deck[i].name);
}

void createLineup(int faceNumber) {
	static int deckPosition;
	const char* cardfaces[4] = {"Hearts","Diamonds","Clubs","Spades"};
	static char nameBuffer[100][20];
	int i;

	//create cards 2-10
	for (i=2;i<=10;i++, deckPosition++) {
		game.deck[deckPosition].value = i;
		game.deck[deckPosition].color = faceNumber;

		sprintf(nameBuffer[deckPosition],"%d %s", i, cardfaces[faceNumber]);
		game.deck[deckPosition].name = nameBuffer[deckPosition];

		printf("%s\n",game.deck[deckPosition].name);
	};

	//create J,Q,K,A
	const char* symbol[4] = { "J", "Q", "K", "A"};
	for (int n=0;n<4;i++, n++, deckPosition++) {
		game.deck[deckPosition].value = i;
		game.deck[deckPosition].color = faceNumber;

		sprintf(nameBuffer[deckPosition],"%s %s", symbol[n], cardfaces[faceNumber]);
		game.deck[deckPosition].name = nameBuffer[deckPosition];
		printf("%s\n", game.deck[deckPosition].name);
	}
}

void shuffleDeck() {
	time_t t;
	srand((unsigned) time(&t));	
	
	int randCounter = rand() % 19000;
	
	for (int i = 1000 + randCounter; i != 0; i--) {
		int rand1 = rand() % 52;
		int rand2 = rand() % 52;
		
		//the most primitive version of randomization: Swapping 2 elements in a struct - not done with pointers
		Card temp = game.deck[rand1];
		game.deck[rand1] = game.deck[rand2];
		game.deck[rand2] = temp;
	}
}

void nullCardArrays(P_PLAYER player) {
	for (int i = 0; i < HAND_AND_BOARD_SIZE; i++)
		player->handAndBoard[i] = {0,0,""};
	for (int i = 0; i < BOARD_SIZE; i++)
		game.boardCards[i] = {0,0,""};
}

//-----------------------------------
// PLAYER CARD OPERATIONS
//-----------------------------------

int dealCardsToPlayer (P_PLAYER player) {
	printf("Printing player cards \n");
	printf("%s cards:\n", player->name);
	

	//TODO: something is not working in here
	for (int i=0; i<2; i++) {
		player->hand[i] = game.deck[game.posInDeck];
		printf("%s\n",player->hand[i].name);
		game.posInDeck++;
	}
	return game.posInDeck;
}

void dealCardToBoard() {
	game.boardCards[game.posOnBoard++] = game.deck[game.posInDeck++];
}
