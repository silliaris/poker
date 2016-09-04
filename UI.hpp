#ifndef UI_INCLUDED
#define UI_INCLUDED

#include <stdio.h>
#include <cstring>
#include "poker.hpp"

struct UI {
	int blankSpaces[2];
};

#define ROW_SIZE 18
#define SCREEN_WIDTH 100

void	mainMenu();
void	introScreen();
void 	clearScreen();

void 	printScreenHeader();
void 	printScreenFooter(const char* infoText, const char* actionText, const char* message);
void	printHand(P_PLAYER player);
void 	printCardTable();
void	printBoardCards();

void	printEmptyLines(int count);
void	printSpaces (int count);
void 	linesSeparator(const char* symbol, int linesCount, int linesLength);

void 	createScreen (const char *infoText, const char *actionText, const char *message);
void 	createBetScreen(int minimumBet, int maximumBet);
void 	resultsScreen(P_PLAYER winner);

void	pressEnterToContinue();
void	wrongInputScreen();

#endif
