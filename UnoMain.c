#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#define DECK_SIZE 108

typedef struct card_s {
	char color[7];
	int value;
	char action[15];
	struct card_s* pt;
} card;

void scanDeck(card d[], FILE* inp);
void shuffleDeck(card d[]);
void addCard(card d[], card** outH, card** outT, int i);
void deleteCard(card** h, int count);
void setDummyCard(card d[], int i);
void printHand(card* h, int turn);
bool checkCard(card* h, card d[], int i, int CardsInDeck);
void addToDiscard(card* h, card d[], int i);
void printCard(card* h, int i);
void printDiscard(card d[]);
bool WIN(card* h, card* t, int i);

int main() {

	char playAgain = 'y';
	int i;
	char chooseDeck;
	char deckName[20];
	int cardPick;
	FILE* deckFile;
	card* deck;

	while (playAgain == 'y') {

		card* pHead[2];
		card* pTail[2];
		int deckCount = 0; // number in deck counter
		int discard = 0; // discard pile index counter
		int turn = 2;
		int playerHandCount[2] = { 7, 7 };
		int numPlayers = 2;

		deck = (card*)malloc(DECK_SIZE * sizeof(card));

		pHead[0] = NULL;
		pHead[1] = NULL;
		pTail[0] = NULL;
		pTail[1] = NULL;

		printf("Let's play a game of UNO\n\n");
		printf("Press 1 to shuffle the UNO deck or 2 to load a deck from the file: ");
		scanf(" %c", &chooseDeck);
		printf("\n");

		while (!(chooseDeck == '1' || chooseDeck == '2')) { // repeats until user enters a 1 or a 2 (chooses deck)
			printf("Invalid Input.\n\n");
			printf("Press 1 to shuffle the UNO deck or 2 to load a deck from the file: ");
			scanf(" %c", &chooseDeck);
			printf("\n");
		}

		if (chooseDeck == '1') { // initializes deck to standard pre-built deck file
			strcpy(deckName, "baseDeck.txt");
			deckFile = fopen(deckName, "r");
		}
		else { // initializes deck to file of user's choice
			printf("Enter the name of your deck's file: ");
			scanf("%s", deckName);
			printf("\n");
			deckFile = fopen(deckName, "r");
			while (deckFile == NULL) { // loops until user enters a valid deck file
				printf("File not found.\n\n");
				printf("Enter the name of your deck's file: "); 
				scanf("%s", deckName);
				printf("\n");
				deckFile = fopen(deckName, "r");
			}
		}

		scanDeck(deck, deckFile);
		shuffleDeck(deck);

		printf("The deck is shuffled. Press any key to deal the cards. ");
		scanf("%*c%*c");
		printf("\n");

		while (deckCount < (numPlayers * 7)) { // Deals Cards
			for (i = 0; i < numPlayers; ++i) {
				addCard(deck, &pHead[i], &pTail[i], deckCount); // moves single card
				setDummyCard(deck, deckCount);
				++deckCount;
			}
		}
		deck[discard] = deck[deckCount];
		++deckCount;

		if (deck[discard].value > 9 && deck[discard].value < 13) {
			printf("Discard pile: %s (%s) \n\n", deck[discard].action, deck[discard].color);
		}

		else if (deck[discard].value >= 13) {
			printf("Discard pile: %s \n\n", deck[discard].action);

			printf("If the first card is a wild card restart the game (according to game rules). \n\n");
		}

		else {
			printf("Discard pile: %d (%s)\n\n", deck[discard].value, deck[discard].color);
		}

		while (!WIN(pHead[(turn - 1) % 2], pTail[(turn - 1) % 2], turn)) { // Checks if game has been won

			printHand(pHead[turn % 2], turn);

			if (playerHandCount[turn % 2] != 1) { // Player picks their card
				printf("Press 1-%d to play any card from your hand, or press zero to draw a card from the deck: ", playerHandCount[turn % 2]);
			}
			else {
				if (turn % 2 == 0) {
					printf("Player one has UNO\n\n");
				}
				else {
					printf("Player two has UNO\n\n");
				}
				printf("Press 1 to play the card from your hand, or press zero to draw a card from the deck: ");
			}
			scanf("%d", &cardPick);
			printf("\n");
			while (cardPick > playerHandCount[turn % 2]) { // checks if user chose valid number
				printf("Invalid Input.\n\n");
				if (playerHandCount[turn % 2] != 1) { // Player picks their card
					printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
				}
				else {
					printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
				}
				scanf("%d", &cardPick);
				printf("\n");
			}


			// Check if user draws a card then, check if they can play card or to skip turn
			if (cardPick == 0) { // switches to skip turn choice  after drawing card
				addCard(deck, &pHead[turn % 2], &pTail[turn % 2], deckCount);
				printHand(pHead[turn % 2], turn);
				setDummyCard(deck, deckCount);
				++deckCount;
				++playerHandCount[turn % 2];

				if (playerHandCount[turn % 2] != 1) { // Player picks their card
					printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
				}
				else {
					printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
				}
				scanf("%d", &cardPick);
				printf("\n");
				while (cardPick > playerHandCount[turn % 2]) { // checks if user chose valid number
					printf("Invalid Input.\n\n");
					if (playerHandCount[turn % 2] != 1) { // Player picks their card
						printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
					}
					else {
						printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
					}
					scanf("%d", &cardPick);
					printf("\n");
				}
				while (!checkCard(pHead[turn % 2], deck, cardPick, playerHandCount[turn % 2]) && cardPick != 0) {
					printf("The ");
					printCard(pHead[turn % 2], cardPick);
					printf(" cannot be placed on top of %d (%s)\n\n", deck[discard].value, deck[discard].color);
					if (playerHandCount[turn % 2] != 1) { // Player picks their card
						printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
					}
					else {
						printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
					}
					scanf("%d", &cardPick);
					printf("\n");
					while (cardPick > playerHandCount[turn % 2]) { // checks if user chose valid number
						printf("Invalid Input.\n\n");
						if (playerHandCount[turn % 2] != 1) { // Player picks their card
							printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
						}
						else {
							printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
						}
						scanf("%d", &cardPick);
						printf("\n");
					}
				}
			}

			// check if user plays correct card
			else {
				while (!checkCard(pHead[turn % 2], deck, cardPick, playerHandCount[turn % 2]) && cardPick != 0) {
					printf("The ");
					printCard(pHead[turn % 2], cardPick);
					printf(" cannot be placed on top of %d (%s)\n\n", deck[discard].value, deck[discard].color);
					if (playerHandCount[turn % 2] != 1) { // Player picks their card
						printf("Press 1-%d to play any card from your hand, or press zero to draw a card from the deck: ", playerHandCount[turn % 2]);
					}
					else {
						printf("Press 1 to play the card from your hand, or press zero to draw a card from the deck: ");
					}
					scanf("%d", &cardPick);
					printf("\n");
					while (cardPick > playerHandCount[turn % 2]) { // checks if user chose valid number
						printf("Invalid Input.\n\n");
						if (playerHandCount[turn % 2] != 1) { // Player picks their card
							printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
						}
						else {
							printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
						}
						scanf("%d", &cardPick);
						printf("\n");
					}
					if (cardPick == 0) { // switches to skip turn choice after drawing card
						addCard(deck, &pHead[turn % 2], &pTail[turn % 2], deckCount);
						printHand(pHead[turn % 2], turn);
						setDummyCard(deck, deckCount);
						++deckCount;
						++playerHandCount[turn % 2];

						if (playerHandCount[turn % 2] != 1) { // Player picks their card
							printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
						}
						else {
							printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
						}
						scanf("%d", &cardPick);
						printf("\n");
						while (cardPick > playerHandCount[turn % 2]) { // checks if user chose valid number
							printf("Invalid Input.\n\n");
							if (playerHandCount[turn % 2] != 1) { // Player picks their card
								printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
							}
							else {
								printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
							}
							scanf("%d", &cardPick);
							printf("\n");
						}
						while (!checkCard(pHead[turn % 2], deck, cardPick, playerHandCount[turn % 2]) && cardPick != 0) {
							printf("The ");
							printCard(pHead[turn % 2], cardPick);
							printf(" cannot be placed on top of %d (%s)\n\n", deck[discard].value, deck[discard].color);
							if (playerHandCount[turn % 2] != 1) { // Player picks their card
								printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
							}
							else {
								printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
							}
							scanf("%d", &cardPick);
							printf("\n");
							while (cardPick > playerHandCount[turn % 2]) { // checks if user chose valid number
								printf("Invalid Input.\n\n");
								if (playerHandCount[turn % 2] != 1) { // Player picks their card
									printf("Press 1-%d to play any card from your hand, or press zero to skip your turn: ", playerHandCount[turn % 2]);
								}
								else {
									printf("Press 1 to play the card from your hand, or press zero to skip your turn: ");
								}
								scanf("%d", &cardPick);
								printf("\n");
							}
						}
					}
				}
			}

			if (cardPick != 0) { // checks if user played card or skipped turn (runs if they played card)
				addToDiscard(pHead[turn % 2], deck, cardPick);
				deleteCard(&pHead[turn % 2], cardPick);
				--playerHandCount[turn % 2];
				if (deck[discard].value >= 10 && deck[discard].value <= 11) { // special condition for reverses and skips (skips turn)
					++turn;
				}
				else if (deck[discard].value == 12) { // special condition for draw two cards (adds 2 cards + skip turn)
					++turn;
					for (i = 0; i < 2; ++i) {
						addCard(deck, &pHead[turn % 2], &pTail[turn % 2], deckCount);
						++deckCount;
						++playerHandCount[turn % 2];
					}
				}
				else if (deck[discard].value == 13) { // special condition for wild cards (choose color)
					printf("Choose a color for your wild (red/yellow/green/blue): ");
					scanf("%s", deck[discard].color);
					printf("\n");
				}
				else if (deck[discard].value == 14) { // special condition for wild draw four cards (adds 4 cards + choose color + skip turn)
					++turn;
					for (i = 0; i < 4; ++i) {
						addCard(deck, &pHead[turn % 2], &pTail[turn % 2], deckCount);
						++deckCount;
						++playerHandCount[turn % 2];
					}
					printf("Choose a color for your wild (red/yellow/green/blue): ");
					scanf("%s", deck[discard].color);
					printf("\n");
				}
			}

			printDiscard(deck);
			++turn;

			if (deckCount == DECK_SIZE) { // reshuffles deck if draw deck is empty
				scanDeck(deck, deckFile);
				shuffleDeck(deck);
				deckCount = 1;
			}
		}

		printf("Would you like to play again (y/n)? ");
		scanf(" %c", &playAgain);
		printf("\n");
	}

	printf("Bye bye\n\n");

	return 0;
}

void scanDeck(card d[], FILE* inp) { // scans file into deck array
	int i;

	for (i = 0; i < DECK_SIZE; ++i) { // loops through deck of size DECK_SIZE and scans in value and color
		fscanf(inp, "%d %s", &d[i].value, d[i].color);
		if (d[i].value == 10) {
			strcpy(d[i].action, "Reverse");
		}
		else if (d[i].value == 11) {
			strcpy(d[i].action, "Skip");
		}
		else if (d[i].value == 12) {
			strcpy(d[i].action, "Draw Two");
		}
		else if (d[i].value == 13) {
			strcpy(d[i].action, "Wild");
		}
		else if (d[i].value == 14) {
			strcpy(d[i].action, "Wild Draw Four");
		}
	}
}

void shuffleDeck(card d[]) { // shuffles deck array
	int i, a, b;
	card temp;

	srand(time(0));

	for (i = 0; i < 1000; ++i) {
		a = rand() % DECK_SIZE;
		b = rand() % DECK_SIZE;
		temp = d[a];
		d[a] = d[b];
		d[b] = temp;
	}
}

void addCard(card d[], card** outH, card** outT, int i) { // adds a card to a players hand
	card* temp;
	temp = (card*)malloc(sizeof(card));
	temp->value = d[i].value;
	strcpy(temp->color, d[i].color);
	strcpy(temp->action, d[i].action);

	temp->pt = *outH;
	*outH = temp;
}

void deleteCard(card** h, int count) { // deletes a card from a players hand
	int i;
	card* temp1;
	card* temp2;
	temp1 = *h;

	if (count == 1) { //DELETES CARD IF THE TARGET CARD IS THE FIRST CARD BY MOVING HEAD
		(*h) = temp1->pt;
	}

	for (i = 0; i < count - 2; ++i) { //COUNT IS THE POSITION OF THE THING WE WANT TO DELETE
		temp1 = temp1->pt;
	}

	temp2 = temp1->pt;
	if (temp2 != NULL) { // if this is the last card in players hand
		temp1->pt = temp2->pt;
	}
	else {
		temp1->pt = NULL;
	}
}

void setDummyCard(card d[], int i) { // to make sure we aren't reusing cards
	d[i].value = -1;
	strcpy(d[i].color, "DUMMY");
	strcpy(d[i].action, "DUMMY");
}

void printHand(card* h, int turn) { // prints a player's entire hand of cards
	char player[10];
	if (turn % 2 == 0) {
		strcpy(player, "one");
	}
	else {
		strcpy(player, "two");
	}
	printf("Player %s's hand: ", player);
	if (h->value > 12) {
		printf("%s", h->action);
	}
	else if (h->value > 9) {
		printf("%s (%s)", h->action, h->color);
	}
	else {
		printf("%d (%s)", h->value, h->color);
	}
	h = h->pt;
	while (h != NULL) {
		if (h->value > 12) {
			printf(", %s", h->action);
		}
		else if (h->value > 9) {
			printf(", %s (%s)", h->action, h->color);
		}
		else {
			printf(", %d (%s)", h->value, h->color);
		}
		h = h->pt;
	}
	printf("\n\n");
}

bool checkCard(card* h, card d[], int i, int CardsInDeck) { // Checks if a card can be played onto current discard pile card

	int WildDrawFourCheck = 0;
	card* temp = h;

	for (int j = 0; j < i - 1; ++j) {
		h = h->pt;
	}
	if (h->value == d[0].value) { // checks for same card number
		return true;
	}
	else if (strcmp(h->color, d[0].color) == 0) { // checks for same card color
		return true;
	}
	else if (strcmp(h->action, d[0].action) == 0) { // checks for same card action
		return true;
	}
	else if (strcmp(h->color, "black") == 0) { // checks if it's a wild card

		if (strcmp(h->action, "Wild Draw Four") == 0) {
			for (int x = 0; x < CardsInDeck - 1; x++)
			{
				temp = temp->pt;

				if ((temp->value == d[0].value) || (strcmp(temp->color, d[0].color) == 0)) {
					WildDrawFourCheck = WildDrawFourCheck + 1;
				}
			}

			if (WildDrawFourCheck > 0) { // check if player has another playable card before placing wild draw four
				printf("You cannot play a wild card when you have a card in your deck that can be placed on the discard card.\n");
				printf("\n");
				return false;
			}

			else {
				return true;
			}
		}
		else {
			return true;
		}
	}



	return false;
}

void addToDiscard(card* h, card d[], int i) { // adds to first index in deck array (discard pile location)
	for (int j = 0; j < i - 1; ++j) {
		h = h->pt;
	}
	d[0].value = h->value;
	strcpy(d[0].color, h->color);
	strcpy(d[0].action, h->action);
}

void printCard(card* h, int i) { // Prints single card from linked list
	for (int j = 0; j < i - 1; ++j) {
		h = h->pt;
	}
	if (h->value > 12) {
		printf("%s", h->action);
	}
	else if (h->value > 9) {
		printf("%s (%s)", h->action, h->color);
	}
	else {
		printf("%d (%s)", h->value, h->color);
	}
}

void printDiscard(card d[]) { // prints top of discard pile
	printf("Discard pile: ", d[0].value, d[0].color);
	if (d[0].value > 12) {
		printf("%s (%s)", d[0].action, d[0].color);
	}
	else if (d[0].value > 9) {
		printf("%s (%s)", d[0].action, d[0].color);
	}
	else {
		printf("%d (%s)", d[0].value, d[0].color);
	}
	printf("\n\n");
}

bool WIN(card* h, card* t, int i) { // checks if player is out of cards
	if (h == NULL && t == NULL) {
		printf("Player %d wins\n\n", (i % 2) + 1);
		return true;
	}
	return false;
}