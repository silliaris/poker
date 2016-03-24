#pragma once
#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <string>
#include <utility>
#include <algorithm>
#include <math.h>
//#include <cstring>

#include "betting.hpp"
#include "poker.cpp"


struct Player*	currentPlayer() {
	return game.flow.pCurrentPlayer;
}

//remove money from player, add them into common pool, add them into player's bet pool
void processMoneyBet(struct Player* player, int bet, int minBet) {
	//potřebuju zjistit, kolik jsem ve skutečnosti vsadil
	
	int realBet = bet + minBet;
	
	if (bet > -1) {
		player->moneyTotal -= realBet;
		player->moneyBet += realBet;

		game.bet.betOnTable = player->moneyBet;
		game.betOnBoard = player->moneyBet;
		game.bet.pot += realBet;
	}
}

int isPlayer (struct Player *player) {
	if (player == pPlayers[0])
		return 1;
	return 0;
}

//this is slightly altered function for screen creation, making composeScreen() obsolete
void createScreen (string infoText, string actionText, string message) {
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

void wrongInputScreen () {
	createScreen(game.flow.infoText,game.flow.actionText,"Wrong input. Try again.");
}

void createBetScreen (int minimumBet, int maximumBet) {
	string limits = "Minimum bet: " + to_string(minimumBet) + ". Maximum bet: " + to_string(maximumBet);
	createScreen("Enter your bet (number) or fold cards (F):",limits,"");
}


struct Player* nextPlayer() {
	int playerNumber = -1;

	//find out player's number in pPlayers array
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (game.flow.pCurrentPlayer == pPlayers[i]) {
			playerNumber = i;
			break;
		}
	}
	
	if (playerNumber < 0) {
		cout << "Debug: Next player not chosen" << endl;
		//This should be ideally handled as exception
	}

	//if this is last player, bring us a first one
	if (playerNumber == MAX_PLAYERS - 1) {
		//loop to first player
		game.flow.pCurrentPlayer = pPlayers[0];
		cout << "Returning to first player" << endl;
	} else {
		game.flow.pCurrentPlayer = pPlayers[playerNumber+1];
	}
	//cout << "Next player: " << game.flow.pCurrentPlayer->name << endl;
	return game.flow.pCurrentPlayer;
}

int enterBetAI (struct Player* player, int minimumBet) {
	//TODO: přidat raise chance a zmenšit fold v závislosti na agresivitě - POZDĚJI

	int bluffingVal = player->AIAgressivity;
	int handValue = calculateHandValue(player);
	int totalBet = game.bet.pot;
	int round = game.flow.round;
	int maxBet = maximumBet();
	int foldChance = 0;
	int raiseChance = 0;
	int raiseValue = 0;
	
	//initialize random value
	time_t t;
	srand((unsigned) time(&t));	
	int randomNumber = rand() % 100;
	
	if (handValue < 16) {
		foldChance = 10;
		raiseChance = 5;
	} else {
		foldChance = 5;
		raiseChance = 25;
	}
	
	//větší raise v závislosti na hodnotě karet:
	if (handValue > 16) {
		raiseChance += sqrt(handValue) * 0.5;
	}	
	
	//čím vyšší stakes a menší handValue, tak tím vyšší fold chance
	if (minimumBet > 0)
		foldChance += (minimumBet + (totalBet * 0.1)) / (sqrt(handValue) + 5);
	else
		foldChance = 0;
	
	cout << endl;
	cout << "Round" << round << endl;
	cout << "Random number: " << randomNumber << endl;
	cout << "handValue: " << handValue << endl;
	cout << player->name << " chances:" << endl
	<< "foldChance " << foldChance << endl 
	<< "raiseChance " << raiseChance << endl;
	
	//výpočet prohození
	
	if (randomNumber <= foldChance) {
		return -1;		//fold
	} else if (randomNumber >= 100-raiseChance) {
		//výpočet, o kolik raisnout
		int raiseRate = randomNumber - (100 - raiseChance);

		//potřebuju raise rate zaokrouhlit na desítky
		raiseValue = (raiseRate / 10);
		raiseValue = minimumBet + (raiseValue * 10);
		
		if (raiseValue > maxBet)
			raiseValue = maxBet;
		
		return raiseValue - minimumBet;
	} 
	else {
		return 0; //called
	}
}

/*
//currently not used
int enterBetPlayer (struct Player* player, int minimumBet) {
	string input;
	string line1;
	int bet;

	cout << "Player: " << player->name << endl; //DEBUG

	//player entering bet
	if (isPlayer(player)) {
		createBetScreen(minimumBet, maximumBet());
		int validInput = 0;
		while (!validInput) {
			cin >> input;
			if (isdigit(input[0])) {
				bet = stoi(input);
				if (bet >= minimumBet && bet <= maximumBet()) {
					cout << "Valid input entered" << endl;
					validInput = 1;
				} else {
					wrongInputScreen();
				}
			} else if (input[0] == 'F' || input[0] == 'f') {
				//valid input -> go to next screen
				validInput = 1;
				cout << "Fold input entered" << endl;
				bet = -1;
			} else {
				cout << "Wrong input" << endl;
				wrongInputScreen();
			}
		}
		//cin.ignore();
	} else {
		//AI calculating bet
		bet = enterBetAI(player, minimumBet);
		if (bet < 0) {
			line1 = player->name + " folded.";
		} else {
			line1 = player->name + " has betted " + to_string(bet);
		}
		createScreen(line1,"","PRESS ENTER TO CONTINUE");
		//cin.ignore();
	}
	cin.ignore();

	processMoneyBet(player, bet, minimumBet);
	cout << "BET: " << bet << endl; //DEBUG print
	
	return bet;
}
*/

/*
void bettingCycle() {
	game.bet.minBet = 10; //na začátku každého cyklu je minBet 10
	game.bet.lastPlayerToRaise = nullptr;
	
	//3x pustím betting round, ale dívám se, jestli mi nevyšla nula
	for (int i = 0; i < 3; i++) {
		//hráč, který začíná bettig round je vždy small blind
		game.flow.pCurrentPlayer = game.bet.smallBlindPlayer; //PRŮSERR: POTŘEBUJU DOSTAT ČÍSLO CURRENTPLAYERA Z POINTERU NA SMALLBETTERA

		if (bettingRound() == 0) {
			break;
		}
	}
	game.flow.pCurrentPlayer = game.bet.smallBlindPlayer; //PRŮSERR: POTŘEBUJU DOSTAT ČÍSLO CURRENTPLAYERA Z POINTERU NA SMALLBETTERA
	return;
}
*/

/*
struct Player* bettingRound() {
	struct Player* curPlayer = currentPlayer();
	int currentBet;
	int minimumBet;
	
	for (int i = 0; i < MAX_PLAYERS; i++) {
		cout << "Player on turn: " << curPlayer->name << endl;
		if (curPlayer->isInGame) {
			currentBet = enterBetPlayer(curPlayer, minimumBet);


			
			if (currentBet >= 0) {
				if (currentBet > game.bet.minBet) {
					//find out if he raised or checked
					game.bet.minBet = currentBet;
					game.bet.lastPlayerToRaise = curPlayer;
				} else if (game.bet.lastPlayerToRaise == curPlayer) {
					//pokud poslední hráč, který zvyšoval, nepřisadil, tak končí celé sázení
					cout << "Moving to anothe bet round." << endl;
					return 0;
				}
			} else {
				//fold cards
				curPlayer->isInGame = 0;
				cout << "Player " << curPlayer->name << " folded" << endl; //debug print
			}
		} else {
			cout << "Player " << curPlayer->name << " is not in game anymore" << endl; //debug print
		}
		cout << "Getting next player" << endl;
		curPlayer = nextPlayer();
	}
	
	cout << "First betting round ended" << endl;
	//return the last player who raised betOnBoard, return current minimum bet
	return curPlayer;
}
*/


