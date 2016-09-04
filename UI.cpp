#include "UI.hpp"
#include "stdlib.h"

//-----------------------------------
// EXTERN VARIABLES
//-----------------------------------
struct UI ui;

const char* faces[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};
const char* basicSpacer = "                    "; // 20 spaces
const char* basicLiner  = "-------------------->   "; // 20+4 letters

//-----------------------------------
// REFACTORED FUNCTIONS
//-----------------------------------

void clearScreen(){
	for(int i = 0; i < 100; i++)
		printf("\n");
}

void linesSeparator(const char* symbol, int linesCount, int linesLength) {
	char str[100] = "";

	if (strlen(symbol) > 1)
		return;

	for (int i = 0; i < linesLength; i++)
		strcat(str, symbol);
	for (int i = 0;i < linesCount;i++)
		printf("%s\n",str);
}

void printScreenHeader() {
	const char* inLineSeparator = "----------";

	linesSeparator("-",2,SCREEN_WIDTH);
	printf("Your bet: %d %s Current Bet: %d %s Pot: %d ----\n",
		   player1.moneyBet, inLineSeparator, game.betOnBoard,
		   inLineSeparator, game.bet.pot);
	linesSeparator("-",1,SCREEN_WIDTH);
}

void printScreenFooter(const char *infoText, const char *actionText, const char *message) {
	linesSeparator("-",2,SCREEN_WIDTH);
	printf("%s%s\n%s%s\n%s%s\n",
		   basicLiner,infoText, basicLiner,
		   actionText, basicLiner, message);
	linesSeparator("-",1,SCREEN_WIDTH);
}

void printEmptyLines(int count) {
	for (int i = 0; i < count; i++)
		printf("\n");
}

int assignBlankSpaces(int occupiedLines) {
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

void introScreen() {
	//clearScreen();
	linesSeparator("-",4,SCREEN_WIDTH);	//empty header

	assignBlankSpaces(1);
	printEmptyLines(ui.blankSpaces[0]);

	printf("%sWELCOME TO THE POKER GAME!", basicSpacer);
	printEmptyLines(ui.blankSpaces[1]);

	printScreenFooter("","Press ENTER to continue.","");

	pressEnterToContinue();
}

void pressEnterToContinue() {
	int enter = 0;
	while (enter != '\r' && enter != '\n')
		enter = getchar();
	printf("Enter entered!");
}

void mainMenu() {
	clearScreen();
	linesSeparator("-",4,SCREEN_WIDTH); //empty header

	assignBlankSpaces(6);
	printEmptyLines(5);
	printEmptyLines(ui.blankSpaces[0]);

	printf("%s(1)  PLAY POKER\n\n%s(2)  CHANGE YOUR NAME\n\n%s(3)  QUIT\n\n",
		   basicSpacer,basicSpacer,basicSpacer);

	printEmptyLines(ui.blankSpaces[1]);
	printScreenFooter("","Enter number:","");


	int input = getchar();
	clearScreen();

	//validate input
	switch (input) {
		case '1': {

			break;
		}
		case '2': {
			printf("Insert new name: \n");

			char name[30];
			getchar();
			fgets (name, 30, stdin);
			printf("%s", name);
			player1.name = name;
			getchar();

			/*
			cout << "Insert new name:" << endl;
			cin >> player1.name;
			cout << "Name succesfully changed!" << endl;
			cin.ignore();
			*/

			clearScreen();
			mainMenu();
			break;
		}
		case '3': {
			exit(EXIT_SUCCESS);
			//abort();
		}
		default: {
			printf("Wrong input!");
			getchar();
			//cout << "Wrong input!";
			//cin.ignore();
			mainMenu();
			break;
		}
	}
}


void wrongInputScreen () {
	createScreen(game.flow.infoText,game.flow.actionText,"Wrong input. Try again.");
}

void createScreen (const char* infoText, const char* actionText, const char* message) {
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

	printScreenFooter(infoText, actionText, message);
}

void createBetScreen (int minimumBet, int maximumBet) {
	char limits[100] = "";

	sprintf(limits, "Minimum bet: %d. Maximum bet: %d", minimumBet, maximumBet);

	createScreen("Enter your bet (number) or fold cards (F):",limits,"");
}

void printHand(P_PLAYER player) {
	printf("Player %s's hand:\n1: %s\n2: %s\n",
		   player->name,player->hand[0].name,player->hand[1].name);
}

void printBoardCards() {
	printf("Cards on the board:\n");
	for(int i=0; i < BOARD_SIZE; i++) {
		printf("%d: %s \n",i+1,game.boardCards[i].name);
	}
}

void resultsScreen(P_PLAYER winner) {
	clearScreen();
	printScreenHeader();
	assignBlankSpaces(11); //11 spaces on screen by default
	printEmptyLines(6);
	printCardTable();
	printEmptyLines(2);
	printBoardCards();
	printEmptyLines(ui.blankSpaces[1]);
	printScreenFooter("Winner is: ", winner->name, "PRESS ANY KEY TO CONTINUE");
}

void printSpaces (int count) {
	for (int i = 0; i < count; i++) {
		printf(" ");
	}
}


void printCardTable() {
	int spacesCount;
	const char* foldedState;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (pPlayers[i]->isInGame)
			foldedState = "In game";
		else
			foldedState = "Folded ";

		printf("%s",foldedState);
		spacesCount = ROW_SIZE + 2 - strlen(foldedState);
		printSpaces(spacesCount);
	}
	printf("\n");

	for (int i = 0; i < MAX_PLAYERS; i++) {
		printf("%s: ",pPlayers[i]->name);
		spacesCount = ROW_SIZE - strlen(pPlayers[i]->name);
		printSpaces(spacesCount);
	}
	printf("\n");

	for (int card = 0; card < 2; card++) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			printf("%s  ",pPlayers[i]->hand[card].name);
			spacesCount = ROW_SIZE - strlen(pPlayers[i]->hand[card].name);
			printSpaces(spacesCount);
		}
		printf("\n");
	}
	printf("\n");

	const char* combination;
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!pPlayers[i]->isInGame)
			combination = " ";
		else
			//combination = cardCombinationName(pPlayers[i]);
			combination = "placeholder combination";

		printf("%s", combination);
		spacesCount = ROW_SIZE + 2 - strlen(combination);

		printSpaces(spacesCount);
	}
	printf("\n");
}


