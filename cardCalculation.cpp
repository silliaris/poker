#pragma once
#include "cardCalculation.hpp"
#include "poker.cpp" //using backwards #include for code assistance in KDevelop

#include <iostream>
#include <algorithm>


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
	/*
	cout << player->name << " Pairs: " << findPairs(player) << " Flush: " 
	<< findFlush(player) << " findStraight: " << findStraight(player) 
	<< " High hand: " << getHighHand(player) <<endl;
	cout << "Card value: " << cardValue << endl;
	*/

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

	//find strings of similar values
	int highestCard;
	int combo = 0;
	int streak = 0;
	
	for (int i = 0; i<7; i++) {
		switch (sortedValues[i+1] - sortedValues[i]) {
			case 0:		//value is the same - continue in loop, just don't increment
			break;
			case 1: 		// value is higher by 1 - increment by 1
				combo++;
			break;
			default:		//end counting and overwrite value combo to get highest value, with highest card
				if (combo > streak) {
					streak = combo + 1;
					combo = 0;
					highestCard = sortedValues[i];
				}
			break;
		}
	}
	
	//find out if it's flush
	int cardsPositions[5];
	int n = 0;
	//find positions of cards that are making straight
	for (int ii = highestCard - 4; ii <= highestCard; ii++) {
		for (int i = 0; i < HAND_AND_BOARD_SIZE; i++) {
			if (player->handAndBoard[i].value == ii) {				
				cardsPositions[n] = i;
				n++;
			}
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

//TODO: Refractor this shit - I have no idea what is it doing
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

int findPairs(struct Player *player) {
	int sameValues[7];
	int cardValue[7];
	//null variables
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


struct Player* calculateWinner() {
	//calculate all players values
	for (int i = 0; i < MAX_PLAYERS; i++) {
		calculateHandValue(pPlayers[i]);
		
		//if player is not in game, his card value is 0
		if (!pPlayers[i]->isInGame)
			pPlayers[i]->handValue = 0;
		
		cout << pPlayers[i]->name << "'s hand value: " << pPlayers[i]->handValue << endl;
	}
	
	//struct Player* higestValuePlayer = pPlayers[0];
	struct Player* currentPlayer;
	int isHighestValue;
	
	for (int playerNumber = 0; playerNumber < MAX_PLAYERS; playerNumber++) {
		currentPlayer = pPlayers[playerNumber];
		
		isHighestValue = 1;
		for (int i = 0; i < MAX_PLAYERS; i++) {
			//pokud kolem tohoto hráče není žádná hodnota vyšší, tak on 
			if (currentPlayer->handValue < pPlayers[i]->handValue) {
				isHighestValue = 0;
				break;
			}
		}
		if (isHighestValue) {
			break;
		}
	}
	//cout << "Something weird happened in this function" << endl;
	return currentPlayer;
}

