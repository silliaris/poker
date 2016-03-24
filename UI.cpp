#pragma once
#include "UI.hpp"
#include "poker.cpp"

#include <string>
#include <iostream>


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
	linesSeparator("-",2,SCREEN_WIDTH);
	cout << "---- " <<"Your money: " << player1.moneyTotal << inLineSeparator
	<< "Your bet: " << player1.moneyBet << inLineSeparator 
	<< "Current Bet: " << game.bet.betOnTable << inLineSeparator
	<< "Pot: " << game.bet.pot << " ----" << endl;
	linesSeparator("-",1,SCREEN_WIDTH);
}

void introScreen() {
	linesSeparator("-",4,SCREEN_WIDTH);	//empty header
	
	assignBlankSpaces(1);
	printEmptyLines(ui.blankSpaces[0]);
	cout << basicSpacer <<"WELCOME TO THE POKER GAME!" << endl;
	printEmptyLines(ui.blankSpaces[1]);
	
	printScreenFooter("","Press ENTER to continue.","");
	
	cin.ignore();	
}


void mainMenu() {
	linesSeparator("-",4,SCREEN_WIDTH); //empty header
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

//find out how many spaces to leave for the game
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

/*
void composeScreen (string infoText, string actionText, string message) {
	game.flow.infoText = infoText;
	game.flow.actionText = actionText;
	
	clearScreen();
	printScreenHeader();
	assignBlankSpaces(11); //11 spaces on screen by default
	
	printEmptyLines(ui.blankSpaces[0]);
	printHand(&player1);
	printEmptyLines(2);
	printBoardCards();
	printEmptyLines(ui.blankSpaces[1]);

	printScreenFooter(infoText, actionText, message);
	
	
	string input;
	cin >> input;
	
	flowController(input); //decide which line to print this time
}
*/

void printScreenFooter(string infoText, string actionText, string message) {
	linesSeparator("-",1,SCREEN_WIDTH);
	cout << basicLiner << ">   " << infoText << endl;
	cout << basicLiner << ">   " << actionText << endl;
	cout << basicLiner << ">   " << message << endl;
	linesSeparator("-",1,SCREEN_WIDTH);
}

//-----------------------------------
// SCREEN FUNCTIONS
//-----------------------------------

/*
void showWinnerScreen() {	//OBSOLETED FUNCTOIN!

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
	printEmptyLines(ui.blankSpaces[0]);
		
	printScreenFooter("","Game ended","");
	
	cin.ignore();
	cin.ignore();
}
*/

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

//WINNER SCREEN FUNCTIONS

struct Player* resultsScreen() {
	//winner calculation
	struct Player* winner = calculateWinner();
	//dej celý betOnBoard winnerovi;
	winner->moneyTotal += game.betOnBoard;
	game.betOnBoard = 0;
	
	processEndOfRound(winner);
	
	clearScreen();
	printScreenHeader();
	assignBlankSpaces(11); //11 spaces on screen by default
	printEmptyLines(ui.blankSpaces[0]);
	printCardTable();
	printEmptyLines(2);
	printBoardCards();
	printEmptyLines(ui.blankSpaces[1]);
	printScreenFooter("Winner is: ", winner->name, "PRESS ANY KEY TO CONTINUE");
	
	cin.ignore();
	return winner;
}


void printCardTable() {
	int spacesCount;
	string foldedState;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (pPlayers[i]->isInGame)
			foldedState = "In game";
		else
			foldedState = "Folded ";

		cout << foldedState;
		spacesCount = ROW_SIZE + 2 - stringLength(foldedState);
		printSpaces(spacesCount);
	}
	cout << endl;
	
	//now: FOR NOW ALL PLAYERS WILL BE SHOWN
	for (int i = 0; i < MAX_PLAYERS; i++) {
		cout << pPlayers[i]->name << ": ";
		spacesCount = ROW_SIZE - stringLength(pPlayers[i]->name);
		printSpaces(spacesCount);
	}
	cout << endl;

	for (int card = 0; card < 2; card++) {
		for (int i = 0; i < MAX_PLAYERS; i++) {
			cout << pPlayers[i]->hand[card].name << "  ";
			spacesCount = ROW_SIZE - stringLength(pPlayers[i]->hand[card].name);
			printSpaces(spacesCount);
		}
		cout << endl;
	}
}

int stringLength(string str) {
	int i;
	for (i = 0; i < 100; i++) {
		if (str[i] == '\0' || str[i] == '\n') {
			break;
		}
	}
	return i; //nevím, jestli to bude přesné nebo o 1 menší - breaknutý for loop stejně zvedne count o 1?
}

void printSpaces (int count) {
	string str; 
	for (int i = 0; i < count; i++) {
		cout << " "; //just insert line :)
	}
}


