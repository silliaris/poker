#ifndef POKER_DEFINED
#define POKER_DEFINED

//DEFINES
#define HAND_AND_BOARD_SIZE 7
#define HAND_SIZE 2
#define BOARD_SIZE 5
#define MAX_PLAYERS 4

#define MODE_INTRO 0
#define MODE_MENU 1

#define MODE_DECIDE_ACTION 	10
#define MODE_ENTER_BET 		11
#define MODE_FOLDED 		12
#define MODE_WINNER			13

#define MAX_PHASE 4

typedef struct Card {
	int			value;
	int			color;
	const char* name;
} CARD, *P_CARD;

typedef struct Player {
	const char* name;
	int         moneyTotal;
	int         moneyBet;
	int         handValue;
	int         isInGame;
	int         AIAgressivity;
	int         allIn;
	const char* cardCombination;
	CARD        hand[2];
	CARD        handAndBoard[7];
} PLAYER, *P_PLAYER;

typedef struct GameFlow {
	int			round; //betting round
	P_PLAYER	pCurrentPlayer;
	const char* infoText;
	const char* actionText;
} FLOW, *P_FLOW;

typedef struct Beting {
	//money on board
	int			pot; //ALL MONEY IN ROUND
	int			betOnTable; //how high is bet
	P_PLAYER	smallBlindPlayer;
} BET, *P_BET;

struct Game {
	int 	posOnBoard;
	int 	posInDeck;
	int		betOnBoard;
	CARD	deck[52];
	CARD	boardCards[5];
	CARD	temp;
	FLOW	flow;
	BET		bet;
};

//extern variables
extern struct Game game;
extern PLAYER player1, player2, player3, player4;
extern P_PLAYER pPlayers[MAX_PLAYERS];
extern struct Beting bet;

//functions
int         assignBlankSpaces(int occupiedLines);
void        runPoker();
int			playAnotherRoundScreen();
P_PLAYER	runPokerRound();

#endif
