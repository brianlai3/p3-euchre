#include <iostream>
#include <fstream>
#include <array>
#include "Player.hpp"
#include "Pack.hpp"

enum Player_Number
{
    PLAYER_ZERO = 0,
    PLAYER_ONE = 1,
    PLAYER_TWO = 2,
    PLAYER_THREE = 3,
};

enum Team_Number
{
    TEAM_ZERO_AND_TWO = 0,
    TEAM_ONE_AND_THREE = 1,
};

class Game
{
public:
    static const int num_players = 4;
    static const int num_teams = 2;

    Game(const std::istream &pack_input, const bool shuffle, const int points_to_win,
         const std::array<std::string, num_players> names, const std::array<std::string, num_players> strategies)
        : points_to_win(points_to_win), shuffle_between_hands(shuffle), hand_number(0), dealer(PLAYER_ZERO)
    {
        points = {0};
        for (int i = 0; i < num_players; i++)
        {
            players[i] = Player_factory(names[i], strategies[i]);
        }
    }

    ~Game()
    {
        for (size_t i = 0; i < players.size(); ++i)
        {
            delete players[i];
        }
    }

    // EFFECTS: Play hands until one team has points_to_win or more points.
    void play()
    {
        while (points[0] < points_to_win && points[1] < points_to_win)
        {
            std::cout << "Hand " << hand_number << std::endl;
            play_hand();
            // print scores
            std::cout << team_names(TEAM_ZERO_AND_TWO) << " have " << points[0] << " points" << std::endl;
            std::cout << team_names(TEAM_ONE_AND_THREE) << " have " << points[1] << " points" << std::endl;
            std::cout << std::endl;

            hand_number++;
            dealer = pass_left(dealer);
        }

        // A team has won
        if (points[0] >= points_to_win)
        {
            std::cout << team_names(TEAM_ZERO_AND_TWO) << " win!" << std::endl;
        }
        else
        {
            std::cout << team_names(TEAM_ONE_AND_THREE) << " win!" << std::endl;
        }
    }

private:
    // variables through entire game
    std::array<Player *, num_players> players; // players indexed 0,1,2,3
    std::array<int, num_teams> points;         // points for players 0 and 2 (index 0) and for players 1 and 3 (index 1)
    Pack pack;
    const int points_to_win;
    const bool shuffle_between_hands;
    const std::array<int, 2 * num_players> dealing_pattern = {3, 2, 3, 2, 2, 3, 2, 3};

    // variables each hand
    int hand_number;
    Player_Number dealer;

    // EFFECTS move player 1 to the left
    Player_Number pass_left(const Player_Number player)
    {
        return Player_Number((player + 1) % num_players);
    }

    // EFFECTS move player 2 to the left (your teammate)
    Player_Number pass_across(const Player_Number player)
    {
        return Player_Number((player + 2) % num_players);
    }

    // EFFECTS get team number of a player
    Team_Number get_team(Player_Number player)
    {
        return Team_Number(player % 2);
    }

    // EFFECTS get the other team
    Team_Number other_team(Team_Number team)
    {
        return Team_Number((team + 1) % 2);
    }

    std::string team_names(Team_Number team)
    {
        Player_Number first_teammate = Player_Number(team);
        Player_Number second_teammate = pass_across(first_teammate);
        return players[first_teammate]->get_name() + " and " + players[second_teammate]->get_name();
    }

    // MODIFIES sets the upcard
    // EFFECTS deal out pack in pattern of dealing_pattern
    void deal(Card &upcard)
    {
        // print who deals
        std::cout << *players[dealer] << " deals" << std::endl;

        // start with player left of dealer
        Player_Number next_player = pass_left(dealer);
        for (int i = 0; i < dealing_pattern.size(); i++)
        {
            // deal dealing_pattern[i] number of cards to next_player
            for (int j = 0; j < dealing_pattern[i]; j++)
            {
                Card next_card = pack.deal_one();
                players[next_player]->add_card(next_card);
            }
            // go to next player
            next_player = pass_left(next_player);
        }

        // deal upcard
        upcard = pack.deal_one();
        std::cout << upcard << " turned up" << std::endl;
    }

    void make_trump(const Card upcard, Suit &trump_suit, Team_Number &ordered_up_team)
    {
        Player_Number next_player = pass_left(dealer);

        // Rounds 1 and 2
        for (int round = 1; round <= 2; round++)
        {
            for (int i = 0; i < num_players; i++)
            {
                bool is_dealer = (next_player == dealer);
                if (players[next_player]->make_trump(upcard, is_dealer, round, trump_suit))
                {
                    // trump_suit has been set by player next_player
                    // that team has ordered up
                    ordered_up_team = get_team(next_player);

                    // print who ordered up and the suit
                    std::cout << *players[next_player] << " orders up " << trump_suit << std::endl;
                    std::cout << std::endl;

                    // if round 1, dealer picks up upcard and discards a card
                    if(round == 1){
                        players[dealer]->add_and_discard(upcard);
                    }
                    return;
                }
                else
                {
                    // player passes
                    std::cout << *players[next_player] << " passes" << std::endl;
                }
                // otherwise, keep going around
                next_player = pass_left(next_player);
            }
        }
    }

    // MODIFIES Changes leader to the winner of the trick, modifies variable tricks to keep track of current tricks per team
    // EFFECTS Plays a trick. returns winner of the trick
    Player_Number play_trick(const Player_Number &leader, const Suit trump_suit, std::array<int, num_teams> &tricks)
    {
        // Lead
        Card led_card = players[leader]->lead_card(trump_suit);
        std::cout << led_card << " led by " << *players[leader] << std::endl;

        // Highest value card
        Card highest_value_card = led_card;
        Player_Number winner_of_the_trick = leader;

        // Rest of the players
        Player_Number next_player = pass_left(leader);
        for (int i = 1; i < num_players; i++)
        {

            // play card and write output
            Card played_card = players[next_player]->play_card(led_card, trump_suit);
            std::cout << played_card << " played by " << *players[next_player] << std::endl;

            // keep track of highest value card
            if (Card_less(highest_value_card, played_card, led_card, trump_suit))
            {
                highest_value_card = played_card;
                winner_of_the_trick = next_player;
            }
            next_player = pass_left(next_player);
        }

        // Scoring the Trick
        std::cout << *players[winner_of_the_trick] << " takes the trick" << std::endl;
        std::cout << std::endl;

        Team_Number winning_team = get_team(winner_of_the_trick);
        tricks[winning_team] += 1;

        return winner_of_the_trick;
    }

    void play_hand()
    {
        Suit trump_suit;
        std::array<int, num_teams> tricks = {0}; // tricks for players 0 and 2 (index 0) and for players 1 and 3 (index 1)
        Team_Number ordered_up_team;             // team that ordered up
        Card upcard;

        // shuffle
        if (shuffle_between_hands)
        {
            pack.shuffle();
        }
        pack.reset();

        // deal
        deal(upcard);

        // make trump
        make_trump(upcard, trump_suit, ordered_up_team);

        // playing the tricks
        Player_Number leader = pass_left(dealer);
        for (int i = 0; i < 5; i++)
        {
            // winner of the trick is the next leader
            leader = play_trick(leader, trump_suit, tricks);
        }

        // scoring the hand
        if (tricks[ordered_up_team] >= 3)
        {
            // ordered up team wins
            std::cout << team_names(ordered_up_team) << " win the hand" << std::endl;
            if (tricks[ordered_up_team] == 5)
            {
                std::cout << "march!" << std::endl;
                points[ordered_up_team] += 2;
            }
            else
            {
                points[ordered_up_team] += 1;
            }
        }
        else
        {
            // non ordered up team wins
            std::cout << team_names(other_team(ordered_up_team)) << " win the hand" << std::endl;
            std::cout << "euchred!" << std::endl;
            points[other_team(ordered_up_team)] += 2;
        }
    }
};

void print_usage()
{
    std::cout << "Usage: euchre.exe PACK_FILENAME [shuffle|noshuffle] "
              << "POINTS_TO_WIN NAME1 TYPE1 NAME2 TYPE2 NAME3 TYPE3 "
              << "NAME4 TYPE4" << std::endl;
}

int main(int argc, char **argv)
{
    // 12 arguments
    if (argc != 12)
    {
        print_usage();
        return 1;
    };

    // read filename
    std::string pack_filename = argv[1];
    std::ifstream pack_input(pack_filename);
    if (!pack_input.is_open())
    {
        std::cout << "Error opening " << pack_filename << std::endl;
        return 2;
    }

    // shuffling
    bool shuffle;
    std::string shuffle_in = argv[2];
    if (shuffle_in == "shuffle")
    {
        shuffle = true;
    }
    else if (shuffle_in == "noshuffle")
    {
        shuffle = false;
    }
    else
    {
        print_usage();
        return 3;
    }

    // points to win game
    int points_to_win = std::stoi(argv[3]);
    if (points_to_win < 1 || points_to_win > 100)
    {
        print_usage();
        return 4;
    }

    // read in players
    const int num_players = 4;
    std::array<std::string, num_players> names;
    std::array<std::string, num_players> strategies;
    for (int i = 0; i < num_players; i++)
    {
        names[i] = argv[4 + 2 * i];
        strategies[i] = argv[5 + 2 * i];
        if (strategies[i] != "Simple" && strategies[i] != "Human")
        {
            print_usage();
            return 5 + i;
        }
    }

    // Print executable and arguments
    for (int i = 0; i < argc; i++)
    {
        std::cout << argv[i] << " ";
    }
    std::cout << std::endl;

    // Play Game
    Game game(pack_input, shuffle, points_to_win, names, strategies);
    game.play();
}

// play as human
// ./euchre.exe pack.in shuffle 3 Ivan Simple Judea Simple Kunle Simple Brian Human 