#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <vector> 
#include "Actor.h"


// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    // add a destructor
    ~StudentWorld();
    StudentWorld(std::string assetPath);
    virtual int init();
    virtual int move();
    virtual void cleanUp();

    // helper function
    std::string getSquareBeahvior(bool can_inform_player, int x, int y);
    bool checkValidPosition(Moving_Actor* player);
    bool checkValidPositionInthisDirection(Moving_Actor* player, int direction);
    int get_bank_amount() {return m_bank_amount;}
    void add_bank_amount_by_value(int value) { m_bank_amount += value; }
    void set_bank_amount_to_zero() {m_bank_amount = 0; }
    
    void create_dropping_square(int x, int y){
        actors.push_back(new DroppingSquare(IID_DROPPING_SQUARE, x, y, this));
    }
    
    void create_vortex(int x, int y, int moving_dir){
        actors.push_back(new Vortex(IID_VORTEX, x, y, moving_dir, this));
    }
    
    bool VortexHitBaddies(int VX, int VY){
        auto p = actors.begin();
        while(p!=actors.end()){
            // find Baddies
            if((*p)->can_be_hit_by_vortex()){
                int X =(*p)->getX();
                int Y = (*p)->getY();

                int X_diff = VX - X;
                int Y_diff = VY - Y;
                
                // IF Baddies is hitted (OVERLAPs)
                if(X_diff > - SPRITE_WIDTH && X_diff < SPRITE_WIDTH && Y_diff > - SPRITE_HEIGHT && Y_diff < SPRITE_HEIGHT){
                    // CALL Overwrite func on baddies' class
                    (*p)->hit_by_vortex();
                    playSound(SOUND_HIT_BY_VORTEX);
                    return true;
                }
                
            }
            p++;
        }
        return false;
    }
    
    // Return a ptr to square using Polymorphism
    Actor* choose_random_square() {
        for(;;){
            auto p = actors.begin();
            int random_number = randInt(0, actors.size()-1);
            int i=0;
            while(p!=actors.end() && i != random_number){
                p++;
                i++;
            }
            if((*p)->is_a_square())
                return *p;
            else
                continue;
        }
    }
    

    void deleteInActiveCharacters(){
        auto p = actors.begin();
        while(p!=actors.end()){
            if((*p)->getAliveOrDeath()==false){
                delete *p;
                *p = nullptr;
                p = actors.erase(p);
            }
            else {
                p ++;
            }
        }
    }
    
    void setSquareToDeath(int X, int Y){
        for(auto p = actors.begin(); p!=actors.end(); p++){
            if((*p)->is_a_square() && (*p)->getX() == X && (*p)->getY() == Y){
                (*p)->setToDeath();
            }
        }
    }
    
    void swapCoins(){
       int peach_Coin =  peach->getCoin();
        peach->setCoinToZERO();
        peach->setCoinByValue(yoshi->getCoin());
        
        yoshi->setCoinToZERO();
        yoshi->setCoinByValue(peach_Coin);
    }
    
    void swapStars(){
        int peach_Star = peach->getStar();
        peach->setStarsToZERO();
        peach->setStarByValue(yoshi->getStar());
        
        yoshi->setStarsToZERO();
        yoshi->setStarByValue(peach_Star);
    }
    
    void swapPosition(){
        int peach_tick = peach->getTicksToMove();
        int peach_X = peach->getX();
        int peach_Y = peach->getY();
        int peach_MovingDir = peach->getMovingDirection();
        int peach_Dir = peach->getDirection();
        int peach_state = peach->getState();
        int peach_first_move_or_not = peach->get_firstMoveOrNot();
        
        //
        peach->moveTo(yoshi->getX(), yoshi->getY());
        peach->setTicksToMove(yoshi->getTicksToMove());
        peach->setDirection(yoshi->getDirection());
        peach->setMovingDirection(yoshi->getMovingDirection());
        peach->setState(yoshi->getState());
        peach->set_firstMoveOrNot(yoshi->get_firstMoveOrNot());
        
        // 
        yoshi->moveTo(peach_X, peach_Y);
        yoshi->setTicksToMove(peach_tick);
        yoshi->setDirection(peach_Dir);
        yoshi->setMovingDirection(peach_MovingDir);
        yoshi->setState(peach_state);
        yoshi->set_firstMoveOrNot(peach_first_move_or_not);
    }
    
    
    // num == 1, return peach
    // num ==2, return yoshi
    
    Avatar* getPlayer(int num){
        if(num == 1) return peach;
        if(num == 2) return yoshi;
        return nullptr;
    }

    
    

private:
    std::vector<Actor*> actors;
    Avatar* yoshi;
    Avatar* peach;
    int m_bank_amount;
};

#endif // STUDENTWORLD_H_
