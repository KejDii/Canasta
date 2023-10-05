#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <chrono>

bool draw_no_rep(int rand, int tab[], int count)
{
    if (count <= 0)
        return false;

    int i = 0;
    do
    {
        if (tab[i] == rand) {
            
            return true;
        }
        i++;

    } while (i < count);

    return false;
}

struct keys {
    bool playfield_confirmed[2];
    int points_thresholds[2];
    bool block;
};

int screenbounds(int in) {
    if (in > 1920) {
        in = 1920;
        return in;
    }
    else if (in < 0) {
        in = 0;
        return in;
    }
    else {
        return in;
    }
}

class Card {
private:
    int id;
    int color;
    int points;
    bool selected;
    sf::Sprite card_img;
    sf::Texture* card_texture;
    sf::IntRect* card_rect;
public:
    Card(sf::Texture& c_texture,int in_id,int in_color);
    Card();
    void wind_draw(sf::RenderWindow& window);
    void setPosition(sf::Vector2f pos);
    void select();
    void rotate(float angle);
    int getPoints();
    int getId();
    int getColor();
    float getRotation();
    bool getSelected();
    sf::Vector2f getPosition();
};

Card::Card(sf::Texture& c_texture, int in_id, int in_color) {
    selected = false;
    card_texture = &c_texture;
    id = in_id;
    color = in_color;

    if (id == 0) {
        points = 50;
    }
    else if (id > 3 && id < 8) {
        points = 5;
    }
    else if (id == 3) {
        if(color==0||color==2){
            points = 100;
        }
        else {
            points = -100;
        }
    }
    else if (id >= 8 && id < 14) {
        points = 10;
    }
    else {
        points = 20;
    }

    if (id != 0) {
        card_rect = new sf::IntRect(1 + (id - 2) * 317 + (id - 2) * 26, 1 + color * 449 + color * 31, 317, 449);
    }
    else {
        card_rect = new sf::IntRect(4458, 3, 317, 449);
    }
    card_img.setTexture(*card_texture);
    card_img.setTextureRect(*card_rect);
    card_img.setScale(sf::Vector2f(0.3, 0.3));
    
}

Card::Card() {

}

void Card::rotate(float angle) {
    card_img.setRotation(angle);
}

float Card::getRotation() {
    return card_img.getRotation();
}

void Card::setPosition(sf::Vector2f pos) {
    card_img.setPosition(pos);
}

void Card::wind_draw(sf::RenderWindow& window) {
    window.draw(card_img);
}

void Card::select() {
    if (selected == false) {
        selected = true;
    }
    else {
        selected = false;
    }
}

int Card::getPoints() {
    return points;
}

int Card::getId() {
    return id;
}

int Card::getColor() {
    return this->color;
}

bool Card::getSelected() {
    return selected;
}

sf::Vector2f Card::getPosition() {
    return this->card_img.getPosition();
}

//TO DO NEXT :  ogólnie raczej game class koñczenie


class Player {
private:
    std::vector<Card>* deck;
    std::vector<Card>* discarded;
    std::vector<std::vector<std::vector<Card>>>* playfield;
    std::vector<Card>* thirds;
    std::vector<Card> hand;
    std::vector<Card> selected_cards;
    std::vector<std::vector<Card>>* closed;
    Card bought;
    int id;
    int team;
    int mouse_flag;
    float mouse_pos_x;
    float mouse_pos_y;
    bool draw_card_bool;
    bool duel;
    bool first_move;
    bool boughtdraw;
    bool next_move;
    keys* key_bools;
    sf::Clock delay;
    sf::Clock mouse_delay;
    sf::Texture cards_texture;
public:
    Player();
    Player(std::vector<Card>& in_deck, std::vector<Card>& in_discarded, std::vector<std::vector<std::vector<Card>>>& inplayfield, std::vector<Card>inthirds[], std::vector<std::vector<Card>> inclosed[], int in_id, bool is_game_duel, keys& in_keybools, sf::Texture& incards_texture);
    void draw_card();
    void hunt_card();
    void sort_hand();

    void move(sf::Event& event, sf::RenderWindow& window, int& player_pointer,int&taskflag);
    void mouse(sf::Event& event, sf::RenderWindow& window);

    void reset();
    void window_draw_hand(sf::RenderWindow& window);
    int getSizeHand();
    void setFirstMoveBoolTrue();
    bool playfield_points_check();
    bool ifbuy();
    std::vector<Card> getHand();
};

Player::Player() {

}

Player::Player(std::vector<Card>& in_deck, std::vector<Card>& in_discarded, std::vector<std::vector<std::vector<Card>>>& inplayfield, std::vector<Card>inthirds[], std::vector<std::vector<Card>> inclosed[], int in_id,bool is_game_duel, keys& in_keybools,sf::Texture&incards_texture) {
    mouse_flag = -1;
    deck = &in_deck;
    playfield = &inplayfield;
    discarded = &in_discarded;
    id = in_id;
    team = id % 2;
    duel = is_game_duel;
    first_move = false;
    draw_card_bool = true;
    mouse_pos_x = 0;
    mouse_pos_y = 0;
    cards_texture = incards_texture;
    thirds = inthirds;
    closed = inclosed;
    key_bools = &in_keybools;
    boughtdraw = false;
    next_move = true;
}

void Player::draw_card() {

    if (deck[0].size() > 0) {

        while (deck[0].back().getId() == 3 && (deck[0].back().getColor() == 0 || deck[0].back().getColor() == 2)) {
            thirds[team].push_back(deck[0].back());
            deck[0].pop_back();
            
        }

        hand.push_back(deck[0][deck[0].size() - 1]);
        deck[0].pop_back();
    }
    

}

void Player::hunt_card() {
    int counter = 0;
    std::mt19937 mt{ static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
        ) };

    std::uniform_int_distribution<> randsize{ 0,106 };
    counter = randsize(mt);
    int side = counter;
   

    for (int i = 0; i < 3; i++) {
        if (deck[0][counter].getId() == 0 || deck[0][counter].getId() == 2) {
            hand.push_back(deck[0][counter]);
            
            deck[0].erase(deck[0].begin() + counter);
        }
        else {
            
            counter++;
        }
    }
    counter = 0;
    while (counter<side) {
        deck[0].push_back(deck[0][0]);
        deck[0].erase(deck[0].begin());
        counter++;
    }

 

}

void Player::sort_hand() {
    for (int i = 0; i < hand.size(); i++) {
        for (int j = 0; j < hand.size() - 1; j++) {
            if (hand[j].getId() > hand[j + 1].getId()) {
                std::swap(hand[j], hand[j + 1]);
            }
        }
    }
   

}

void Player::mouse(sf::Event& event, sf::RenderWindow& window) {
    mouse_flag = -1;
    if (mouse_delay.getElapsedTime().asSeconds() > 0.2) {
        if (event.type == sf::Event::MouseButtonPressed) {
            switch (team) {
            case 0:
                std::cout << "PLAYER1\n";
                if (event.key.code == sf::Mouse::Left) {
                    mouse_pos_x = sf::Mouse::getPosition(window).x;
                    mouse_pos_y = sf::Mouse::getPosition(window).y;
                    std::cout << mouse_pos_x << " " << mouse_pos_y << "\n";
                    std::cout << "ALERT KART < " << (960 - 47 * (hand.size() / 2) + 47 * hand.size() + 47) << " >\n";
                    //draw card 
                    if ((mouse_pos_x > 908 && mouse_pos_x < 1001) && (mouse_pos_y < 483 && mouse_pos_y>349)) {
                        mouse_flag = 0;

                    }
                    //from discarded
                    else if ((mouse_pos_x > 909 && mouse_pos_x < 1003) && (mouse_pos_y > 550 && mouse_pos_y < 683)) {
                        mouse_flag = 1;

                    }
                    //select card
                    else if (((mouse_pos_x > screenbounds(960 - 47 * (hand.size() / 2))) && (mouse_pos_x < screenbounds((960 - 47 * (hand.size() / 2) + 47 * hand.size() + 47)))) && mouse_pos_y > 870) {
                        mouse_flag = 2;

                    }
                    //playfield add
                    else if ((mouse_pos_x > 115 && mouse_pos_x < 789) && (mouse_pos_y > 138 && mouse_pos_y < 854)) {
                        mouse_flag = 3;

                    }
                    //add to closed
                    else if ((mouse_pos_x > 805 && mouse_pos_x < 899) && (mouse_pos_y > 350 && mouse_pos_y < 800)) {
                        mouse_flag = 5;
                    }
                }
                //reset 
                else if (event.key.code == sf::Mouse::Right) {
                    mouse_pos_x = sf::Mouse::getPosition(window).x;
                    mouse_pos_y = sf::Mouse::getPosition(window).y;
                    if ((mouse_pos_x > 115 && mouse_pos_x < 789) && (mouse_pos_y > 138 && mouse_pos_y < 854)) {
                        mouse_flag = 4;

                    }

                }
                mouse_delay.restart();
                break;

            case 1:
                std::cout << "PLAYER2\n";
                if (event.key.code == sf::Mouse::Left) {
                    mouse_pos_x = sf::Mouse::getPosition(window).x;
                    mouse_pos_y = sf::Mouse::getPosition(window).y;
                    std::cout << mouse_pos_x << " " << mouse_pos_y << "\n";
                    //draw card 
                    if ((mouse_pos_x > 908 && mouse_pos_x < 1001) && (mouse_pos_y < 483 && mouse_pos_y>349)) {
                        mouse_flag = 0;

                    }
                    //from discarded
                    else if ((mouse_pos_x > 909 && mouse_pos_x < 1003) && (mouse_pos_y > 550 && mouse_pos_y < 683)) {
                        mouse_flag = 1;

                    }
                    //select card
                    else if (((mouse_pos_x > screenbounds(960 - 47 * (hand.size() / 2))) && (mouse_pos_x < screenbounds((960 - 47 * (hand.size() / 2) + 47 * hand.size() + 47)))) && mouse_pos_y < 182) {
                        mouse_flag = 2;

                    }
                    //playfield add
                    else if ((mouse_pos_x > 1920-789 && mouse_pos_x < 1920-115) && (mouse_pos_y > 138 && mouse_pos_y < 854)) {
                        mouse_flag = 3;

                    }
                    //add to closed
                    else if ((mouse_pos_x > 1920-899 && mouse_pos_x < 1920-805) && (mouse_pos_y > 350 && mouse_pos_y < 800)) {
                        mouse_flag = 5;
                    }
                }
                //reset 
                else if (event.key.code == sf::Mouse::Right) {
                    mouse_pos_x = sf::Mouse::getPosition(window).x;
                    mouse_pos_y = sf::Mouse::getPosition(window).y;
                    if ((mouse_pos_x > 1920-789 && mouse_pos_x < 1920-115) && (mouse_pos_y > 138 && mouse_pos_y < 854)) {
                        mouse_flag = 4;

                    }

                }
                mouse_delay.restart();
                break;

            }

        }
    }

}

void Player::move(sf::Event& event, sf::RenderWindow& window,int &player_pointer, int& taskflag) {

    //delay when player changes
    if (next_move) {
        mouse_delay.restart();
        next_move = false;
    }

    mouse(event, window);
    
    if(delay.getElapsedTime().asSeconds()>1){
        bool playfield_add = false;
        int counter_norm_id = 0;
        int counter_jok_id = 0;
        int cardid = int((mouse_pos_x - (960 - 47 * (hand.size() / 2))) / 47);
        switch(mouse_flag){

        case 0:
            std::cout << "\ncase0\n";
            if (draw_card_bool == true) {
                //if top is red 3 draw one more
                if (deck[0].back().getId() == 3 && (deck[0].back().getColor() == 0 || deck[0].back().getColor() == 2)) {
                    thirds[team].push_back(deck[0].back());
                    deck[0].pop_back();
                    first_move = false;
                    delay.restart();
                }
                //else draw normally
                else {
                    hand.push_back(deck[0].back());
                    deck[0].pop_back();
                    sort_hand();
                    delay.restart();
                    first_move = false;
                    draw_card_bool = false;
                }

            }
            break;

        case 1:
            std::cout << "\ncase1\n";
            if (draw_card_bool == true) {
                //first move
                if (first_move == true) {
                    if (discarded[0].back().getId() == 3 && (discarded[0].back().getColor() == 0 || discarded[0].back().getColor() == 2)) {
                        thirds[team].push_back(discarded[0].back());
                        discarded[0].pop_back();
                        first_move = false;
                        draw_card_bool = true;
                        std::cout << "TROJA";
                    }
                    else {

                        hand.push_back(discarded[0].back());
                        discarded[0].pop_back();
                        sort_hand();
                        
                        first_move = false;
                        draw_card_bool = false;
                    }
                }
                else if (ifbuy()) {
                    bought = discarded[0].back();
                    bought.setPosition(sf::Vector2f(909, 730));
                    boughtdraw = true;
                    selected_cards.push_back(bought);
                    discarded[0].pop_back();
                    draw_card_bool = false;
                    
                }



            }
            //discard card
            
            else if (selected_cards.size() == 1) {
                //ifplayfield points >threshold or == 0 or closed > 0
                int playfield_points = 0;
                for (int i = 0; i < playfield[0][team].size(); i++) {
                    for (int j = 0; j < playfield[0][team][i].size(); j++) {
                        playfield_points += playfield[0][team][i][j].getPoints();
                    }
                }
                //finish game
                if (hand.size() == 1 && closed[team].size() > 0) {
                    discarded[0].push_back(selected_cards.back());
                    for (int i = 0; i < hand.size(); i++) {
                        if (hand[i].getSelected() == true) {
                            hand.erase(hand.begin() + i);
                        }
                    }
                    selected_cards.clear();
                    taskflag = 2;

                }
                //normal discard
                else if ((playfield_points_check() || playfield_points == 0) && !boughtdraw) {
                    if (selected_cards.back().getId() == 2 || selected_cards.back().getId() == 0) {
                        key_bools->block = true;
                    }

                    discarded[0].push_back(selected_cards.back());
                    for (int i = 0; i < hand.size(); i++) {
                        if (hand[i].getSelected() == true) {
                            hand.erase(hand.begin() + i);
                        }
                    }
                    selected_cards.clear();
                    player_pointer++;
                    if (player_pointer == 2) {
                        player_pointer = 0;
                    }
                    draw_card_bool = true;
                }
                next_move = true;
                discarded[0].back().select();

            }
            mouse_delay.restart();
            break;
            
        case 2:
            std::cout << "\ncase2\n";
            if (draw_card_bool == false) {

                if (mouse_pos_x > hand[hand.size() - 1].getPosition().x && mouse_pos_x < hand[hand.size() - 1].getPosition().x + 94) {

                    cardid = hand.size() - 1;

                }
                //switch after team, positions of cards
                switch (team) {
                case 0:
                    if (hand[cardid].getSelected() == true && mouse_pos_y > 870) {
                        hand[cardid].select();

                        //remove from selected vector
                        for (int i = 0; i < selected_cards.size(); i++) {
                            if (selected_cards[i].getPosition().x == hand[cardid].getPosition().x) {
                                selected_cards.erase(selected_cards.begin() + i);
                            }
                        }

                    }
                    else if (mouse_pos_y > 920 && ((selected_cards.size() < hand.size() - 1 || hand.size() == 1) || (selected_cards.size() == hand.size() - 1 && boughtdraw))) {
                        if (selected_cards.size() == 0) {
                            hand[cardid].select();
                            //add to selected vector
                            selected_cards.push_back(hand[cardid]);
                        }
                        else {
                            bool add = false;
                            for (int i = 0; i < selected_cards.size(); i++) {
                                //if 2 or joker or same id card then true
                                if (selected_cards[i].getId() == 2 || selected_cards[i].getId() == 0 || selected_cards[i].getId() == hand[cardid].getId() || hand[cardid].getId() == 2 || hand[cardid].getId() == 0) {
                                    add = true;
                                }
                                //else false and break from for loop
                                else {
                                    add = false;
                                    break;
                                }
                            }
                            if (add == true) {
                                hand[cardid].select();
                                //add to selected vector
                                selected_cards.push_back(hand[cardid]);
                            }

                        }
                    }
                    break;
                case 1:
                    if (hand[cardid].getSelected() == true && mouse_pos_y < 182) {
                        hand[cardid].select();

                        //remove from selected vector
                        for (int i = 0; i < selected_cards.size(); i++) {
                            if (selected_cards[i].getPosition().x == hand[cardid].getPosition().x) {
                                selected_cards.erase(selected_cards.begin() + i);
                            }
                        }

                    }
                    else if (mouse_pos_y < 132 && ((selected_cards.size() < hand.size() - 1 || hand.size() == 1) || (selected_cards.size() == hand.size() - 1 && boughtdraw))) {
                        if (selected_cards.size() == 0) {
                            hand[cardid].select();
                            //add to selected vector
                            selected_cards.push_back(hand[cardid]);
                        }
                        else {
                            bool add = false;
                            for (int i = 0; i < selected_cards.size(); i++) {
                                //if 2 or joker or same id card then true
                                if (selected_cards[i].getId() == 2 || selected_cards[i].getId() == 0 || selected_cards[i].getId() == hand[cardid].getId() || hand[cardid].getId() == 2 || hand[cardid].getId() == 0) {
                                    add = true;
                                }
                                //else false and break from for loop
                                else {
                                    add = false;
                                    break;
                                }
                            }
                            if (add == true) {
                                hand[cardid].select();
                                //add to selected vector
                                selected_cards.push_back(hand[cardid]);
                            }

                        }
                    }
                    break;
                }


                window_draw_hand(window);

            }

            break;

        case 3:
            std::cout << "\ncase3\n";
            if (draw_card_bool == false) {
                if (selected_cards.size() <= 7) {
                    bool append = false;
                    for (int i = 0; i < playfield[0][team].size(); i++) {
                        if ((mouse_pos_x - playfield[0][team][i][0].getPosition().x > 0 && mouse_pos_x - playfield[0][team][i][0].getPosition().x < 94) && (mouse_pos_y - playfield[0][team][i][0].getPosition().y > 0 && mouse_pos_y - playfield[0][team][i][0].getPosition().y < 210)) {
                            int idcard = -1;
                            int selectedid = -1;
                            int selected_count_jok = 0;
                            int selected_count_norm = 0;
                            //idcheck playfield
                            for (int j = 0; j < playfield[0][team][i].size(); j++) {
                                if (playfield[0][team][i][j].getId() != 0 && playfield[0][team][i][j].getId() != 2) {
                                    idcard = playfield[0][team][i][j].getId();
                                    break;
                                }
                                else {
                                    idcard = 0;
                                }
                            }
                            //idcheck selected
                            for (int j = 0; j < selected_cards.size(); j++) {
                                if (selected_cards[j].getId() != 0 && selected_cards[j].getId() != 2) {
                                    selectedid = selected_cards[j].getId();
                                    break;
                                }
                                else {
                                    selectedid = 0;
                                }
                            }
                            //counting
                            for (int j = 0; j < playfield[0][team][i].size(); j++) {
                                if (playfield[0][team][i][j].getId() != 0 && playfield[0][team][i][j].getId() != 2) {
                                    counter_norm_id++;
                                }
                                else {
                                    counter_jok_id++;
                                }
                            }
                            for (int j = 0; j < selected_cards.size(); j++) {
                                if (selected_cards[j].getId() != 0 && selected_cards[j].getId() != 2) {
                                    selected_count_norm++;
                                }
                                else {
                                    selected_count_jok++;
                                }
                            }

                            //appending playfield
                            //if idcheck correct
                            if (idcard == selectedid || selectedid == 0) {
                                if (idcard == 0 || (counter_norm_id + selected_count_norm > counter_jok_id + selected_count_jok)) {
                                    //if bought then check if theres block , else normal append
                                    if (boughtdraw) {
                                        if (!key_bools->block) {
                                            for (int k = 0; k < selected_cards.size(); k++) {
                                                playfield[0][team][i].push_back(selected_cards[k]);
                                            }
                                            append = true;
                                            selected_cards.clear();
                                            int handsize = 0;
                                            while (handsize < hand.size()) {
                                                if (hand[handsize].getSelected() == true) {
                                                    hand.erase(hand.begin() + handsize);
                                                }
                                                else {
                                                    handsize++;
                                                }
                                            }
                                        }
                                    }
                                    else {
                                        for (int k = 0; k < selected_cards.size(); k++) {
                                            playfield[0][team][i].push_back(selected_cards[k]);
                                        }
                                        append = true;
                                        selected_cards.clear();
                                        int handsize = 0;
                                        while (handsize < hand.size()) {
                                            if (hand[handsize].getSelected() == true) {
                                                hand.erase(hand.begin() + handsize);
                                            }
                                            else {
                                                handsize++;
                                            }
                                        }
                                    }
                                }
                            }


                        }
                    }
                    counter_norm_id = 0;
                    counter_jok_id = 0;

                    
                    if (!append) {
                        int idcard = -1;
                        //count not 2 or joker
                        for (int i = 0; i < selected_cards.size(); i++) {
                            if (selected_cards[i].getId() != 0 && selected_cards[i].getId() != 2) {
                                counter_norm_id++;
                                cardid = selected_cards[i].getId();
                            }
                            else {

                                counter_jok_id++;

                            }
                        }
                        //if >=2 cards selected and there is more of other cards than 2 or joker
                        if (cardid != 3) {
                            if (counter_norm_id >= 2 && selected_cards.size() >= 3) {
                                if (counter_norm_id > counter_jok_id) {

                                    playfield_add = true;
                                }
                            }
                            else if (counter_norm_id == 0 && counter_jok_id > 2) {
                                playfield_add = true;
                            }
                        }
                        if (playfield_add == true) {
                            playfield[0][team].push_back(selected_cards);
                            selected_cards.clear();
                            int handsize = 0;
                            while (handsize < hand.size()) {
                                if (hand[handsize].getSelected() == true) {
                                    hand.erase(hand.begin() + handsize);
                                }
                                else {
                                    handsize++;
                                }
                            }

                        }
                    }

                }
            }
            playfield_points_check();
            //after buying
            if ((boughtdraw && selected_cards.size() == 0) && playfield_points_check()) {
                boughtdraw = false;
                key_bools->block = false;
                while (discarded[0].size() != 0) {
                    hand.push_back(discarded[0].back());
                    discarded[0].pop_back();
                }
                sort_hand();
               
            }


            break;

        case 4:
            std::cout << "\ncase4\n";
            if (!key_bools->playfield_confirmed[team]) {
                for (int i = 0; i < playfield[0][team].size(); i++) {
                    for (int j = 0; j < playfield[0][team][i].size(); j++) {
                        hand.push_back(playfield[0][team][i][j]);
                    }
                }
                playfield[0][team].clear();
                for (int i = 0; i < hand.size(); i++) {
                    if (hand[i].getSelected() == true) {
                        hand[i].select();
                    }
                }
                sort_hand();

                //if buying, then give it back to discarded and draw card
                if (boughtdraw) {
                    boughtdraw = false;
                    for (int i = 0; i < hand.size(); i++) {
                        if (hand[i].getId() == bought.getId() && hand[i].getColor() == bought.getColor()) {
                            hand.erase(hand.begin() + i);
                            std::cout << "ZBREKAOWANE";
                            break;

                        }
                    }
                    discarded[0].push_back(bought);
                    draw_card_bool = true;

                }
                std::cout << "KUPIONE " << bought.getId() << " " << bought.getColor() << "\n";

            }
            break;

        case 5:
            std::cout << "\ncase5\n";
            if (selected_cards.size() > 0) {
                //if no joker selected
                bool add_closed = true;
                int canasta_id = -1;
                for (int i = 0; i < selected_cards.size(); i++) {
                    if (selected_cards[i].getId() == 0 || selected_cards[i].getId() == 2) {
                        add_closed = false;
                        
                    }
                    else {
                        canasta_id = selected_cards[i].getId();
                    }
                }
                if (add_closed == true) {
                    //ADDING TO CLOSED CANASTAS
                    for (int j = 0; j < closed[team].size(); j++) {
                        for (int k = 0; k < closed[team][j].size(); k++) {
                            if (closed[team][j][k].getId() == canasta_id) {
                                for (int s = 0; s < selected_cards.size(); s++) {
                                    closed[team][j].push_back(selected_cards[s]);
                                    
                                }
                                selected_cards.clear();
                                int handsize = 0;
                                while (handsize < hand.size()) {
                                    if (hand[handsize].getSelected() == true) {
                                        hand.erase(hand.begin() + handsize);
                                    }
                                    else {
                                        handsize++;
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }

            break;
        }
       

        mouse_flag = -1;
        
    }
}

void Player::reset() {
    hand.clear();
    selected_cards.clear();
    draw_card_bool = true;
}
//wybieranie kart z reki na 4 graczy, PRZEDE WSZYSTKIM ROTACJA KART rêka -> stó³/odrzucone
void Player::window_draw_hand(sf::RenderWindow& window) {
    switch (id) {
    case 0:
        for (int i = 0; i < hand.size(); i++) {
            if (hand[i].getSelected() == false) {
                hand[i].setPosition(sf::Vector2f(960 - 47 * (hand.size() / 2) + i * 47, 970));
            }
            else {
                hand[i].setPosition(sf::Vector2f(960 - 47 * (hand.size() / 2) + i * 47, 920));
            }
            hand[i].wind_draw(window);
        }

        break;

    case 1:
        if (duel == true) {
            for (int i = 0; i < hand.size(); i++) {
                if (!hand[i].getSelected()) {
                    hand[i].setPosition(sf::Vector2f(960 - 47 * (hand.size() / 2) + i * 47, 2));
                }
                else {
                    hand[i].setPosition(sf::Vector2f(960 - 47 * (hand.size() / 2) + i * 47, 52));
                }
                hand[i].wind_draw(window);
            }
        }
        else {
            for (int i = 0; i < hand.size(); i++) {
                if (!hand[i].getSelected()) {
                    hand[i].setPosition(sf::Vector2f(100, 540 - 35 * (hand.size() / 2) -47+ i * 35));
                   
                }
                else {
                    hand[i].setPosition(sf::Vector2f(140, 540 - 35 * (hand.size() / 2) -47+ i * 35));
                }
                if (hand[i].getRotation() != 90) {
                    hand[i].rotate(90);
                }
                hand[i].wind_draw(window);
            }
        }
        break;

    case 2:
        for (int i = 0; i < hand.size(); i++) {
            if (!hand[i].getSelected()) {
                hand[i].setPosition(sf::Vector2f(960 - 47 * (hand.size() / 2) + i * 47, 2));
            }
            else {
                hand[i].setPosition(sf::Vector2f(960 - 47 * (hand.size() / 2) + i * 47, 52));
            }
            hand[i].wind_draw(window);
        }
        break;

    case 3:
        for (int i = 0; i < hand.size(); i++) {
            if (!hand[hand.size() - 1 - i].getSelected()) {
                hand[hand.size() - 1 - i].setPosition(sf::Vector2f(1820, 540 - 35 * (hand.size() / 2)+47 + i * 35));

            }
            else {
                hand[hand.size() - 1 - i].setPosition(sf::Vector2f(1780, 540 - 35 * (hand.size() / 2)+47 + i * 35));
            }
            if (hand[hand.size() - 1 - i].getRotation() != 270) {
                hand[hand.size() - 1 - i].rotate(270);
            }
            hand[i].wind_draw(window);
        }
        break;

    }
    if (boughtdraw) {
        bought.wind_draw(window);
    }


}

int Player::getSizeHand() {
    return hand.size();
}

void Player::setFirstMoveBoolTrue() {
    this->first_move = true;
}

bool Player::playfield_points_check() {
    //if already done
    if (key_bools->playfield_confirmed[team]) {
        std::cout << "RAZ";
        return true;
    }
    //else count points
    int pts = 0;
    for (int i = 0; i < playfield[0][team].size(); i++) {
        for (int j = 0; j < playfield[0][team][i].size(); j++) {
            pts += playfield[0][team][i][j].getPoints();
        }
    }
    if (pts >= key_bools->points_thresholds[id]) {
        key_bools->playfield_confirmed[id] = true;
        std::cout << "DWA";
        return true;
    }
    else {
        return false;
    }
}

bool Player::ifbuy() {
    //joker or 2 or one card on hand return false
    if (discarded[0].back().getId() == 0 || discarded[0].back().getId() == 2 || discarded[0].back().getId() == 3 || hand.size() == 1) {
        return false;
    }
    //count if possible to buy
    int onhand = 0;
    int onplayfield = 0;
    for (int i = 0; i < hand.size(); i++) {
        if (hand[i].getId() == discarded->back().getId()) {
            onhand++;
        }
    }
    for (int i = 0; i < playfield[0][team].size(); i++) {
        for (int j = 0; j < playfield[0][team][i].size(); j++) {
            if (playfield[0][team][i][j].getId() == discarded->back().getId()) {
                onplayfield++;
            }
        }
    }
    //if possible return true else false
    if ((onhand >= 2 && hand.size() > 2) || (onplayfield >= 2 && !key_bools->block)) {
        return true;
    }
    else {
        return false;
    }
       
  

}

std::vector<Card> Player::getHand() {
    return this->hand;
}


class Interface {
private:
    sf::Texture cards_texture;
    sf::Font font;

    std::vector<Card>* discarded;
    std::vector<std::vector<std::vector<Card>>>* playfield;
    std::vector<Card>* closed;
    std::vector<Card>* thirds;

    sf::Sprite top_deck;
    sf::IntRect card_rect;
    sf::Text deck_size;
    sf::Text discarded_size;

public:
    Interface(std::vector<Card>& discard, std::vector<std::vector<std::vector<Card>>>& inplayfield,std::vector<Card>inthirds[], std::vector<Card> inclosed[]);
    void draw(sf::RenderWindow& window);
    void update(int deck_size);

};

Interface::Interface(std::vector<Card>& discard, std::vector<std::vector<std::vector<Card>>>& inplayfield, std::vector<Card>inthirds[], std::vector<Card> inclosed[]) {
    cards_texture.loadFromFile("Resources/cards.png");
    font.loadFromFile("Resources/Arialn.ttf");

    discarded = &discard;
    playfield = &inplayfield;

    card_rect.top = 964;
    card_rect.left = 4457;
    card_rect.height = 449;
    card_rect.width = 316;
    top_deck.setTexture(cards_texture);
    top_deck.setTextureRect(card_rect);
    top_deck.setScale(0.3, 0.3);
    top_deck.setPosition(908, 350);

    deck_size.setFont(font);
    deck_size.setCharacterSize(30);
    deck_size.setFillColor(sf::Color::White);
    deck_size.setOutlineColor(sf::Color::Black);
    deck_size.setOutlineThickness(1);
    deck_size.setPosition(sf::Vector2f(943, 300));

    discarded_size.setFont(font);
    discarded_size.setCharacterSize(30);
    discarded_size.setFillColor(sf::Color::White);
    discarded_size.setOutlineColor(sf::Color::Black);
    discarded_size.setOutlineThickness(1);
    discarded_size.setPosition(sf::Vector2f(950, 700));
    
    thirds = inthirds;
    closed = inclosed;
    std::cout << thirds[0].size();
}

void Interface::update(int deck_size) {
    this->deck_size.setString(std::to_string(deck_size));
    this->discarded_size.setString(std::to_string(discarded[0].size()));
    if (deck_size < 10) {
        this->deck_size.setPosition(sf::Vector2f(950, 300));
    }
    if (discarded[0].size() > 9) {
        discarded_size.setPosition(sf::Vector2f(943,700));
    }
    else {
        discarded_size.setPosition(sf::Vector2f(950, 700));
    }


    if (discarded[0].size() > 0) {
        discarded[0][discarded[0].size() - 1].setPosition(sf::Vector2f(908, 550));
    }
    

    //playfield
    for (int d = 0; d < 2; d++) {
        int p = -1;
        for (int i = 0; i < playfield[0][d].size(); i++) {

            int k = i % 6;
            if (k == 0) {
                p++;
            }

            for (int j = 0; j < playfield[0][d][i].size(); j++) {
              
                playfield[0][d][i][j].setPosition(sf::Vector2f(125 + k * 105 + d * 1040, 118 + 30 * j + p * 280));

            }
        }
    }

    //thirds
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < thirds[i].size(); j++) {
            thirds[i][j].setPosition(sf::Vector2f(805+i*205,185+j*30));
        }
    }
    
    //closed
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < closed[i].size(); j++) {
            closed[i][j].setPosition(sf::Vector2f(805 + i * 205, 350 + j * 70));
        }
    }
    

}

void Interface::draw(sf::RenderWindow& window) {
   //125 118 start, +105 next 6 w jednej linii +30 w pionie do 268 - 6 karta,+150 w pionie od 7 karty na now¹ kupkê, ostatnia kolumna zostawiona na zamkniête, druga po³owa sto³u zaczyna siê od 1165
   /* Card karta(cards_texture, 3, 0);
    Card karta2(cards_texture, 3, 1);
    karta.setPosition(sf::Vector2f(1165, 550));
    karta2.setPosition(sf::Vector2f(780, 550));
    karta2.wind_draw(window);
    karta.wind_draw(window);*/
    window.draw(top_deck);
    window.draw(deck_size);
    window.draw(discarded_size);
    if (discarded[0].size() > 0) {
        discarded[0][discarded[0].size() - 1].wind_draw(window);
    }

    for (int team = 0; team < 2; team++) {
        for (int i = 0; i < playfield[0][team].size(); i++) {
            for (int j = 0; j < playfield[0][team][i].size(); j++) {
                playfield[0][team][i][j].wind_draw(window);
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < thirds[i].size(); j++) {
            thirds[i][j].wind_draw(window);
           
        }
    }
    //closed
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < closed[i].size(); j++) {
            closed[i][j].wind_draw(window);
        }
    }
    
}

//powrót do menu

class Game {
private:
    std::vector<Card> deck;
    std::vector<Card> discarded;
    std::vector<std::vector<std::vector<Card>>>playfield;
    std::vector<std::vector<Card>> closed[2];
    std::vector<Card> closed_interface[2];
    std::vector<Card> thirds[2];

    Player* players;
    Interface* interface;
    int taskflag;
    int menuflag;
    int player_pointer;
    int global_player_pointer;
    int number_players;
    int points[2];
    keys key_bools;

    sf::RenderWindow* window;
    sf::RectangleShape background1;
    sf::RectangleShape background2;
    sf::Texture cards_texture;

    sf::Clock mouse_delay;
    sf::Text* mainscreen_texts;
    sf::Text title;
    sf::Texture menu_texture;
    sf::Texture table_screen_txt;
    sf::Font font;
    sf::Sprite menu_background;
    sf::Sprite table_screen;
    int menu_text_counter;
    int menu_text_number;
public:
    Game();
    //cards functions
    void shuffle();
    void dealcards();

    //functioning
    //game tasking
    void tasking(sf::Event& event);
    void start();
    void close_canasta();
    void reset();
    void count_points();

    //menu
    void menu();
    void menu_draw();
    void text_select(sf::Event&event);

    //other
    void wind_draw();
    void gameloop();
   
};

Game::Game() {
    playfield.resize(2);
    
    
    
    interface = new Interface(discarded, playfield,thirds,closed_interface);

    background1.setFillColor(sf::Color(179, 110, 41));
    background1.setSize(sf::Vector2f(1920, 1080));

    background2.setFillColor(sf::Color::Green);
    background2.setSize(sf::Vector2f(1920 - 1920 * 0.12, 1080 - 1080 * 0.2));
    background2.setPosition(sf::Vector2f(1920 * 0.06, 1080 * 0.1));

    window = new sf::RenderWindow(sf::VideoMode(1920, 1080),"WELCOME TO CANASTA");
    cards_texture.loadFromFile("Resources/cards.png");

    number_players = 4;
    players = new Player[number_players];

    key_bools.playfield_confirmed[0] = false;
    key_bools.playfield_confirmed[1] = false;
    key_bools.points_thresholds[0] = 50;
    key_bools.points_thresholds[1] = 50;
    key_bools.block = false;
    for (int i = 0; i < number_players; i++) {

        if (number_players == 2) {
            players[i] = Player(deck, discarded, playfield, thirds, closed, i, true,key_bools, cards_texture);
        }
        else {
            players[i] = Player(deck, discarded, playfield, thirds, closed, i, false,key_bools, cards_texture);
        }
    }
        
    taskflag = 0;
    player_pointer = 0;
    global_player_pointer = 0;
   
    points[0] = 0;
    points[1] = 0;

    //menu
    menuflag = 0;
    menu_texture.loadFromFile("Resources/back.jpg");
    
    font.loadFromFile("Resources/Arialn.ttf");
    mainscreen_texts = new sf::Text[4];
    menu_text_number = 4;
    menu_text_counter = -1;
    for (int i = 0; i < menu_text_number; i++) {
        mainscreen_texts[i].setFont(font);
        mainscreen_texts[i].setCharacterSize(80);
        mainscreen_texts[i].setFillColor(sf::Color::White);
        mainscreen_texts[i].setOutlineColor(sf::Color::Black);
        mainscreen_texts[i].setOutlineThickness(3);
        mainscreen_texts[i].setPosition(sf::Vector2f(160,150 + i * 200));
    }
    mainscreen_texts[0].setString("PLAY");
    mainscreen_texts[1].setString("LOAD");
    mainscreen_texts[2].setString("EXTRAS");
    mainscreen_texts[3].setString("QUIT");

    menu_background.setTexture(menu_texture);
   // menu_background.setColor(sf::Color(150, 255, 255));
    menu_background.setScale(sf::Vector2f(0.75, 0.75));
   
    title.setCharacterSize(100);
    title.setString("WELCOME TO CANASTA");
    title.setFillColor(sf::Color(255,255,255));
    title.setFont(font);
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(3);
    title.setPosition(sf::Vector2f(500, 30));
    
    
    table_screen_txt.loadFromFile("Resources/table_screen.png");
    table_screen.setTexture(table_screen_txt);
    table_screen.setScale(sf::Vector2f(0.5, 0.5));
    table_screen.setPosition(sf::Vector2f(680, 330));

}

void Game::shuffle(){
    deck.clear();

    std::mt19937 mt{ static_cast<std::mt19937::result_type>(
        std::chrono::steady_clock::now().time_since_epoch().count()
        ) };

    std::uniform_int_distribution<> randsize{ 0,109 };

    std::vector<Card> deckbase;
  
    //karo trefl serce pik 0 1 2 3
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < 4; i++) {
            for (int j = 2; j < 15; j++) {
                deckbase.emplace_back(cards_texture, j, i);
            }
        }
        for (int q = 0; q < 3; q++) {
            deckbase.emplace_back(cards_texture, 0, 0);
        }
    }

    int* tab = new int[110];
    int counter = 0;
    while (counter != 110) {
        int rand = randsize(mt);
        if (draw_no_rep(rand, tab, counter) == false) {
            tab[counter] = rand;
            counter++;
        }

    }
    
    for (int i = 0; i < 110; i++) {
        deck.push_back(deckbase[tab[i]]);
        
    }

    


    
    delete[] tab;
}

void Game::dealcards() {
    int flag = 0;
    int counter = player_pointer;
    

    while (flag!=number_players) {
        flag = 0;
        if (players[counter].getSizeHand() != 13) {

            players[counter].draw_card();

            for (int i = 0; i < number_players; i++) {
                if (players[i].getSizeHand() == 13) {
                    flag++;
                }
            }
        }
        counter++;
        if (counter > number_players - 1) {
            counter = 0;
        }
    }
    
  
    //std::cout << "\n\n" << players[0].getSizeHand() << " " << players[1].getSizeHand() << " " << deck.size();
  


}

void Game::wind_draw() {
    window->draw(background1);
    window->draw(background2);
    interface->draw(*window);
    for (int i = 0; i < number_players; i++) {
        players[i].window_draw_hand(*window);
    }

    
}

void Game::start() {
    shuffle();
    players[player_pointer].hunt_card();
    dealcards();

    discarded.push_back(deck.back());
    deck.pop_back();
    
    players[global_player_pointer].setFirstMoveBoolTrue();
    
    interface->update(deck.size());
    for (int i = 0; i < number_players; i++) {
        players[i].sort_hand();
    }
 
    

    

    taskflag = 1;
}

void Game::close_canasta() {
    int count_norm = 0;
    int count_jok = 0;
    int canasta_id = -1;

    for (int i = 0; i < playfield.size(); i++) {
        for (int j = 0; j < playfield[i].size(); j++) {
            if (playfield[i][j].size() >= 7) {
                for (int k = 0; k < playfield[i][j].size(); k++) {
                    if (playfield[i][j][k].getId() != 0 && playfield[i][j][k].getId() != 2) {
                        count_norm++;
                        canasta_id = playfield[i][j][k].getId();
                    }
                    else {
                        count_jok++;
                    }
                }

                if (canasta_id == -1) {
                    canasta_id = 0;
                }

                closed[i].push_back(playfield[i][j]);
                if (canasta_id != 0) {
                    if (count_jok > 0) {
                        closed_interface[i].emplace_back(cards_texture, canasta_id, 1);
                    }
                    else {
                        closed_interface[i].emplace_back(cards_texture, canasta_id, 2);
                    }
                }
                else {
                    closed_interface[i].emplace_back(cards_texture, canasta_id, 0);
                }

                playfield[i].erase(playfield[i].begin() + j);

            }
        }
    }


}

void Game::tasking(sf::Event& event) {
    switch (taskflag) {
    case 0:
        //game start
        start();
        break;
    case 1:
        //player move
        players[player_pointer].move(event, *window, player_pointer,taskflag);
        interface->update(deck.size());
        close_canasta();
        break;
    case 2:
        //finish game
        reset();
        start();
        break;

    }
    //escape/return to menu check
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::RControl) {
            menuflag = 0;
        }
    }


}

void Game::reset() {
    count_points();

    global_player_pointer++;
    if (global_player_pointer >= number_players) {
        global_player_pointer = 0;
    }
    player_pointer = global_player_pointer;

    deck.clear();
    discarded.clear();
    std::cout << "discarded PRZESZLO";
    thirds[0].clear();
    thirds[1].clear();
    std::cout << "thirds PRZESZLO";
    closed_interface[0].clear();
    closed_interface[1].clear();
    std::cout << "CLOSEDinterace PRZESZLO";
    closed[0].clear();
    closed[1].clear();
    std::cout << "CLOSED PRZESZLO";
    playfield[0].clear();
    playfield[1].clear();
    std::cout << "PFIELD PRZESZLO";

    for (int i = 0; i < number_players; i++) {
        players[i].reset();
    }
    std::cout << "players PRZESZLO";
    //new thresholds
    for (int i = 0; i < 2; i++) {
        key_bools.playfield_confirmed[i] = false;
        if (points[i] >= 3000) {
            key_bools.points_thresholds[i] = 150;
        }
        else if (points[i] >= 1500) {
            key_bools.points_thresholds[i] = 90;
        }
        else if (points[i] >= 0) {
            key_bools.points_thresholds[i] = 50;
        }
        else {
            key_bools.points_thresholds[i] = 15;
        }

    }


    key_bools.block = false;

    
}

void Game::count_points() {
    //finish points
    points[player_pointer % 2] += 200;
    std::cout << "raz";

    for (int i = 0; i < playfield.size(); i++) {
        for (int j = 0; j < playfield[i].size(); j++) {
            for (int k = 0; k < playfield[i][j].size(); k++) {
                //points on playfield
                if (closed[i].size() > 0) {
                    points[i] += playfield[i][j][k].getPoints();
                }
                else {
                    points[i] -= playfield[i][j][k].getPoints();
                }
            }
        }
        for (int j = 0; j < closed[i].size(); j++) {
            int clean = 0;
            for (int k = 0; k < closed[i][j].size(); k++) {
                //points for cards in closed canasta
                points[i] += closed[i][j][k].getPoints();
                if (closed[i][j][k].getId() != 0 && closed[i][j][k].getId() != 2) {
                    clean++;
                }
            }
            //points for canasta
            if (clean == 7) {
                points[i] += 600;
            }
            else if (clean == 0) {
                points[i] += 1000;
            }
            else {
                points[i] += 300;
            }
        }

        if (thirds[i].size() == 4) {
            points[i] += 1000;
        }
        else {
            points[i] += thirds[i].size() * 100;
        }



    }
    std::cout << "dwa";
    for (int i = 0; i < number_players; i++) {
        std::vector<Card> handcards = players[i].getHand();
        for (int j = 0; j < players[i].getSizeHand(); j++) {
            if (handcards[j].getPoints() != -100) {
                points[i%2] -= handcards[j].getPoints();
            }
            else {
                points[i%2] += handcards[j].getPoints();
            }
        }
    }
    std::cout << "trzy";
    std::cout << "pierwsza druzyna - " << points[0] << " druga druzyna - " << points[1] << "\n";
}

void Game::menu() {
    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        window->clear();

        
        switch (menuflag) {

        case 0:
            //startscreen
            text_select(event);
            menu_draw();
            
            break;
        case 1:
            //play
            tasking(event);
            wind_draw();
            break;
        case 2:
            //load
            break;
        case 3:
            //extras
            break;
        case 4:
            //quit
            window->close();
            break;
        }
        window->display();
    }
}

void Game::menu_draw() {
    switch (menuflag) {
    case 0:
        window->draw(menu_background);
        for (int i = 0; i < menu_text_number; i++) {
            window->draw(mainscreen_texts[i]);
        }
        window->draw(title);
        window->draw(table_screen);
        
        break;
    }
}

void Game::text_select(sf::Event& event) {
    if (mouse_delay.getElapsedTime().asSeconds() > 0.05) {
        if (event.type == sf::Event::MouseMoved) {

            float mouse_pos_x = sf::Mouse::getPosition(*window).x;
            float mouse_pos_y = sf::Mouse::getPosition(*window).y;
            //std::cout << "x - " << mouse_pos_x << "y - " << mouse_pos_y << "\n";
            for (int i = 0; i < menu_text_number; i++) {
                float txt_pos_x = mainscreen_texts[i].getPosition().x;
                float txt_pos_y = mainscreen_texts[i].getPosition().y;
                if (mouse_pos_x > txt_pos_x && mouse_pos_x < txt_pos_x + 254 && mouse_pos_y>txt_pos_y && mouse_pos_y < txt_pos_y + 60) {
                    mainscreen_texts[i].setCharacterSize(120);
                    mainscreen_texts[i].setFillColor(sf::Color::Cyan);
                    menu_text_counter = i;
                }
                else {
                    mainscreen_texts[i].setCharacterSize(80);
                    mainscreen_texts[i].setFillColor(sf::Color::White);
                    if (menu_text_counter == i) {
                        menu_text_counter = -1;
                    }
                }
            }
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.key.code == sf::Mouse::Left) {
                menuflag = menu_text_counter + 1;
            }
        }
        mouse_delay.restart();
    }
}

void Game::gameloop() {
    

    while (window->isOpen())
    {
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        window->clear();


        tasking(event);
        wind_draw();
       


        window->display();
    }
}

int main()
{
   
    Game gra;
    gra.menu();

    return 0;
}