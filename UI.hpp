#define ROW_SIZE 15
#define SCREEN_WIDTH 100

//-- SCREEN FUNCTIONS
void 	clearScreen();
void 	printDeck();
void 	printHand(struct Player* player);
void 	printBoardCards();

//-- INTERACTION INTERFACE
void 	linesSeparator(string symbol, int linesCount, int linesLength);
void	pressEnterToContinue();
void	mainMenu();
void	introScreen();
void	printEmptyLines(int linesCount);
void 	printScreenHeader();
void 	printScreenFooter(string infoText, string actionText, string message);

//-- UNDER CONSTRUCTION
void	composeScreen (string infoText, string actionText, string message);

//-- WINNER SCREEN FUNCTIONS
struct Player* 	resultsScreen();
void 	printCardTable();
int 	stringLength(string str);
void 	printSpaces(int count);