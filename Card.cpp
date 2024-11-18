#include <cassert>
#include <iostream>
#include <array>
#include "Card.hpp"

using namespace std;

/////////////// Rank operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const RANK_NAMES[] = {
  "Two",   // TWO
  "Three", // THREE
  "Four",  // FOUR
  "Five",  // FIVE
  "Six",   // SIX
  "Seven", // SEVEN
  "Eight", // EIGHT
  "Nine",  // NINE
  "Ten",   // TEN
  "Jack",  // JACK
  "Queen", // QUEEN
  "King",  // KING
  "Ace"    // ACE
};

//REQUIRES str represents a valid rank ("Two", "Three", ..., "Ace")
//EFFECTS returns the Rank corresponding to str, for example "Two" -> TWO
Rank string_to_rank(const std::string &str) {
  for(int r = TWO; r <= ACE; ++r) {
    if (str == RANK_NAMES[r]) {
      return static_cast<Rank>(r);
    }
  }
  assert(false); // Input string didn't match any rank
  return {};
}

//EFFECTS Prints Rank to stream, for example "Two"
std::ostream & operator<<(std::ostream &os, Rank rank) {
  os << RANK_NAMES[rank];
  return os;
}

//REQUIRES If any input is read, it must be a valid rank
//EFFECTS Reads a Rank from a stream, for example "Two" -> TWO
std::istream & operator>>(std::istream &is, Rank &rank) {
  string str;
  if(is >> str) {
    rank = string_to_rank(str);
  }
  return is;
}



/////////////// Suit operator implementations - DO NOT CHANGE ///////////////

constexpr const char *const SUIT_NAMES[] = {
  "Spades",   // SPADES
  "Hearts",   // HEARTS
  "Clubs",    // CLUBS
  "Diamonds", // DIAMONDS
};

//REQUIRES str represents a valid suit ("Spades", "Hearts", "Clubs", or "Diamonds")
//EFFECTS returns the Suit corresponding to str, for example "Clubs" -> CLUBS
Suit string_to_suit(const std::string &str) {
  for(int s = SPADES; s <= DIAMONDS; ++s) {
    if (str == SUIT_NAMES[s]) {
      return static_cast<Suit>(s);
    }
  }
  assert(false); // Input string didn't match any suit
  return {};
}

//EFFECTS Prints Suit to stream, for example "Spades"
std::ostream & operator<<(std::ostream &os, Suit suit) {
  os << SUIT_NAMES[suit];
  return os;
}

//REQUIRES If any input is read, it must be a valid suit
//EFFECTS Reads a Suit from a stream, for example "Spades" -> SPADES
std::istream & operator>>(std::istream &is, Suit &suit) {
  string str;
  if (is >> str) {
    suit = string_to_suit(str);
  }
  return is;
}


/////////////// Write your implementation for Card below ///////////////

Card::Card() : rank(TWO), suit(SPADES) {}

Card::Card(Rank rank_in, Suit suit_in) : rank(rank_in), suit(suit_in) {}

Rank Card::get_rank() const {return rank;}

Suit Card::get_suit() const {return suit;}

Suit Card::get_suit(Suit trump) const {
  if(Card::is_left_bower(trump)){
    return trump;
  } else{
    return suit;
  }
}

bool Card::is_face_or_ace() const {
  return rank >= 9;
}

bool Card::is_right_bower(Suit trump) const {
  return rank == JACK && suit == trump;
}

bool Card::is_left_bower(Suit trump) const {
  Suit same_color_suit = Suit_next(trump); // suit of same color as trump suit
  return rank == JACK && suit == same_color_suit;
}

bool Card::is_trump(Suit trump) const {
  return (suit == trump) || is_left_bower(trump);
}

std::ostream & operator<<(std::ostream &os, const Card &card){
  os << RANK_NAMES[card.get_rank()];
  os << " of ";
  os << SUIT_NAMES[card.get_suit()];
  return os;
}

std::istream & operator>>(std::istream &is, Card &card){
  string rank_in;
  string of;
  string suit_in;
  is >> rank_in;
  is >> of; 
  is >> suit_in;
  card.rank = string_to_rank(rank_in);
  card.suit = string_to_suit(suit_in);
  return is;
}

bool operator<(const Card &lhs, const Card &rhs){
  return lhs.get_rank() < rhs.get_rank();
}

bool operator==(const Card &lhs, const Card &rhs){
  return lhs.get_rank() == rhs.get_rank();
}

bool operator<=(const Card &lhs, const Card &rhs){
  return (lhs < rhs) || (lhs == rhs);
}

bool operator>(const Card &lhs, const Card &rhs){
  return !(lhs <= rhs);
}

bool operator>=(const Card &lhs, const Card &rhs){
  return !(lhs < rhs);
}

bool operator!=(const Card &lhs, const Card &rhs){
  return !(lhs == rhs);
}

Suit Suit_next(Suit suit){
  return Suit((suit + 2) % 4);
}

bool Card_less(const Card &a, const Card &b, Suit trump){
  bool is_trump_a = a.is_trump(trump);
  bool is_trump_b = b.is_trump(trump);
  if(is_trump_a && !is_trump_b){return false;}
  if(!is_trump_a && is_trump_b){return true;}
  if(!is_trump_a && is_trump_b){return a < b;}
  // both trump case
  if(a.is_right_bower(trump)){return false;}
  if(b.is_right_bower(trump)){return true;}
  if(a.is_left_bower(trump)){return false;}
  if(b.is_left_bower(trump)){return true;}
  return a < b;
}

bool Card_less(const Card &a, const Card &b, const Card &led_card, Suit trump){
  Suit led_suit = led_card.get_suit();
  if(led_suit == trump){return Card_less(a, b, trump);}
  if(a.is_trump(trump) || b.is_trump(trump)){return Card_less(a, b, trump);}
  // both are not trump suit and led suit is not trump suit
  Suit a_suit = a.get_suit();
  Suit b_suit = b.get_suit();
  if(a_suit == led_suit && b_suit != led_suit){return false;}
  if(a_suit != led_suit && b_suit == led_suit){return true;}
  // either both led suit or both non-trump + non-led
  return a < b; 
}











