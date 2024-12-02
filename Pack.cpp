#include "Pack.hpp"

using namespace std;

Pack::Pack() : next(0) {
    int counter = 0;
    for(int suit = SPADES; suit <= DIAMONDS; suit++){
        for(int rank = NINE; rank <= ACE; rank++){
            cards[counter] = Card(Rank(rank), Suit(suit));
            counter++;
        }
    }
}

Pack::Pack(std::istream& pack_input) : next(0) {
    for(int i = 0; i < PACK_SIZE; i++){
        pack_input >> cards[i];
    }
}

Card Pack::deal_one() {
    next++;
    return cards[next-1];
}

void Pack::reset() {
    next = 0;
}

void Pack::single_in_shuffle(){
    std::array<Card, PACK_SIZE> old_cards = cards;
    for(int i = 0; i < PACK_SIZE/2; i++){
        cards[2*i] = old_cards[PACK_SIZE/2 + i];
        cards[2*i + 1] = old_cards[i];
    }
}

void Pack::shuffle(){
    for(int i = 0; i < 7; i++){
        this->single_in_shuffle();
    }
}

bool Pack::empty() const {
    return next >= PACK_SIZE;
}
