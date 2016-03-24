//betting_experiments.cpp
//#pragma once
#include "betting_experiments.hpp"
#include "poker.cpp"

struct Player* moveSmallBlindPlayer() {
	struct Player* smallBlindPlayer = game.bet.smallBlindPlayer;
	
	for(int i = 0; i < MAX_PLAYERS; i++)
		if (&smallBlindPlayer == &pPlayers[i]) {
			game.bet.smallBlindPlayer = pPlayers[i+1];
			cout << "New small blind is: " << pPlayers[i+1]->name << endl;
			return pPlayers[i+1];
		}
}

int betRound() {
	//funkce: restartuj bet round pokaždé, když někdo zvýší
	//pokud je první kolo, 
	//tak platí speciální pravidla: 
	//automatické sázky + small blind dorovnává poslední
	
	struct Player* playerOnTurn = game.bet.smallBlindPlayer;
	int bet;
	int betRaised = 0;
	int smallBlindTurn = 1;

	for (int i = 0; i < MAX_PLAYERS; i++) {
		
		//restarting betting round
		if (betRaised > 0) {
			betRaised = 0;
			i = 0;
		}
		
		if (playerOnTurn->isInGame) {	
			//automatic bets
			if (game.flow.round == 0 && i <= 1 && betRaised == 0) {
				if (i == 0)
					bet = smallBlindBet(playerOnTurn);
				else if (i == 1)
					bet = bigBlindBet(playerOnTurn);
			} else {
				//non-automatic bet
				if (isPlayer(playerOnTurn))
					bet = betPlayer(playerOnTurn);
				else
					bet = betAI(playerOnTurn);
				if (bet < 0)
					playerOnTurn->isInGame = 0;
				
				if (bet > 0) {
					i--;
					betRaised++;
					cout << playerOnTurn->name <<" raised by" << bet << endl;
				}
			}
			
			playerOnTurn = nextPlayer();
			
			//return back to small blind in first round
			if (i == MAX_PLAYERS - 1 && !betRaised && smallBlindTurn) {
				i--;
				smallBlindTurn = 0;
			}
			
			//check if other players folded
			int playersInGame = 0;
			for (int p = 0; p < MAX_PLAYERS; p++)
				if(pPlayers[p]->isInGame)
					playersInGame++;
			if (playersInGame < 2)
				return 0;
		}
	}
	return 1;
}

/*
int enterBet(struct Player* player) {
	int minimum = minimumBet(player);
	int maximum = maximumBet();
	int bet;

	//send it to players
	if (isPlayer(player))
		bet = betPlayer(player, minimum);
	else
		bet = betAI(player, minimum);
	
	return bet;
};
*/

int betAI(struct Player* player) {
	int minimum = minimumBet(player);
	int bet = enterBetAI(player, minimum);
	string infoText;

	processMoneyBet(player, bet, minimum);
	
	switch (bet) {
		case -1:
			infoText = player->name + " folded.";
			break;
		case 0:
			if (minimum > 0)
				infoText = player->name + " called to " + to_string(player->moneyBet);
			else
				infoText = player->name + " checked";
			break;
		default:
			infoText = player->name + " has raised by " + to_string(bet);
			break;
	}
	
	
	createScreen(infoText,"","PRESS ENTER TO CONTINUE");
	cin.ignore();	
	return bet;
}

int betPlayer(struct Player* player) {
	int minimum = minimumBet(player);
	int maximum = maximumBet();
	int bet;
	string input;
	//int bet = enterBetPlayer(player, minimum);

	createBetScreen(minimum,maximum);

	//wait for valid input
	int validInput = 0;
	while (!validInput) {
		cin >> input;
		if (isdigit(input[0])) {
			bet = stoi(input);
			if (bet >= minimum && bet <= maximum)
				validInput = 1;
			else
				wrongInputScreen();
			
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
	
	//potřebuju zjistit, jestli jsem raisnul, nebo calloval.
	if (bet == minimum)
		bet = 0;
	else if (bet > minimum)
		bet = bet - minimum;

	processMoneyBet(player, bet, minimum);
	cout << "BET: " << bet << endl;
// 	OUTPUTS:
// 	Folded:	-1
// 	Called:	 0
// 	Raised:	>0 - říká, o kolik jsi zvýšil
	return bet;
}



int smallBlindBet (struct Player* player) {
	processMoneyBet(player, 5, 0);

	string infoText = player->name + " is SMALL BLIND";
	createScreen (infoText, "", "PRESS ENTER TO CONTINUE");
	cin.ignore();
	return 5;
}

int bigBlindBet (struct Player* player) {
	processMoneyBet(player, 5, 5);

	string infoText = player->name + " is BIG BLIND";
	createScreen (infoText, "", "PRESS ENTER TO CONTINUE");
	cin.ignore();
	return 5;
}


int minimumBet(struct Player* player) {
	return game.bet.betOnTable - player->moneyBet;
}

int maximumBet() {
	//you can only rise amount of bet on board
	return game.bet.pot;
}


void processEndOfRound(struct Player* winner) {
	//vynuluj hráčům sázky
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (&pPlayers[i] == &winner)
			winner->moneyTotal += game.bet.pot;
		pPlayers[i]->moneyBet = 0;
	}

	game.bet.pot = 0;
	game.bet.betOnTable = 0;
	game.betOnBoard = 0; //OBSOLETE	
}

