/*
 * Poker game
 */

#include <stdio.h>
#include <ctype.h>
//#include <cstdlib>
//#include <cstring>
#include <iostream>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

//Headers

//-- MAIN FUNCTIONS
void 	runPoker();

//-- SCREEN FUNCTIONS
void 	clearScreen();
void 	printDeck();
void 	printHand(struct Player* player);
void 	printBoardCards();

//-- UTILITY FUNCTIONS
void 	nullCardArrays(struct Player* player); //is it really needed?

//-- CARD DECK OPERATIONS
void 	shuffleDeck();
void 	createDeck();
void 	createLineup(int faceNumber);

//-- PLAYER CARD OPERATIONS
int 	dealCardsToPlayer(struct Player* player);
void 	dealCardToBoard();
void 	createCardDeck();

//-- CARD VALUE CALCULATIONS
int 	calculateHandValue(struct Player* player);
void 	handAndBoardCards(struct Player* player);

int 	getHighHand(struct Player* player);
int 	findPairs(struct Player *player);
int 	findFlush(struct Player* player);
int 	findStraight (struct Player* player);

//-- INTERACTION INTERFACE
void 	linesSeparator(string symbol, int linesCount, int linesLength);
void	pressEnterToContinue();
void	mainMenu();
void	introScreen();
void	printEmptyLines(int linesCount);
void 	printScreenHeader();
void 	printScreenFooter(string infoText, string actionText, string message);

//-- UNDER CONSTRUCTION
void	composeScreen (string infoText, string actionText, string message);

//sizes of different arrays (is this really needed?)
#define HAND_AND_BOARD_SIZE 7
#define HAND_SIZE 2
#define BOARD_SIZE 5

#define MAX_PLAYERS 4

// TODO: Aby výsledné hodnoty zůstaly zachovány,
// tak předělám pouze hodnotu karet ve výpočtech na 2x větší
// ne pouze celý výsledek

#define PAIR_VALUE 100
#define TWO_PAIRS_VALUE 200
#define TRIPLE_VALUE 300
#define STRAIGHT_VALUE 1000
#define FLUSH_VALUE 2000
#define FULL_HOUSE_VALUE 5000
#define QUADRIPLE_VALUE 8000
#define STRAIGHT_FLUSH_VALUE 10000

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
		int mode;  // mode - screen mode that is active
		string infoText;
		string actionText;
	} flow;
	
	struct Beting {
		int minBet;
		int maxBet;
		int playerOnTurn;
		int bettingRound;
	} bet;
} game;

struct Player {
	int 	id;
	string 	name;	
	int 	moneyTotal;
	int 	moneyBet;
	int 	isInGame;
	int		handValue;
	struct Cards hand[2], handAndBoard[7];
} player1, player2, player3, player4, player5;
struct Player* pPlayers[MAX_PLAYERS] = {&player1, &player2, &player3, &player4};

//extern variables
string faces[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};
extern const string basicSpacer = "                    "; // 20 spaces
extern const string basicLiner  = "--------------------"; // 20 letters

void flowController(string input);
void startDecideMode();
void startBetMode();
void startNextPhase();
int	 isPlayerWinner();
int  assignBlankSpaces(int occupiedLines);

//include other files - TBD
//#include "handValue.cpp"
#include "betting.cpp"

//-----------------------------------
// UNDER CONSTRUCTION FUNCTIONS
//-----------------------------------

void showWinnerScreen() {

	clearScreen();
	printScreenHeader();
	assignBlankSpaces(26);
	
	printEmptyLines(ui.blankSpaces[0]);

	//printBoardCards();
	for (int i = 0; i<4; i++) {
		printHand(pPlayers[i]);
		cout << pPlayers[i]->name << "'s card value: " << calculateHandValue(pPlayers[i]) << endl;
	}
	
	//DECIDE WINNER
	printEmptyLines(1);
	if (isPlayerWinner() == 1) {
		cout << "YOU WON!" << endl;
	} else {
		cout << "YOU LOST." << endl;
	}
	printEmptyLines(ui.blankSpaces[0]);
		
	printScreenFooter("","Game ended","");
	
	cin.ignore();
	cin.ignore();
}

int isPlayerWinner() {
	for (int i = 1; i < MAX_PLAYERS; i++) {
		if (pPlayers[0]->handValue <= pPlayers[i]->handValue) {
			if (pPlayers[0]->handValue < pPlayers[i]->handValue) {
				return 0;
			} else {
				return -1;
			}
		}
	}
	return 1;
}

void startNextPhase()
{
	if (game.flow.phase <= MAX_PHASE) {
		game.flow.phase++;
	} else {
		game.flow.phase = 0;
	}

	//DEBUG
	cout << game.flow.phase << endl;
	
	//struct Player *pPlayers[MAX_PLAYERS] = {&player1, &player2, &player3, &player4};
	
	switch (game.flow.phase) {
		case 0:
			for (int i = 0; i < MAX_PLAYERS; i++)
				dealCardsToPlayer(pPlayers[i]);
			break;
		case 1:
			for (int i = 0; i < 3; i++)
				dealCardToBoard();
			break;
		case 2:
		case 3:
			dealCardToBoard();
			break;
		case 4:
			//vyhodnocení - poslední bet (?) - a pak zobrazení výsledků
			cout << "End flow control" << endl;
			showWinnerScreen(); // nebo dostaň hru do fáze MODE_WINNER
			return;
			break;
	}
}

void startDecideMode () {
	game.flow.mode = MODE_DECIDE_ACTION;
	composeScreen("Please decide your action: ","Will you bet or fold (B/F)", "");
}

void startBetMode () {
	game.flow.mode = MODE_ENTER_BET;	
	composeScreen("Enter your bet: ", "Minimum bet: 1, maximum bet: any", "");
}

void startFoldedMode() {
	//pokud hráč nehraje, tak jenom vidí, co se děje.
	composeScreen("", "Press ENTER to continue",""); //TODO: Zatím nefunguje!
	startNextPhase();
}

void flowController (string input) {
	//podle toho, jaký dostane output, tak vytvoří další obrazovku	
	char inputChar = input[0];
	int flowMode = game.flow.mode;
	int bet = 0;

	if (isupper(inputChar))
		inputChar = tolower(inputChar);
	
	switch (flowMode) {
		//intro and menu not implemented yet
		/*
		case MODE_INTRO: {
			break;
		}
		case MODE_MENU: {
			break;
		}
		*/

		case MODE_DECIDE_ACTION:
			// validní inputy jsou: B/F/ - TODO: Do vlastní funkce!
			switch (inputChar) {
				case 'b':
					startBetMode();
					break;
				case 'f':
					// hráč pouze vidí, co dělaejí ostatní hráči, dává ENTER, aby viděl kdo co dělá. TODO
					startFoldedMode();
					break;
				default: 
					cout << "Wrong input." << endl << "Phase: " << game.flow.mode << endl;
					break;
			}
		break;
		case MODE_ENTER_BET: 
			bet = atoi(input.c_str());
			if (bet > 0) {
				//veškeré beting operace musí být opatřeny minimy a maximy, a každý hráč nesmí vsadit víc než teď má
				if (bet >= game.bet.minBet && bet <= game.bet.maxBet && bet <= player1.moneyTotal)	{//TODO: Create "All In" event
				player1.moneyTotal -= bet;
				player1.moneyBet += bet;
				game.betOnBoard += bet;
				}
				if (true) { 	//dostaň hru do další fáze - jakmile všichni dosázeli TODO: Interface pro ostatní hráče
					startNextPhase();
				}
				startDecideMode();
			} else {
				cout << "Wrong input" << endl << "Phase: " << game.flow.mode << endl;
			}
			break;
			
		case MODE_WINNER: 
			//ukaž winner obrazovku
			showWinnerScreen();
			break;
		default: 
			cout << "Error: WRONG MODE" << endl;
			break;
	}
	
	//pokud flowMode zůstal stejný, tak vyhodím error - špatný input
	if (flowMode == game.flow.mode)
		composeScreen(game.flow.infoText,game.flow.actionText, "WRONG INPUT!");
}

int assignBlankSpaces(int occupiedLines) {
	//find out how many spaces to leave for the game
	static int linesTotal = 30;
	int freeLines = linesTotal - occupiedLines;
	
	if (freeLines % 2) {
		ui.blankSpaces[0] = (freeLines - 1) / 2;
		ui.blankSpaces[1] =  (freeLines + 1) / 2;
	} else {
		ui.blankSpaces[0] = ui.blankSpaces[1] = freeLines / 2;
	}
	return linesTotal;
}

void composeScreen (string infoText, string actionText, string message) {
	game.flow.infoText = infoText;
	game.flow.actionText = actionText;
	
	clearScreen();
	printScreenHeader();
	assignBlankSpaces(11); //11 spaces on sceen by default
	
	printEmptyLines(ui.blankSpaces[0]);
	printHand(&player1);
	printEmptyLines(2);
	printBoardCards();
	printEmptyLines(ui.blankSpaces[1]);

	//footer
	linesSeparator("-",1,80);
	cout << basicLiner << ">   " << infoText << endl;
	cout << basicLiner << ">   " << actionText << endl;
	cout << basicLiner << ">   " << message << endl;
	linesSeparator("-",1,80);
	
	string input;
	cin >> input;
	
	flowController(input); //decide which line to print this time
}

void printScreenFooter(string infoText, string actionText, string message) {
	linesSeparator("-",1,80);
	cout << basicLiner << ">   " << infoText << endl;
	cout << basicLiner << ">   " << actionText << endl;
	cout << basicLiner << ">   " << message << endl;
	linesSeparator("-",1,80);
}

/*
 * Musím se rozhodnout, kdy je chvíle, kdy sázím neprodleně (tzn. v době, kdy nemusím zvyšovat)
 * a kdy dávám check/fold/raise.
 * 
 * Když jedu jako první, tak dávám bet
 * Kdykoli dávám bet, tak můžu dát fold
 * 
 * Hra na smallBlind atd.
 * Informace o tom, že je to river, atd.
 * 
 * Můžu nějak zamíchat s pořadím hráčů? - Ne, začáteční negr se posouvá každé kolo - TODO!
 */

//-----------------------------------
// USER INTERFACE
//-----------------------------------

/*
 * Interface design:
 * Jednotlivý části je potřeba mít pořád na stejných mísech
 * Nesmím se roztahovat moc do šířky
 * 
 * 1: Info o penězích
 * 2: Info o současné ruce
 * 3: Info o současném stolu
 * 4: "Rozhodovací část"
 * Různé abstrakce - oddělovače, tabulátory,...
 * ----
 * Funkce:
 * runIntroScreen
 * runIntroScreen
 */

void printEmptyLines (int linesCount) {
	for (int i = 0; i < linesCount; i++)
		cout << endl;
}

void pressEnterToContinue() {
	cout << "Press ENTER to continue:";
	cin.ignore();
	clearScreen();
}

void linesSeparator(string symbol, int linesCount, int linesLength) {

	//prepare line to print
	if (symbol.length() > 1)
		return;

	string str;
	string line;

	for (int i = 0; i < linesLength; i++)
		str.append(symbol);
	for (int i = 0;i < linesCount;i++)
		cout << str << endl;
}

void printScreenHeader()  {
	//string inLineSeparator = " ~~~~~~~~~~ ";
	string inLineSeparator = " ---------- ";
	//player1.moneyBet = 0;	
	
	//header of the screen - shoud be always present
	linesSeparator("-",2,80);
	cout << "---- " <<"Your money: " << player1.moneyTotal << inLineSeparator
	<< "Your bet: " << player1.moneyBet << inLineSeparator 
	<< "Total Bet: " << game.betOnBoard << " ----" << endl;
	linesSeparator("-",1,80);
}

void introScreen() {
	linesSeparator("-",4,80);	//empty header
	
	assignBlankSpaces(1);
	printEmptyLines(ui.blankSpaces[0]);
	cout << basicSpacer <<"WELCOME TO THE POKER GAME!" << endl;
	printEmptyLines(ui.blankSpaces[1]);
	
	printScreenFooter("","Press ENTER to continue.","");
	
	cin.ignore();	
}


void mainMenu() {
	linesSeparator("-",4,80); //empty header
	assignBlankSpaces(5);
	printEmptyLines(ui.blankSpaces[0]);

	cout << basicSpacer <<"(1)  PLAY POKER" << endl
	<< endl 
	<< basicSpacer << "(2)  Change your name   (Current name: " << player1.name << ")" << endl 
	<< endl 
	<< basicSpacer << "(0)  Quit" << endl;

	printEmptyLines(ui.blankSpaces[1]);
	printScreenFooter("","Enter number:","");
	
	string input;
	cin >> input;

	clearScreen();
	switch (input[0]) {
		case '1': {
			return;
			break;
		}
		case '2': {
			cout << "Insert new name:" << endl;
			cin >> player1.name;
			cout << "Name succesfully changed!" << endl;
			cin.ignore();
			clearScreen();
			mainMenu();
			break;
		}
		case '0': {
			exit(EXIT_SUCCESS);
			//abort();
		}
		default: {
			cout << "Wrong input!";
			cin.ignore();
			mainMenu();
			break;
		}
	}
}

/*
 * Round:
 * Jednotný interface, který ohraničuje karty:
 * První řádky: Množství peněz
 * Bet na stole
 * 
 * 
 */


//-----------------------------------
// CARD VALUE CALCULATIONS
//-----------------------------------

//main function for comparations
int calculateHandValue(struct Player* player) {

	//prepare numbers for calculating
	handAndBoardCards(player); 

	int cardValue = 0;
	cardValue = findPairs(player) + findFlush(player) + findStraight(player);
	cardValue = cardValue * 2 + getHighHand(player); //get cards from hand, which has most minor value from all cards

	//DEBUG PRINT - this should not be present in final version!
	cout << player->name << " Pairs: " << findPairs(player) << " Flush: " 
	<< findFlush(player) << " findStraight: " << findStraight(player) 
	<< " High hand: " << getHighHand(player) <<endl;

	cout << "Card value: " << cardValue << endl;
	player->handValue = cardValue;
	return cardValue;
};

int getHighHand(struct Player* player) {
	int highHand = 0;
	for (int i = 0; i<7; i++) {
		if (player->handAndBoard[i].value > highHand)
			highHand = player->handAndBoard[i].value;
	}
	return highHand;
}

int findFlush(struct Player* player) {
	//zjisti pro každou barvu počet karet - pokud jich je pět, tak je to flush
	extern string faces[4];
	int sameFaces[4] = {0,0,0,0};
	int flushValue = 0;
	
	for (int i = 0; i<4; i++) {
		for (int ii = 0; ii<7; ii++) {
			if (player->handAndBoard[ii].color == i && player->handAndBoard[ii].value != 0)
				sameFaces[i]++;
		}
		//cout << sameFaces[i] << endl; //DEBUG
		if (sameFaces[i] >= 5)
			flushValue = FLUSH_VALUE;
	}
	
	return flushValue;
}

int findStraight (struct Player* player) {

	//sort cards
	int sortedValues[HAND_AND_BOARD_SIZE + 1];
	
	for (int i = 0; i<HAND_AND_BOARD_SIZE; i++)
		sortedValues[i] = player->handAndBoard[i].value;
	sort(sortedValues, sortedValues + HAND_AND_BOARD_SIZE);
	sortedValues[HAND_AND_BOARD_SIZE] = 0; //variable compared after last card

	/*
	cout << "Sorted cards: " << endl;					//DEBUG
	for (int i = 0; i< HAND_AND_BOARD_SIZE + 1; i++)		
		cout << sortedValues[i] << endl;
	cout << endl << endl;
	*/

	//find strings of similar values
	int highestCard;
	int combo = 0;
	int streak = 0;
	
	for (int i = 0; i<7; i++) {
		switch (sortedValues[i+1] - sortedValues[i]) {
			case 0: {		//value is the same - continue in loop, just don't increment
				break;
			}
			case 1: { 		// value is higher by 1 - increment by 1
				combo++;
				break;				
			}
			default: {		//end counting and overwrite value combo to get highest value, with highest card
				if (combo > streak) {
					streak = combo + 1;
					combo = 0;
					highestCard = sortedValues[i];
				}
				break;
			}
		}
	}
	
	//find out if it's flush
	int cardsPositions[5];
	int n = 0;
	//find positions of cards that are making straight
	for (int ii = highestCard - 4; ii <= highestCard; ii++) {
		for (int i = 0; i < HAND_AND_BOARD_SIZE; i++)
			if (player->handAndBoard[i].value == ii) {				
				cardsPositions[n] = i;
				n++;
			}
	}

	int isFlush;
	for (int i = 0; i < 5; i++) {
		if (player->handAndBoard[cardsPositions[i]].color != player->handAndBoard[cardsPositions[0]].color) {
			isFlush = 0;
			break;
		}
	}
	isFlush = 1;
	
	//DEBUG - print of card positions
	/*for (int i = 0; i<5; i++)
		cout << "Cards positions: " << cardsPositions[i] << endl;
	*/
	
	//Assign return value
	int returnVal = 0;
	if (streak >= 5) {
		returnVal = STRAIGHT_VALUE;

		if (isFlush == 1) {
			//cout << "Straight flush!" << endl;
			returnVal = STRAIGHT_FLUSH_VALUE;
		}
	}
	
	//Low ace straight
	if (streak == 4 && highestCard - 4 == 2 && sortedValues[HAND_AND_BOARD_SIZE] == 14) {
		//cout << "Straight from Ace! HighestCard: " << streak << endl;
		returnVal = STRAIGHT_VALUE;
	}
	
	if (returnVal) 
		returnVal += highestCard;

	return returnVal;
}

/* 
 * Přiřaď hodnotu ke každé kombinaci. Musím si udělat na kombinace tabulku, která bude říkat, co má jakou honotu. 
 * 
 * High hand: 	0-14
 * Pair: 		100
 * Two pairs:	200
 * 3 of a kind: 300
 * Straight: 	1000 (měřit nejnižší kartu)
 * Flush: 		2000
 * Full house:	5000
 * 4 of a kind: 8000
 * S. flush:	10 000
 * Royal flush: 10 014
 */

void handAndBoardCards(struct Player* player) {
		int i = 0;
	for (; i<2; i++)
		player->handAndBoard[i] = player->hand[i];
	
	for (int j = 0; !game.boardCards[j].name.empty() && j<5;i++, j++)
		player->handAndBoard[i] = game.boardCards[j];

	//DEBUG
	/*
	cout << "Running handAndBoardCards():" << endl;
	for (i = 0; !player->handAndBoard[i].name.empty() && i<7; i++)
		cout << player->handAndBoard[i].name << endl;
	*/
}

//porovnej kartu se zbytkem ruky
int findPairs(struct Player *player) {

	int sameValues[7];
	int cardValue[7];
	for (int i = 0; i<7;i++) {
		sameValues[i] = 0;
		cardValue[i] = 0;
	}

	for (int j = 0; j < 7 && player->handAndBoard[j].value != 0; j++) {
		
		int currentCardVal = player->handAndBoard[j].value;
		
		for (int i = 0; i < 7; i++) {
			
			if (currentCardVal == player->handAndBoard[i].value) {
				sameValues[j]++;
				cardValue[j] = player->handAndBoard[j].value;
			}
		}
	}
	
	int combinationValue = 0;
	int combined = 0;
	for (int i = 0; i<7;i++) {
		switch (sameValues[i]) {
			case 1: break;
			case 2: {
				combinationValue += PAIR_VALUE + cardValue[i];
				break;
			}
			case 3: {
				combinationValue += PAIR_VALUE + cardValue[i];
				break;
			}
			case 4: {
				combinationValue += PAIR_VALUE + cardValue[i];
			}
		}
	}
	return combinationValue;
}

//-----------------------------------
// SCREEN FUNCTIONS
//-----------------------------------

//manual definition of to_string funct
std::string to_string(int i){
    std::stringstream ss;
    ss << i;
    return ss.str();
}

void clearScreen(){
	cout << string( 100, '\n' );
}

void printDeck() {
	int i = 0;
	while (!game.deck[i].name.empty()) 
		cout << game.deck[i++].name << endl;
}

void printBoardCards() {
	cout << "Cards on the board: " << endl;
	for(int i=0; i < BOARD_SIZE; i++)
		cout << i+1 << ": " << game.boardCards[i].name << endl;
}

void printHand(struct Player* player) {
	
	cout << "Player " << player->name << "'s hand:" << endl
	<< "1: " << player->hand[0].name << endl
	<< "2: " << player->hand[1].name << endl;
}

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

void shuffleDeck () {
	time_t t;
	srand((unsigned) time(&t));	
	
	for (int i = 10000; i != 0; i--) {
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

/*
 * requirements:
 * board card positions
 * deckCardPosition
 */

/*
 * ARCHITEKTURE DEALOVÁNÍ: 
 * Sázení - (na začátku pouze 1 round)
 * --- nabídka položit karty/vsadit (v prvním kole opačně)
 * --- nabídka sázení: (množství peněz, výše sázky, strop sázky)
 */

/* AI fáze - requirements
 * Průběžné počítání pravděpodobnosti výhry
 * Nastavení levelu agresivity - blufování
 * Podle výpočtu šance x agresivity vypočítat, kolik AI vsadí
 * 
 * FÁZE: 
 * 1. AI ví, kdy dát check a kdy pass
 * 2. AI umí bluffovat
 * 3. AI umí nastavit výšku sázky
 */

/*
 * Každý hráč by měl mít funkce:
 * - ukaž obsah ruky
 * - vsaď
 * - fold/raise/call
 */

//main fnc
void runPoker() {

	//initialize basic variables and structures
	string playerName = "Tarantule";	
	createCardDeck();
	shuffleDeck();
	//printDeck();	
	game.posInDeck = 0;
	game.posOnBoard = 0;
	game.flow.phase = 0;
	game.bet.minBet = 10;
	game.bet.maxBet = 100;
	//struct Player *pPlayers[MAX_PLAYERS] = {&player1, &player2, &player3, &player4};
	string names[MAX_PLAYERS] = {
		playerName,
		"Ugly Bot",
		"Nervous Bot",
		//"Alien Bot",
		"All Stars Bot"
	};
	for (int i = 0; i<MAX_PLAYERS; i++) {
		pPlayers[i]->name = names[i];
		pPlayers[i]->moneyTotal = 500;
		nullCardArrays(pPlayers[i]);
		pPlayers[i]->isInGame = 1;
	}
	
	//initialize game
	clearScreen();
	introScreen();
	clearScreen();
	
	mainMenu();
	clearScreen();

	//deal cards to players
	for (int i = 0; i<MAX_PLAYERS; i++) {
		dealCardsToPlayer(pPlayers[i]);
		//printHand(pPlayers[i]);
		//calculateHandValue(pPlayers[i]);
	}

	//hacking start of the game - now it should be in loop
	//game.flow.phase = 1;
	startDecideMode();

	
	//victory screen: 
	
	//deal first 3 cards on the board:
	for(int i=0; i<3;i++)
		dealCardToBoard();
	printBoardCards();
	
	clearScreen();
	//runRound(2);
	cin.ignore();

	dealCardToBoard();

	clearScreen();
	//runRound(3);
	cin.ignore();

	dealCardToBoard();

	clearScreen();
	//runRound(1);
	cin.ignore();

	pressEnterToContinue();
	pressEnterToContinue();
	pressEnterToContinue();

	/*
	for (int i = 0; i<MAX_PLAYERS; i++) {
		dealCardsToPlayer(pPlayers[i]);
		//printHand(pPlayers[i]);
		//calculateHandValue(pPlayers[i]);
	}
	*/

	/*
	
	cout << "----------------" << endl;
	cout << calculateHandValue(pPlayers[0]) << endl;

	//second bets - PLACEHOLDERS
	pressEnterToContinue();

	printHand(&player1);
	dealCardToBoard();
	printBoardCards();

	//last bets - PLACEHOLDERS
	pressEnterToContinue();
	
	printHand(&player1);
	dealCardToBoard();
	printBoardCards();
	
	//calculateHandValue(&player1);
	//calculateHandValue(&player2);
	//calculateHandValue(&player3);
	*/

	pressEnterToContinue();

	printBoardCards();
	for (int i = 0; i<4; i++) {
		printHand(pPlayers[i]);
		cout << pPlayers[i]->name << "'s card value: " << calculateHandValue(pPlayers[i]) << endl;
	}
	
	//DECIDE WINNER
	if (isPlayerWinner()) {
		cout << "You are winner!" << endl;
	} else {
		cout << "You lose." << endl;
	}
}
