#pragma once
using namespace std;

//každé .hpp by to mělo mít svoji sadu includů
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <utility>
#include <algorithm>

//Headers
#include "poker.hpp"
#include "UI.hpp"
#include "betting.hpp"
#include "cardCalculation.hpp"
#include "initGame.hpp"
#include "betting_experiments.hpp"

//should not be here in the end
#include "UI.cpp"
#include "betting.cpp"
#include "cardCalculation.cpp"
#include "initGame.cpp"
#include "betting_experiments.cpp"

//tohle je jeden celý round
struct Player* runPokerRound() {
	//TODO: zohlednit, který hráč jede jako první
	
	string roundMessages[5] = {
		"STARTING GAME",
		"FLOP ROUND",
		"TURN ROUND",
		"RIVER ROUND",
		"RESULTS"
	};
	int gameRunning;
	
		//this is betting cycle for one round
	for (game.flow.round = 0; game.flow.round < 5; game.flow.round++) {
		int round = game.flow.round;
		switch (round) {
			case 0: 
				//deal cards to all players
				for (int i = 0; i<MAX_PLAYERS; i++)
					dealCardsToPlayer(pPlayers[i]);
				break;
			case 1: 
				for(int i=0; i<3;i++)
					dealCardToBoard();
				break;
			case 2:
			case 3:
				dealCardToBoard();
				break;

			default: 
				//not adding any cards
				break;
		}
		createScreen(roundMessages[round],"","PRESS ENTER TO CONTINUE");
		cin.ignore();
		cin.ignore();

		//if (round < 2)
			//cin.ignore();	//this is some very weird hack - don't know how this works right now
		//if (round != 5)

		cout << "Round " << round << endl;
		gameRunning = betRound();
		if (!gameRunning)
			break;
	}

	//show results of a round
	struct Player* winner = resultsScreen();
	return winner;
}

void setupNextRound() {
	//TODO: posuň small blind a current playera
}

int playAnotherRoundScreen() {
	int validInput = 0;
	string input = "";
	
	while (!validInput) {
		createScreen ("Play another round?", "", "YES (Y) x NO (N)");
		cin >> input;
		input = toupper(input[0]);
		
		if (input == "Y" || input == "N") {
			validInput = 1;
			if (input == "Y") {
				return 1;
				//start another round				
			} else {
				//get into main menu
				return 0;
			}
		} else {
			wrongInputScreen();
		}
	}	
}
		
void setNewTable() {
	/*
	 * Nový stůl
	 * 	- zamíchat karty
	 *  - odstranit všechny bety na stole
	 *  - odstranit všechny karty ze stolu
	 *  - začít z nového balíčku
	 *  - všichni hráči jsou opět ve hře
	 */

	shuffleDeck();

	game.posInDeck = 0;
	game.posOnBoard = 0;
	game.flow.phase = 0;
	game.bet.smallBlindPlayer = pPlayers[0]; //TODO: zvolit správný small blind
	game.flow.pCurrentPlayer = pPlayers[0]; //TODO: zvolit správného currentPlayera
	
	for (int i = 0; i<MAX_PLAYERS; i++) {
		pPlayers[i]->moneyBet = 0;
		nullCardArrays(pPlayers[i]);
		pPlayers[i]->isInGame = 1;
		pPlayers[i]->handValue = 0;

		//delete hand and board for each players
		pPlayers[i]->hand[0] = {0,0,""};
		pPlayers[i]->hand[1] = {0,0,""};
		for (int card = 0; card<HAND_AND_BOARD_SIZE; card++)
			pPlayers[i]->hand[card] = {0,0,""};		
	}
	for (int i = 0; i < BOARD_SIZE; i++)
		game.boardCards[i] = {0,0,""};
}

void runPoker() {
	//initialize basic variables and structures
	
	string playerName = "Tarantule";
	createCardDeck();
	shuffleDeck();

	game.posInDeck = 0;
	game.posOnBoard = 0;
	game.flow.phase = 0;
	game.bet.minBet = 10;
	game.bet.maxBet = 100;
	game.bet.smallBlindPlayer = pPlayers[0];
	game.flow.pCurrentPlayer = pPlayers[0];

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
		pPlayers[i]->AIAgressivity = 5; //value 0-10
	}
	
	//initialize game
	clearScreen();
	introScreen();
	clearScreen();
	
	mainMenu();
	clearScreen();
	

	//TEMP - hráč může hrát tak dlouho jak chce
	int newRound;
	while (true) {
		setNewTable();
		runPokerRound();

		moveSmallBlindPlayer();
				
		newRound = playAnotherRoundScreen();
		if (!newRound)
			break;
	}

	//if you break from loop, return to main menu
	clearScreen();
	//mainMenu();
	runPoker(); //TEMP -going directly into recursion	
}
