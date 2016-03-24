
#define PAIR_VALUE 100
#define TWO_PAIRS_VALUE 200
#define TRIPLE_VALUE 300
#define STRAIGHT_VALUE 1000
#define FLUSH_VALUE 2000
#define FULL_HOUSE_VALUE 5000
#define QUADRIPLE_VALUE 8000
#define STRAIGHT_FLUSH_VALUE 10000

//-- CARD VALUE CALCULATIONS
int 	calculateHandValue(struct Player* player);
void 	handAndBoardCards(struct Player* player);

int 	getHighHand(struct Player* player);
int 	findPairs(struct Player *player);
int 	findFlush(struct Player* player);
int 	findStraight (struct Player* player);

//-- WINNER CALCULATION
struct Player* calculateWinner();
