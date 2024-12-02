#include "Player.hpp"
#include <cassert>
#include <array>
#include <algorithm>

class SimplePlayer : public Player
{
public:
    SimplePlayer(std::string name_in) : name(name_in) {}

    // EFFECTS returns player's name
    const std::string &get_name() const override
    {
        return name;
    }

    // REQUIRES player has less than MAX_HAND_SIZE cards
    // EFFECTS  adds Card c to Player's hand
    void add_card(const Card &c) override
    {
        hand.push_back(c);
    }

    // REQUIRES round is 1 or 2
    // MODIFIES order_up_suit
    // EFFECTS If Player wishes to order up a trump suit then return true and
    //   change order_up_suit to desired suit.  If Player wishes to pass, then do
    //   not modify order_up_suit and return false.
    bool make_trump(const Card &upcard, bool is_dealer,
                    int round, Suit &order_up_suit) const override
    {
        if (round == 1)
        {
            // first round, order up in it would give then 2 or more face or ace trumps (greater value than then trump 10)
            Suit upcard_suit = upcard.get_suit();
            Card trump_ten(TEN, upcard_suit);
            int num_good_cards = 0;
            for (int i = 0; i < hand.size(); i++)
            {
                if (Card_less(trump_ten, hand[i], upcard_suit))
                {
                    // hard[i] is greater value than the trump ten
                    num_good_cards++;
                }
            }
            if (num_good_cards >= 2)
            {
                order_up_suit = upcard_suit;
                return true;
            }
            return false;
        }
        // round is 2
        Suit suit_same_color_as_upcard = Suit_next(upcard.get_suit());

        // dealer always order up suit with same color as upcard
        if (is_dealer)
        {
            order_up_suit = suit_same_color_as_upcard;
            return true;
        }

        // otherwise, order up suit with same color as upcard if it will
        // give 2 or more face or ace trumps (greater value than then trump 10)
        Card trump_ten(TEN, suit_same_color_as_upcard);
        int num_good_cards = 0;
        for (int i = 0; i < hand.size(); i++)
        {
            if (Card_less(trump_ten, hand[i], suit_same_color_as_upcard))
            {
                // hard[i] is greater value than the trump ten
                num_good_cards++;
            }
        }
        if (num_good_cards >= 1)
        {
            order_up_suit = suit_same_color_as_upcard;
            return true;
        }
        return false;
    }

    // REQUIRES Player has at least one card
    // EFFECTS  Player adds one card to hand and removes one card from hand.
    void add_and_discard(const Card &upcard) override
    {
        // pick up upcard
        hand.push_back(upcard);
        // discard lowest value card in hand
        Suit trump_suit = upcard.get_suit();
        int index = 0; // index of lowest value card
        for (int i = 1; i < hand.size(); i++)
        {
            if (Card_less(hand[i], hand[index], trump_suit))
            {
                // card i is of lower value than card index
                index = i;
            }
        }
        // DEBUG std::cout << "Dealer " << *this << " adds " << upcard << " and discards " << hand[index] << std::endl;
        discard_card(index);

    }

    // REQUIRES Player has at least one card
    // EFFECTS  Leads one Card from Player's hand according to their strategy
    //   "Lead" means to play the first Card in a trick.  The card
    //   is removed the player's hand.
    Card lead_card(Suit trump) override
    {

        // Find highest value trump in hand, if any
        int highest_value_trump_index = -1;

        // Also find highest non-trump in hand, if any
        int highest_value_non_trump_index = -1;

        for (int i = 0; i < hand.size(); i++)
        {
            if (hand[i].is_trump(trump))
            {
                // trump card
                if (highest_value_trump_index == -1 || Card_less(hand[highest_value_trump_index], hand[i], trump))
                {
                    highest_value_trump_index = i;
                }
            }
            else
            {
                // non-trump card
                if (highest_value_non_trump_index == -1 || Card_less(hand[highest_value_non_trump_index], hand[i], trump))
                {
                    highest_value_non_trump_index = i;
                }
            }
        }

        // return highest value non-trump, if any
        if (highest_value_non_trump_index != -1)
        {
            return discard_card(highest_value_non_trump_index);
        }

        // otherwise, return highest value trump
        return discard_card(highest_value_trump_index);
    }

    // REQUIRES Player has at least one card
    // EFFECTS  Plays one Card from Player's hand according to their strategy.
    //   The card is removed from the player's hand.
    Card play_card(const Card &led_card, Suit trump) override
    {

        // Find highest value led suit card in hand, if any
        int highest_value_led_index = -1;

        // Also find lowest value card in hand
        int lowest_value_index = -1;

        Suit led_suit = led_card.get_suit(trump);

        for (int i = 0; i < hand.size(); i++)
        {
            if (hand[i].get_suit(trump) == led_suit)
            {
                // led suit card
                if (highest_value_led_index == -1 || Card_less(hand[highest_value_led_index], hand[i], trump))
                {
                    highest_value_led_index = i;
                }
            }
            else
            {
                // non led suit card
                if (lowest_value_index == -1 || Card_less(hand[i], hand[lowest_value_index], trump))
                {
                    lowest_value_index = i;
                }
            }
        }

        // play highest value card of led suit, if any
        if (highest_value_led_index != -1)
        {
            return discard_card(highest_value_led_index);
        }

        // otherwise, play lowest value card
        return discard_card(lowest_value_index);
    }

private:
    std::string name;
    std::vector<Card> hand; // player's hand

    // REQUIRES i < hand.size()
    // EFFECTS Discard card at index i, and return it
    Card discard_card(int i)
    {
        Card return_card = hand[i];
        hand.erase(hand.begin() + i);
        return return_card;
    }
};

class HumanPlayer : public Player
{
public:
    HumanPlayer(std::string name_in) : name(name_in) {}

    // EFFECTS returns player's name
    const std::string &get_name() const override {
        return name;
    }

    // REQUIRES player has less than MAX_HAND_SIZE cards
    // EFFECTS  adds Card c to Player's hand
    void add_card(const Card &c) override
    {
        hand.push_back(c);
        std::sort(hand.begin(), hand.end());
    }

    // REQUIRES round is 1 or 2
    // MODIFIES order_up_suit
    // EFFECTS If Player wishes to order up a trump suit then return true and
    //   change order_up_suit to desired suit.  If Player wishes to pass, then do
    //   not modify order_up_suit and return false.
    bool make_trump(const Card &upcard, bool is_dealer,
                    int round, Suit &order_up_suit) const override
    {
        print_hand();
        std::cout << "Human player " << name << ", please enter a suit, or \"pass\":\n";
        std::string decision;
        std::cin >> decision;

        if (decision != "pass") {
            Suit ordered_up = string_to_suit(decision);
            order_up_suit = ordered_up;
            return true;
        }
        else {
            return false;
        }
    }

    // REQUIRES Player has at least one card
    // EFFECTS  Player adds one card to hand and removes one card from hand.
    void add_and_discard(const Card &upcard) override{
        print_hand();
        std::cout << "Discard upcard: [-1]\n";
        std::cout << "Human player " << name << ", please select a card to discard:\n";
        int decision;
        std::cin >> decision;

        if (decision != -1){
            // discard decision card and add upcard
            discard_card(decision);
            hand.push_back(upcard);
            std::sort(hand.begin(), hand.end());
        }

        // do nothing if discarding upcard
    }

    // REQUIRES Player has at least one card
    // EFFECTS  Leads one Card from Player's hand according to their strategy
    //   "Lead" means to play the first Card in a trick.  The card
    //   is removed the player's hand.
    Card lead_card(Suit trump) override{
        print_hand();
        std::cout << "Human player " << name << ", please select a card:\n";
        int decision;
        std::cin >> decision;
        return discard_card(decision);
    }

    // REQUIRES Player has at least one card
    // EFFECTS  Plays one Card from Player's hand according to their strategy.
    //   The card is removed from the player's hand.
    Card play_card(const Card &led_card, Suit trump) override{
        return lead_card(trump);
    }

private:
    std::string name;
    std::vector<Card> hand; // player's hand

    void print_hand() const
    {
        for (size_t i = 0; i < hand.size(); ++i)
            std::cout << "Human player " << name << "'s hand: "
                      << "[" << i << "] " << hand[i] << "\n";
    }

    // REQUIRES i < hand.size()
    // EFFECTS Discard card at index i, and return it
    Card discard_card(int i)
    {
        Card return_card = hand[i];
        hand.erase(hand.begin() + i);
        std::sort(hand.begin(), hand.end());
        return return_card;
    }
};

Player *Player_factory(const std::string &name, const std::string &strategy)
{
    // We need to check the value of strategy and return
    // the corresponding player type.
    if (strategy == "Simple")
    {
        // The "new" keyword dynamically allocates an object.
        return new SimplePlayer(name);
    }
    // Repeat for each other type of Player
    if (strategy == "Human")
    {
        // The "new" keyword dynamically allocates an object.
        return new HumanPlayer(name);
    }

    // Invalid strategy if we get here
    assert(false);
    return nullptr;
}

std::ostream &operator<<(std::ostream &os, const Player &p)
{
    os << p.get_name();
    return os;
}