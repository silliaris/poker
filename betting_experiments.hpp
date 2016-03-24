//flow controllers
int betRound();

struct Player* moveSmallBlindPlayer();

//entering bets
int enterBet(struct Player* player);
int betAI(struct Player* player);
int betPlayer(struct Player* player);

//automatic bets
int smallBlindBet (struct Player* player);
int bigBlindBet (struct Player* player);

//mins and maxes for betting
int minimumBet(struct Player* player);
int maximumBet();

void processEndOfRound(struct Player* winner);