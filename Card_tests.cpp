#include "Card.hpp"
#include "unit_test_framework.hpp"
#include <iostream>

using namespace std;


TEST(test_card_ctor) {
    Card c(ACE, HEARTS);
    ASSERT_EQUAL(ACE, c.get_rank());
    ASSERT_EQUAL(HEARTS, c.get_suit());
}

TEST(card_less_with_trump_and_led) {
    // trump spades
    Suit trump = SPADES;
    Card qOfD(QUEEN,DIAMONDS);
    Card aOfS(ACE,SPADES);
    Card jOfS(JACK,SPADES);
    // led diamonds
    ASSERT_TRUE(Card_less(aOfS, jOfS, qOfD, trump));
    ASSERT_FALSE(Card_less(Card(KING,DIAMONDS), Card(ACE,HEARTS), qOfD, trump));
    ASSERT_TRUE(Card_less(Card(KING,DIAMONDS), Card(NINE,SPADES), qOfD, trump));
    ASSERT_TRUE(Card_less(Card(KING,SPADES), Card(JACK,CLUBS), qOfD, trump));
}

TEST_MAIN()
