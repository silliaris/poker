#include <stdio.h>
#include <ctype.h>
#include <iostream>
#include <math.h>

#include "poker.hpp"
#include "betting.hpp"
#include "cardCalculation.hpp"

P_PLAYER currentPlayer() {
	return game.flow.pCurrentPlayer;
}

//remove money from player, add them into common pool, add them into player's bet pool
void processMoneyBet(struct Player* player, int bet, int minBet) {
	
	int realBet = bet + minBet;
	
	if (bet > -1) {
		player->moneyTotal -= realBet;
		player->moneyBet += realBet;

		game.bet.betOnTable = player->moneyBet;
		game.betOnBoard = player->moneyBet;
		game.bet.pot += realBet;
	}
}

int isPlayer(P_PLAYER player) {
	if (player == pPlayers[0])
		return 1;
	return 0;
}

P_PLAYER nextPlayer() {
	int playerNumber = -1;

	//find out player's number in pPlayers array
	for (int i = 0; i < MAX_PLAYERS; i++)
		if (game.flow.pCurrentPlayer == pPlayers[i]) {
			playerNumber = i;
			break;
		}
	if (playerNumber == MAX_PLAYERS - 1) {
		//loop to first player
		game.flow.pCurrentPlayer = pPlayers[0];
	} else {
		game.flow.pCurrentPlayer = pPlayers[playerNumber+1];
	}
	return game.flow.pCurrentPlayer;
}

int enterBetAI (P_PLAYER player, int minimumBet) {

	//int bluffingVal = player->AIAgressivity; //not currently used
	int handValue = calculateHandValue(player);
	int totalBet = game.bet.pot;
	int round = game.flow.round;
	int maxBet = maximumBet(player);
	int foldChance = 0;
	int raiseChance = 0;
	int raiseValue = 0;
	
	//initialize random value
	time_t t;
	srand((unsigned) time(&t));	
	int randomNumber = rand() % 100;
	
	//all in option
	if (maxBet <= minimumBet) {
		player->allIn = 1;
		if (maxBet > minimumBet)
			return maxBet - minimumBet;
		else
			return 0;
	}
		
	if (handValue < 16) {
		foldChance = 10;
		raiseChance = 5;
	} else {
		foldChance = 5;
		raiseChance = 25;
	}
	
	//higher raise change depending on value of hand cards
	if (handValue > 16)
		raiseChance += sqrt(handValue) * 0.5;
	
	//larger the stakes and lesser handValue, the higher is fold chance
	if (minimumBet > 0)
		foldChance += (minimumBet + (totalBet * 0.1)) / (sqrt(handValue) + 5);
	else
		foldChance = 0;

	//debug print
	printf("\nRound %d \nrandomNumber: %d \nhandValue: %d \n%s chances: \nfoldChance %d \nraiseChance %d \n",
		   round, randomNumber, handValue, player->name, foldChance, raiseChance);

	if (randomNumber <= foldChance) {
		return -1;		//fold
	} else if (randomNumber >= 100-raiseChance) {
		//calculation how much to raise
		int raiseRate = randomNumber - (100 - raiseChance);

		//round raise value
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

P_PLAYER moveSmallBlindPlayer() {
	if (game.bet.smallBlindPlayer == nullptr)
		game.bet.smallBlindPlayer = pPlayers[0];
	P_PLAYER smallBlindPlayer = game.bet.smallBlindPlayer;
	
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if (smallBlindPlayer == pPlayers[i]) {
			game.bet.smallBlindPlayer = pPlayers[i+1];
			printf("New small blind player is: %s\n", pPlayers[i+1]->name);
			return pPlayers[i+1];
		}
	}
	return nullptr;
}

int betRound() {
	
	int bet;
	P_PLAYER playerOnTurn = game.flow.pCurrentPlayer;
	if (game.flow.round == 0)
		playerOnTurn = game.bet.smallBlindPlayer;

	//auto-bets
	if (game.flow.round == 0) {
		smallBlindBet(playerOnTurn);
		playerOnTurn = nextPlayer();
		bigBlindBet(playerOnTurn);
		playerOnTurn = nextPlayer();
	}

	//manual bets
	for (int i = 0; i < MAX_PLAYERS; i++) {
		printf("Player on turn: %s - %d\n", playerOnTurn->name, i);

		if (playerOnTurn->isInGame && !playerOnTurn->allIn) {
			if (isPlayer(playerOnTurn))
				bet = betPlayer(playerOnTurn);
			else
				bet = betAI(playerOnTurn);

			switch (bet) {
				case -1: //fold
					playerOnTurn->isInGame = 0;
					break;
				case 0:	//check
					break;
				default: //raise - resatrt bet round
					i = -1;
					break;
			}
		}
		
		playerOnTurn = nextPlayer();
		
		//end betting if other players folded
		if (countPlayersInGame() < 2)
			return 0;
	}
	return 1;
}

int countPlayersInGame() {
	//check if other players folded
	int playersInGame = 0;
	for (int p = 0; p < MAX_PLAYERS; p++)
		if(pPlayers[p]->isInGame)
			playersInGame++;
	return playersInGame;
}

int betAI(P_PLAYER player) {
	int minimum = minimumBet(player);
	int bet = enterBetAI(player, minimum);
	const char* infoText;
	char textBuffer[50];

	processMoneyBet(player, bet, minimum);
	
	switch (bet) {
		case -1:
			sprintf(textBuffer, "%s folded", player->name);
			infoText = textBuffer;
			//infoText = player->name + " folded.";
			break;
		case 0:
			if (minimum > 0)
				//infoText = player->name + " called to " + to_string(player->moneyBet);
				sprintf(textBuffer, "%s called to %d", player->name, player->moneyBet);
			else
				//infoText = player->name + " checked";
				sprintf(textBuffer, "%s checked", player->name);
			infoText = textBuffer;

			break;
		default:
			//infoText = player->name + " has raised by " + to_string(bet) + " to bet of " + to_string(player->moneyBet);
			sprintf(textBuffer, "%s has raised by %d to bet of %d", player->name, bet, player->moneyBet);
			infoText = textBuffer;
			break;
	}
	if (player->allIn) {
		//infoText = player->name + " goes ALL IN with bet of " + to_string(player->moneyBet);
		sprintf(textBuffer, "%s goes ALL IN with bet of %d", player->name, player->moneyBet);
		infoText = textBuffer;
	}

	createScreen(infoText,"","PRESS ENTER TO CONTINUE");
	getchar();
	return bet;
}

int betPlayer(P_PLAYER player) {
	int minimum = minimumBet(player);
	int maximum = maximumBet(player);
	int bet;
	char input[100];
	//int bet = enterBetPlayer(player, minimum);

	createBetScreen(minimum,maximum);

	//wait for valid input
	int validInput = 0;
	while (!validInput) {
		fgets (input, 100, stdin);

		if (isdigit(input[0])) {
			bet = atoi(input);
			if (bet >= minimum && bet <= maximum) {
				validInput = 1;
			}
			else if (bet < minimum && bet > maximum) {
				validInput = 1;
				player->allIn = 1;
			} else {
				wrongInputScreen();
			}			
		} else if (input[0] == 'F' || input[0] == 'f') {
			validInput = 1;
			printf("Fold input entered \n");
			bet = -1;
		} else {
			printf("Wrong input\n");
			wrongInputScreen();
		}
	}
	//getchar();
	
	//find out if raised or fold
	if (bet == minimum)
		bet = 0;
	else if (bet > minimum)
		bet = bet - minimum;

	processMoneyBet(player, bet, minimum);
	printf("BET: %d\n", bet);

	// 	OUTPUTS:
	// 	Folded:	-1
	// 	Called:	 0
	// 	Raised:	>0 - říká, o kolik jsi zvýšil
	return bet;
}

int smallBlindBet (P_PLAYER player) {
	processMoneyBet(player, 5, 0);

	char infoText[32];
	strcpy(infoText, player->name);
	strcat(infoText, " is SMALL BLIND");

	createScreen (infoText, "", "PRESS ENTER TO CONTINUE");
	getchar();
	return 5;
}

int bigBlindBet (P_PLAYER player) {
	processMoneyBet(player, 5, 5);

	char infoText[32];
	strcpy(infoText, player->name);
	strcat(infoText, " is BIG BLIND");

	createScreen (infoText, "", "PRESS ENTER TO CONTINUE");
	getchar();
	return 5;
}

int minimumBet(P_PLAYER player) {
	return game.bet.betOnTable - player->moneyBet;
}

int maximumBet(P_PLAYER player) {
	//you can only rise amount of bet on board - if you have enough money
	if (player->moneyTotal > game.bet.pot)
		return game.bet.pot;
	else 
		return player->moneyTotal;
}

void processEndOfRound(P_PLAYER winner) {
	//zero all bets
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (pPlayers[i] == winner)
			winner->moneyTotal += game.bet.pot;
		pPlayers[i]->moneyBet = 0;
	}

	game.bet.pot = 0;
	game.bet.betOnTable = 0;
	//game.betOnBoard = 0; //OBSOLETE
}
