#include "StudentWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "Actor.h"
#include <string>
#include "Board.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

// helper function
// if the position is still valid when moving in CURRENT direction
bool StudentWorld:: checkValidPosition(Moving_Actor* player){
    int x = player->getX();
    int y = player->getY();
    int direction = player->getMovingDirection();
    
    // only checks if NEXT grid appears
    // every move 16 pixels
    
    if(direction == 0 || direction == 180){
        player->getPositionInThisDirection(direction, SPRITE_WIDTH, x, y);
    }
    else if(direction == 90 || direction == 270){
        player->getPositionInThisDirection(direction, SPRITE_HEIGHT, x, y);
    }
    
    for(auto p = actors.begin(); p!=actors.end(); p++){
        int sX = (*p)->getX();
        int sY = (*p)->getY();
        
        if(sX == x && sY == y)
            return true;
    }
    
    return false;
}

string StudentWorld::getSquareBeahvior(bool can_inform_player, int x, int y){
    string msg = "";
    
    for(auto p= actors.begin(); p!=actors.end(); p++){
        if((*p)->getX() == x && (*p)->getY() == y) {
            if((*p)->can_inform_player(msg)){
                can_inform_player = (*p)->can_inform_player(msg);
                return msg;
            }
        }
    }
    return msg;
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{

}

int StudentWorld::init()
{
    // load the board
    char number = getBoardNumber() + '0';
    string path = assetPath() + "/board0" + number + ".txt";
    Board board;
    Board::LoadResult result = board.loadBoard(path);
    
    if (result == Board::load_fail_file_not_found)
        cerr << "Could not find board01.txt data file\n";
    else if (result == Board::load_fail_bad_format)
        cerr << "Your board was improperly formatted\n";
    else if (result == Board::load_success) {
//        cerr << "Successfully loaded board\n";
    
    // initalize objects
    
    // local variables to keep bowsers
    vector<Actor*> bowsers;
        
    for (int x = 0; x < BOARD_WIDTH; x++){
        for (int y = 0; y < BOARD_HEIGHT; y++){
            Board::GridEntry ge = board.getContentsOf(x, y);
            switch(ge){
                case Board::empty:
                    break;
                case Board::red_coin_square:
                    actors.push_back(new CoinSquare(IID_RED_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 0, 1, this));
                    break;
                case Board::blue_coin_square:
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 1, this));
                    break;
                case Board::star_square:
                    actors.push_back(new StarSquare(IID_STAR_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, this));
                    break;
                case Board::event_square:
                    actors.push_back(new EventSquare(IID_EVENT_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, this));
                    break;
                case Board::bank_square:
                    actors.push_back(new BankSquare(IID_BANK_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, this));
                    break;
                case Board::up_dir_square:
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 90, this));
                    break;
                case Board::down_dir_square:
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 270, this));
                    break;
                case Board::left_dir_square:
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 180, this));
                    break;
                case Board::right_dir_square:
                    actors.push_back(new DirectionalSquare(IID_DIR_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 0, this));
                    break;
                case Board::boo:
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 1, this));
                    actors.push_back(new Boo(IID_BOO, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 0, this));
                    break;
                case Board::bowser:
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 1, this));
                    bowsers.push_back(new Bowser(IID_BOWSER, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 0, this));
                    break;
                case Board::player:
                    actors.push_back(new CoinSquare(IID_BLUE_COIN_SQUARE, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, 1, this));
                    peach = new Avatar(IID_PEACH, x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 1, this);
                    yoshi = new Avatar(IID_YOSHI,x*SPRITE_WIDTH, y*SPRITE_HEIGHT, 2, this);
                    break;
                }
            }
        }
        // Therefore, bowsers always doSomething Last after all Squares do something
        for(int i=0; i < bowsers.size();i++){
            actors.push_back(bowsers[i]);
        }
    }
    
    
    
    // initalize bank amount
    m_bank_amount = 0;
    // start the timer
    startCountdownTimer(99);
    
    // default return
    return GWSTATUS_CONTINUE_GAME;
    
}


int StudentWorld::move()
{

    ostringstream oss;
    oss.fill('0');
    oss << "P1 Roll: " << (peach->getTicksToMove() + 7) / 8  << " Stars: " << peach->getStar() << " $$: " << peach->getCoin();
    if (peach->hasVortex()) {
        oss << " VOR" ;
    }
    oss << " | Time: " << timeRemaining() << " | Bank: " << m_bank_amount << " | ";
    oss << "P2 Roll: " << (yoshi->getTicksToMove() + 7 )/ 8  << " Stars: " << yoshi->getStar() << " $$: " << yoshi->getCoin() ;
    
    if (yoshi->hasVortex()){
        oss << " VOR";
    }

    peach->doSomething();
    yoshi->doSomething();

    for(auto p = actors.begin(); p !=actors.end(); p++){
        if((*p)->getAliveOrDeath() == true)
            (*p)->doSomething();
        
    }

    

    

    
    
    string result = oss.str();
    setGameStatText(result);
    
    
    if (timeRemaining() <= 0) {
        playSound(SOUND_GAME_FINISHED);
        
        if(yoshi->getStar() > peach->getStar()){
            setFinalScore(yoshi->getStar(), yoshi->getCoin());
            return GWSTATUS_YOSHI_WON;
        }
        else if(yoshi->getStar()< peach->getStar()) {
            setFinalScore(peach->getStar(), peach->getCoin());
            return GWSTATUS_PEACH_WON;
        }
        else if(yoshi->getStar() == peach->getStar() && yoshi->getCoin() > peach->getCoin()){
            setFinalScore(yoshi->getStar(), yoshi->getCoin());
            return GWSTATUS_YOSHI_WON;
        }
        else if(yoshi->getStar() == peach->getStar() && yoshi->getCoin() < peach->getCoin()){
            setFinalScore(peach->getStar(), peach->getCoin());
            return GWSTATUS_PEACH_WON;
        }
        
        int randI = randInt(1, 2);
        
        if(randI == 1){
            setFinalScore(peach->getStar(), peach->getCoin());
            return GWSTATUS_PEACH_WON;
        }
        else{
            setFinalScore(yoshi->getStar(), yoshi->getCoin());
            return GWSTATUS_YOSHI_WON;
        }
            
    }
    
    deleteInActiveCharacters();
    
	return GWSTATUS_CONTINUE_GAME;
}



void StudentWorld::cleanUp(){
    
    for (auto p = actors.begin(); p !=actors.end(); p++){
        delete *p;
        *p = nullptr;
    }
    
    if (yoshi != nullptr){
        delete yoshi;
        yoshi = nullptr;
    }
    if (peach != nullptr){
        delete peach;
        peach = nullptr;
    }
    
}


StudentWorld::~StudentWorld(){
    cleanUp();
}




bool StudentWorld::checkValidPositionInthisDirection(Moving_Actor* player, int direction){
    int x = player->getX();
    int y = player->getY();
    
    // only checks if NEXT grid appears
    // every move 16 pixels
    
    if(direction == 0 || direction == 180){
        player->getPositionInThisDirection(direction, SPRITE_WIDTH, x, y);
    }
    else if(direction == 90 || direction == 270){
        player->getPositionInThisDirection(direction, SPRITE_HEIGHT, x, y);
    }

    for(auto p = actors.begin(); p!=actors.end(); p++){
        int sX = (*p)->getX();
        int sY = (*p)->getY();
        if(sX == x && sY == y){
            return true;
        }
    }
    return false;

}
