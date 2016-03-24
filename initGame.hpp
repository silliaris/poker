//-- UTILITY FUNCTIONS
void 	nullCardArrays(struct Player* player); //is it really needed?

//-- CARD DECK OPERATIONS
void 	shuffleDeck();
void 	createDeck();
void 	createLineup(int faceNumber);

//-- PLAYER CARD OPERATIONS
int 	dealCardsToPlayer(struct Player* player);
void 	dealCardToBoard();
void 	createCardDeck();
