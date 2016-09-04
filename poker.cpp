#include "poker.hpp"
#include "UI.hpp"

#include <stdio.h>
#include <algorithm>
#include <string.h>

#include "betting.hpp"
#include "cardCalculation.hpp"
#include "initGame.hpp"

//declare basic structures
struct Game game;
PLAYER player1, player2, player3, player4;
P_PLAYER pPlayers[MAX_PLAYERS] = {&player1, &player2, &player3, &player4};
struct Beting bet;

void runPoker() {

	//initialize game
	initializePlayers();
	createCardDeck();
	clearScreen();

	//create intro
	introScreen();
	clearScreen();

	mainMenu();
	clearScreen();

	//Player can play as long as he want
	int newRound = 1;
	while (newRound) {
		printf("Setting up new table \n");
		setNewTable();
		runPokerRound();
		newRound = playAnotherRoundScreen();
	}

	clearScreen();
	runPoker(); //Function is going to recursion
}

P_PLAYER runPokerRound() {
	//TODO: Make sure who should be going first
	
	const char* roundMessages[5] = {
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
		
		//create round info screen
		createScreen(roundMessages[round],"","PRESS ENTER TO CONTINUE");
		getchar();

		if (round == 0)
			getchar();

		printf("Round %d \n", round);

		//run betting for the round and detect
		//if anyone won by other players folded
		if (round < 4) {
			gameRunning = betRound();
			if (!gameRunning)
				break;
		}
	}

	//calculate and show winner
	P_PLAYER winner = calculateWinner();
	processEndOfRound(winner);

	if (pPlayers[0]->moneyTotal > 0)
		resultsScreen(winner);
	else {
		createScreen ("YOU LOST.", "", "PRESS ANY GAME TO CONTINUE");
		getchar();
		runPoker();
	}
	return winner;
}

int playAnotherRoundScreen() {
	int validInput = 0;
	int input;
	
	while (!validInput) {
		createScreen ("Play another round?", "", "YES (Y) x NO (N)");

		input = getchar();
		//cin >> input;
		//char firstChar = toupper(input[0]);
		
		if (input == 'Y' || input == 'N') {
			validInput = 1;
			if (input == 'Y') {
				return 1;
				//start another round
			}
		} else {
			wrongInputScreen();
		}
	}
	return 0;
}
