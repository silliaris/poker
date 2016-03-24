//functs initializing the game

#include "initGame.hpp"

//-----------------------------------
// CARD DECK OPERATIONS
//-----------------------------------

void createCardDeck() {	
	for (int i=0;i<4;i++) {
		createLineup(i);
	}
}

void createLineup(int faceNumber) {
	static int deckPosition;
	string cardfaces[4] = {"Hearts","Diamonds","Clubs","Spades"};

	int i;
	for (i=2;i<=10;i++, deckPosition++) {
		game.deck[deckPosition].value = i;
		game.deck[deckPosition].color = faceNumber;
		game.deck[deckPosition].name = to_string(i) + " " + cardfaces[faceNumber];		
	};
	
	string symbol[4] = { "J", "Q", "K", "A"};
	for (int n=0;n<4;i++, n++, deckPosition++) {
		game.deck[deckPosition].value = i;
		game.deck[deckPosition].color = faceNumber;
		game.deck[deckPosition].name = symbol[n] + " " + cardfaces[faceNumber];
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
		Cards temp = game.deck[rand1];
		game.deck[rand1] = game.deck[rand2];
		game.deck[rand2] = temp;
	}
}

void nullCardArrays(struct Player* player) {
	
	for (int i = 0; i < HAND_AND_BOARD_SIZE; i++) {
		player->handAndBoard[i].color = 0;
		player->handAndBoard[i].value = 0;
	}
	
	for (int i = 0; i < BOARD_SIZE; i++) {
		game.boardCards[i].color = 0;
		game.boardCards[i].value = 0;
	}
}

//-----------------------------------
// PLAYER CARD OPERATIONS
//-----------------------------------

int dealCardsToPlayer (Player* player) {
	//cout << player->name <<"'s cards:" << endl; //TEMP!
	
	for (int i=0; i<2; i++) {
		player->hand[i] = game.deck[game.posInDeck];
		game.posInDeck++;
	}
	return game.posInDeck;
}

void dealCardToBoard() {
	game.boardCards[game.posOnBoard++] = game.deck[game.posInDeck++];
}
