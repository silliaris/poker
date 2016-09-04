#include "cardCalculation.hpp"

//-----------------------------------
// CARD VALUE CALCULATIONS
//-----------------------------------

//main function for comparations
int calculateHandValue(P_PLAYER player) {
	if (!player->isInGame)
		return 0;

	//prepare numbers for calculating
	handAndBoardCards(player);
	
	int cardValue = findPairs(player) + findFlush(player) + findStraight(player);
	if (cardValue == 0)
		cardValue = getHighHand(player);
	player->handValue = cardValue;

	printf("%s's comination: %s = %d",player->name,cardCombinationName(player),player->handValue);

	return cardValue;
}


int getHighHand(P_PLAYER player) {
	int highHand = 0;
	for (int i = 0; i<7; i++)
		if (player->handAndBoard[i].value > highHand)
			highHand = player->handAndBoard[i].value;
	return highHand;
}

int findFlush(P_PLAYER player) {
	//find out faces of each card. If 5 are same, it's flush
	int sameFaces[4] = {0,0,0,0};
	int flushValue = 0;
	
	for (int i = 0; i<4; i++) {
		for (int ii = 0; ii<7; ii++)
			if (player->handAndBoard[ii].color == i && player->handAndBoard[ii].value != 0)
				sameFaces[i]++;
		if (sameFaces[i] >= 5)
			flushValue = FLUSH_VALUE;
	}
	return flushValue;
}

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}


int findStraight (P_PLAYER player) {
	//skonči pokud je 4 a míň karet
	if (game.flow.round < 1) {
		printf("Not staright: Not enough cards on board");
		return 0;
	}

	printf("Calculating straight.");

	//sort cards in hand
	int sortedValues[HAND_AND_BOARD_SIZE];
	for (int i = 0; i<HAND_AND_BOARD_SIZE; i++)
		sortedValues[i] = player->handAndBoard[i].value;
	qsort(sortedValues, HAND_AND_BOARD_SIZE, sizeof(int), cmpfunc);

	int highestCard = 0;
	int straightLength = 0;
	int savedStraight = 0;	

	for (int i = 0; i < HAND_AND_BOARD_SIZE; i++)
		switch (sortedValues[i+1] - sortedValues[i]) {
			case 0:		//value is the same - continue in loop, just don't increment
			break;
			case 1: 		//karta je o 1 větší
				straightLength++;
			break;
			default:		//skonči tulhe postupku, zaznamenej její délku a nejvyšší kartu
				if (straightLength > savedStraight) {
					savedStraight = straightLength + 1; //skutečná délka postupky
					highestCard = sortedValues[i];
				}
				straightLength = 0;
			break;
		}

	//TODO: tohle by mělo vrátit momentální postupku
	printf("\n");
	printf("Straight cards: %d - highest card: %d\n",savedStraight, highestCard);

	printf("Finding out flush\n");

	//find out if it's flush
	int cardsPositions[5] = {0,0,0,0,0};
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
	
	int isFlush = 1;
	for (int i = 0; i < 5; i++) {
		if (player->handAndBoard[cardsPositions[i]].color != player->handAndBoard[cardsPositions[0]].color) {
			isFlush = 0;
			break;
		}
	}

	printf("Assigning return valu\n");
	//cout << "Assigning return value" << endl;

	//Assign return value
	int returnVal = 0;
	if (savedStraight >= 5) {
		if (isFlush)
			returnVal = STRAIGHT_FLUSH_VALUE;
		else 
			returnVal = STRAIGHT_VALUE;
	}
	//Low ace straight
	if (savedStraight == 4 && highestCard - 4 == 2 && sortedValues[HAND_AND_BOARD_SIZE] == 14)
		returnVal = STRAIGHT_VALUE;
	
	if (returnVal != 0)
		returnVal += highestCard;

	printf("Straight value = %d\n",returnVal);
	//cout << "Straight value = " << returnVal << endl;
	return returnVal;
}


//TODO: Refractor this shit - I have no idea what is it doing
void handAndBoardCards(P_PLAYER player) {
	int i = 0;
	for (; i<2; i++)
		player->handAndBoard[i] = player->hand[i];
	
	//for (int j = 0; !game.boardCards[j].name.empty() && j<5;i++, j++)
	for (int j = 0; strlen(game.boardCards[j].name) > 0 && j<5;i++, j++)
		player->handAndBoard[i] = game.boardCards[j];
}

int findPairs(P_PLAYER player) {
	
	int cardsCombined;
	struct Combination {
		int combinationType; //pair, 3/4 of kind
		int cardValue; //king, ace,etc
	} combination[3] = {{0,0},{0,0},{0,0}};
	
	int combinationNumber = 0;
	for (int j = 0; j < 7 && player->handAndBoard[j].value != 0; j++) {
		int currentCardVal = player->handAndBoard[j].value;
		cardsCombined = 0;
		for (int i = 0; i < 7; i++) {
			if (currentCardVal == player->handAndBoard[i].value)
				cardsCombined++;
		}
		//assign combination to a structure
		printf("Calculating pairs\n");
		//cout << "Calculating pairs" << endl;
		if (cardsCombined > 1)
			if (j == 0 ||
			(combination[0].cardValue != currentCardVal
			&& combination[1].cardValue != currentCardVal
			&& combination[2].cardValue != currentCardVal)) {
				combination[combinationNumber].combinationType = cardsCombined;
				combination[combinationNumber].cardValue = currentCardVal;
				combinationNumber++;
				printf("Combination: %d: %d cards, %d\n", combinationNumber, cardsCombined, currentCardVal);
				//cout << "Combination " << combinationNumber << ": " << cardsCombined << " cards, " << currentCardVal << endl;
			}		
	}

	int resultCombination = 0;
	//najdi typ kombinací 
	for (int i = 0; i < 3; i++) {
		if (combination[i].combinationType > 0) {
			if (combination[i].combinationType == 4)
				resultCombination = 16; //4 OF KIND
			else 
				resultCombination += combination[i].combinationType;
		}
	}

	int combinationValue = 0;
	switch (resultCombination) {
		case 2:
			combinationValue = 100;
			break;
		case 3:
			combinationValue = 300;
			break;
		case 4:
			combinationValue = 200;
			break;
		case 5:
			combinationValue = 5000;
			break;
		case 6:
			combinationValue = 200;
			break;
		case 16:
			combinationValue = 8000;
			break;
	}
	
	//najdi nejvyšší kartu kombinace
	int highestCardValue = 0;

	if (combinationValue > 0) {
		int highestCombinationType = 0;
		for (int i = 0; i < 3; i++) {
			if (combination[i].combinationType >= highestCombinationType) {
				if (combination[i].combinationType > highestCombinationType)
					highestCardValue = 0;
				highestCombinationType = combination[i].combinationType;
				if (combination[i].cardValue > highestCardValue)
					highestCardValue = combination[i].cardValue;
			}
		}
	}
	
	return combinationValue + highestCardValue;
}

const char *cardCombinationName(P_PLAYER player) {
	/* 
	* Přiřaď hodnotu ke každé kombinaci. Musím si udělat na kombinace tabulku, která bude říkat, co má jakou honotu. 
	* 
	* High hand: 	0-14
	* Pair: 		100
	* Two pairs:	200
	* 3 of a kind:  300
	* Straight: 	1000 (měřit nejnižší kartu)
	* Flush: 		2000
	* Full house:	5000
	* 4 of a kind: 8000
	* S. flush:	10 000
	* Royal flush: 10 014
	*/

	int handVal = player->handValue;
	
	if (handVal < 100)
		return "High hand";
	else if (handVal >= 100 && handVal < 200)
		return "Pair";
	else if (handVal >= 200 && handVal < 300)
		return "Two pairs";
	else if (handVal >= 300 && handVal < 1000)
		return "3 of kind";
	else if (handVal >= 1000 && handVal < 2000)
		return "Straight";
	else if (handVal >= 2000 && handVal < 5000)
		return "Flush";
	else if (handVal >= 5000 && handVal < 8000)
		return "Full house";
	else if (handVal >= 8000 && handVal < 10000)
		return "4 of kind";
	else if (handVal >= 10000 && handVal < 10014)
		return "Straight flush";
	else
		return "Royal flush";
}


P_PLAYER calculateWinner() {
	//calculate all players values
	for (int i = 0; i < MAX_PLAYERS; i++) {
		calculateHandValue(pPlayers[i]);
		
		//if player is not in game, his card value is 0
		if (!pPlayers[i]->isInGame)
			pPlayers[i]->handValue = 0;

		//cout << pPlayers[i]->name << "'s hand value: " << pPlayers[i]->handValue << endl;
		printf("%s' hand value: %d\n",pPlayers[i]->name,pPlayers[i]->handValue);
	}
	
	//struct Player* higestValuePlayer = pPlayers[0];
	P_PLAYER currentPlayer;
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
		if (isHighestValue)
			break;
	}
	return currentPlayer;
}
