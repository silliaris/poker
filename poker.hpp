//sizes of different arrays (is this really needed?)
#define HAND_AND_BOARD_SIZE 7
#define HAND_SIZE 2
#define BOARD_SIZE 5

#define MAX_PLAYERS 4

//Game flow modes
#define MODE_INTRO 0
#define MODE_MENU 1

#define MODE_DECIDE_ACTION 	10
#define MODE_ENTER_BET 		11
#define MODE_FOLDED 		12
#define MODE_WINNER			13

#define MAX_PHASE 4

struct Cards {
	int 	value;
	int 	color;
	string 	name;
};


struct UI {
	int blankSpaces[2];
} ui;

struct Game 
{
	int 	posOnBoard;
	int 	posInDeck;
	int		betOnBoard;

	struct Cards deck[52], boardCards[5], temp;

	struct GameFlow {
		int phase; // game phase - start, flop, turn, river, finish
		int round; // round - how many games were played already
		//int currentPlayer;
		struct Player* pCurrentPlayer;
		string infoText;
		string actionText;
	} flow;

	struct Beting {
		//money on board
		int	pot; //ALL MONEY IN ROUND
		int	betOnTable; //how high is bet
		int timesRaised;

		int minBet;	//OBSOLETE
		int maxBet;	//OBSOLETE
		int playerOnTurn;
		int bettingRound; //JAK se liší od round?
		struct Player* lastPlayerToRaise;
		struct Player* smallBlindPlayer;
	} bet;
} game;

struct Player {
	int 	id;
	string 	name;	
	int 	moneyTotal;
	int 	moneyBet;
	int 	isInGame;
	int		handValue;
	int		AIAgressivity;
	struct Cards hand[2], handAndBoard[7];
} player1, player2, player3, player4;
struct Player* pPlayers[MAX_PLAYERS] = {&player1, &player2, &player3, &player4};

//extern variables
string faces[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};
extern const string basicSpacer = "                    "; // 20 spaces
extern const string basicLiner  = "--------------------"; // 20 letters

//void flowController(string input);
//void startDecideMode();
//void startBetMode();
void startNextPhase();
int  assignBlankSpaces(int occupiedLines);

//-- MAIN FUNCTIONS
void runPoker();
struct Player* runPokerRound();
int  playAnotherRoundScreen();
void setNewTable();