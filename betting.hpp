// BETTING UI - tohle UI nahrazuje většinu toho, které teď v UI a poker.cpp je. Tam je strašnej bordel.
void 	createScreen (string infoText, string actionText, string message);
void	wrongInputScreen();
void 	createBetScreen (int minimumBet, int maximumBet);

//auxillary UI fnc
int 	countLines (string s);

//General functions
int 			isPlayer (struct Player *player);
struct Player* 	nextPlayer();
struct Player*	currentPlayer();

int 			enterBetAI (struct Player* player, int minimumBet);
int 			enterBetPlayer (struct Player* player, int minimumBet);
struct Player*	bettingRound();
void			bettingCycle();
void 			processMoneyBet(struct Player* player, int bet, int minBet);