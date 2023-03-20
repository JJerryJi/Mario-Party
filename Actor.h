#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <string>

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// 0, 90, 180, 270 / right, up, left, down

class Actor: public GraphObject{
public:
    // set Sprite Direction to right
    Actor(int imageID, int startX, int startY, int depth, StudentWorld* world): GraphObject(imageID, startX, startY, 0, depth, 1.0){
        // spirte_direction default to zero(RIGHT) for all actors
        m_world = world;
        m_alive = true;
    }
    
    virtual ~Actor(){}
    virtual bool can_be_hit_by_vortex() = 0;
    virtual void doSomething() = 0;
    virtual bool is_a_square() = 0;
    
    virtual void hit_by_vortex(){}; // tell an game object it has been hit by a vortex
    
    virtual bool can_inform_player(std::string& msg){return false;}
  
    virtual void setToDeath(){m_alive = false;}
    virtual bool getAliveOrDeath(){return m_alive;}
    StudentWorld* getWorld(){ return m_world;}
    
    
private:
    StudentWorld* m_world;
    bool m_alive;
};

// Only Moving Actors: Avatar, Boo, Bowser, (Vortex) can have ** moving_direction **
class Moving_Actor: public Actor{
public:
    Moving_Actor(int imageID, int startX, int startY, int depth, StudentWorld* world): Actor(imageID, startX, startY, depth, world){
        moving_direction = right;
    }
    virtual ~Moving_Actor(){}
    bool isAtFork();
    void setMovingDirection(int direction){ moving_direction = direction; }
    int getMovingDirection() {return moving_direction;}
    void selectValid_RandomMovingDirection();
    void findANewValidDirection();
private:
    int moving_direction;
};



class Avatar : public Moving_Actor {
public:
    // spec specifies:
    // 1 for Peach, 2 for Yoshi
    
    Avatar(int imageID, int startX, int startY, int which_player, StudentWorld* m_world): Moving_Actor(imageID, startX, startY, 0, m_world)
    {
        m_playerNumber = which_player;
        m_numCoins = 0;
        m_numStars = 0;
        m_hasVortex = false;
        m_ticksToMove = 0;
        m_state = 0;
        firstMoveOrNot = true;
        m_teleported = false;
    }
    
    virtual bool can_be_hit_by_vortex(){return false;}
    virtual ~Avatar(){}
    
    virtual void doSomething();
    
    virtual bool is_a_square(){ return false;}
    
    int getCoin(){return m_numCoins;}
    
    // add appropriate amount
    void setCoinByValue(int value){
        m_numCoins+=value;
        
        // m_numCoins will never be negative
        if(m_numCoins<=0){
            m_numCoins = 0;
        }
    }
    void setCoinToZERO(){ m_numCoins = 0; }
    
    int getStar(){return m_numStars;}
    
    void setStarByValue(int value) {
        m_numStars+=value;
        // m_numStars will never be negative
        if(m_numStars < 0)
            m_numStars = 0;
    }
    
    void setStarsToZERO(){m_numStars = 0;}
    
    // waiting to roll : 0
    // walking_state: 1
    int getState() {return m_state; }
    
    void setState(int value) {m_state = value;}
    
    int getTicksToMove() {return m_ticksToMove; }
    
    void setTicksToMove(int value) {m_ticksToMove = value; }
    
    void teleportToAnotherSquare();
    
    void setVortex(){
        m_hasVortex = true;
    }
    
    void deleteVortexFromPlayer(){
        m_hasVortex = false;
    }
    bool hasVortex(){
        return m_hasVortex == true;
    }
    
//    bool get_just_swapped() {
//        return m_just_swapped;
//    }
//
//    void set_just_swapped(bool value){
//        m_just_swapped = value;
//    }
    
    bool get_firstMoveOrNot(){
        return firstMoveOrNot;
    }
    
    void set_firstMoveOrNot(bool value){
        firstMoveOrNot = value;
    }
    
private:
   // 1 for Peach, 2 for Yoshi
    int m_playerNumber;
    int m_numCoins;
    int m_numStars;
    bool m_hasVortex;
    int m_ticksToMove;
    // waiting to roll : 0
    // walking_state: 1
    int m_state;
    bool firstMoveOrNot;
    bool m_teleported;

};


class Baddies: public Moving_Actor{
public:
    Baddies(int imageID, int startX, int startY, int depth, StudentWorld* m_world): Moving_Actor(imageID, startX, startY, depth, m_world) {
        m_state = 0;
        m_pause_counter = 180;
        m_ticks_to_move  = 0;
        player_new[0] = true;
        player_new[1] = true;
    }
    virtual ~Baddies(){}
    
    virtual bool can_be_hit_by_vortex(){ return true; }
    
    // Baddies is not a square
    virtual bool is_a_square(){ return false; }
    
    int getState(){ return m_state; }
    
    void setState(int value){ m_state = value; }
    
    int getPauseCounter(){ return m_pause_counter; }
    
    void setPauseCounterByOne(){ m_pause_counter -= 1; }
    
    void ResetPauseCounterTo180(){ m_pause_counter = 180; }
    
    int getTicksToMove() { return m_ticks_to_move; }
    
    void setTicksToMove(int value){ m_ticks_to_move = value; }
    
    void setTicksToMoveByOne(){ m_ticks_to_move -= 1; }

    bool checkPlayerNewlyArrive(int i){
        i-=1;
        return player_new[i];
    }
    
    // 1 Peach
    // 2 Yoshi
    void setPlayerNewlyArrive(int i, bool value) {
        i-=1;
        player_new[i] = value;
    }
    
    bool isAvatarLandOnBaddies(Avatar* player);
    
    // Overwrite func when Baddies is hit by vortex
    // In other actor class, this function does nothing
    virtual void hit_by_vortex();
    
    
private:
    // POSED : 0
    // WALKING: 1
    int m_state;
    int m_pause_counter;
    int m_ticks_to_move;
    
    // 1 Peach
    // 2 Yoshi
    bool player_new[2];
};


class Bowser: public Baddies {
public:
    Bowser(int imageID, int startX, int startY, int depth, StudentWorld* m_world): Baddies(imageID, startX, startY, depth, m_world){
        // bowser may also turn the first square into a dropping square, activate for the first time
        m_firstMove = true;
    }
    
    virtual ~Bowser(){};
    virtual void doSomething();
    
private:
    bool m_firstMove;
};





class Boo: public Baddies {
public:
    Boo(int imageID, int startX, int startY, int depth, StudentWorld* m_world): Baddies(imageID, startX, startY, depth, m_world){}
    virtual ~Boo(){};
    virtual void doSomething();

private:
};



class Square: public Actor{
public:
    Square(int imageID, int startX, int startY, int depth, StudentWorld* world): Actor(imageID, startX, startY, depth, world){
        for(int i=0;i<2;i++)
            player_new[i] = true;
    }
    
    virtual ~Square(){}
    
    // all squares are squares
    virtual bool is_a_square(){ return true; }
    
    virtual bool can_be_hit_by_vortex(){ return false; }

    // check if Avatar ONLY LANDS on Square
    bool isAvatarLandOnSquare (Avatar* player){
        if(player->getState() == 0 && player->getX() == getX() && player->getY() == getY())
            return true;
        return false;
    }
    
    // check if Avatar ONLY MOVES OVER Square
    bool isAvatarMoveOverSquare (Avatar* player){
        if(player->getState() == 1 && player->getX() == getX() && player->getY() == getY())
            return true;
        return false;
    }

    // check if Avatar MOVES or LANDS on Square
    bool MoveOrLandOnSquare(Avatar* player){
        if( player->getX() == getX() && player->getY() == getY() )
           return true;
        return false;
    }
    
    // 1 Peach
    // 2 Yoshi
    // HAVE TO DECREASE i by ONE BEFORE DOING ANYTHING
    // IF you pass 1, you check if PEACH NEWLY arrives
    // if you pass 2, you check if Yoshi NEWLY arrives
    bool checkPlayerNewlyArrive(int i){
        i-=1;
        return player_new[i];
    }
    
    // 1 Peach
    // 2 Yoshi
    void setPlayerNewlyArrive(int i, bool value) {
        i-=1;
        player_new[i] = value;
    }
    
    virtual void doSomething() = 0;
    
    
private:
    
    // EVEN THOUGH THE INDEX STARTS FROM ZERO, STILL PASS 1 or 2
    // pass value represent: 
    // 1 Peach
    // 2 Yoshi
    bool player_new[2];
};



class CoinSquare: public Square{
public:
    CoinSquare(int imageID, int startX, int startY, int which_type, int depth, StudentWorld* world): Square(imageID, startX, startY, depth, world){
        m_type = which_type;
    }
    virtual ~CoinSquare(){}
    virtual void doSomething();

private:
    // Define
    // 0 for red : - 3
    // 1 for blue : + 3
    int m_type;
};



class StarSquare: public Square{
public:
    StarSquare(int imageID, int startX, int startY, int depth, StudentWorld* world): Square(imageID, startX, startY, depth, world){}
    virtual ~StarSquare(){}
    virtual void doSomething();
};


class EventSquare: public Square{
public:
    EventSquare(int imageID, int startX, int startY, int depth, StudentWorld* world): Square(imageID, startX, startY, depth, world){
        which_player_cannot_use_it_until_move_away = -1;
    }
    
    virtual ~EventSquare(){};
    virtual void doSomething();
    
private:
    int which_player_cannot_use_it_until_move_away;

};


class BankSquare: public Square{
public:
    BankSquare(int imageID, int startX, int startY, int depth, StudentWorld* world): Square(imageID, startX, startY, depth, world){}
    
    virtual ~BankSquare(){}
    
    virtual void doSomething();
    
private:
};

class DirectionalSquare : public Square {
public:
    DirectionalSquare(int imageID, int startX, int startY, int depth, int force_direction, StudentWorld* world): Square(imageID, startX, startY, depth, world){
        setDirection(force_direction);
        m_forcing_drection = force_direction;
    }
    
    virtual ~DirectionalSquare(){}
    
    virtual void doSomething(){};
    
    virtual bool can_inform_player(std::string& msg){
        msg = std::to_string(getDirection());
        return true;
    }
    
    
private:
    int m_forcing_drection;
};

class DroppingSquare: public Square{
public:
    DroppingSquare(int imageID, int startX, int startY, StudentWorld* world,  int depth = 1): Square(imageID, startX, startY, depth, world){}
    virtual ~DroppingSquare(){};
    virtual void doSomething();
};


class Vortex: public Moving_Actor{
public:
    Vortex(int imageID, int startX, int startY, int moving_dir, StudentWorld* world, int depth = 0): Moving_Actor(imageID, startX, startY, depth, world){
        setMovingDirection(moving_dir);
    }
    virtual ~Vortex(){}
    virtual void doSomething();
    virtual bool can_be_hit_by_vortex(){return false;}
    virtual bool is_a_square() {return false;}
    
private:
    
};

#endif // ACTOR_H_
