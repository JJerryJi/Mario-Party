#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

// All Moving_Actor need to check for forks
bool Moving_Actor:: isAtFork(){
    int count = 0;
    int direction = getMovingDirection();
    
    // if current direction is valid, add count by 1
    if(getWorld()->checkValidPositionInthisDirection(this, direction)){
        count ++ ;
    }
    
    // 4 cases
    if(direction == left || direction == right){
        if(getWorld()->checkValidPositionInthisDirection(this, up))
            count ++;
        if(getWorld()->checkValidPositionInthisDirection(this, down))
            count ++;
    }

    else if(direction == up || direction == down){
        if(getWorld()->checkValidPositionInthisDirection(this, left))
            count ++;
        if(getWorld()->checkValidPositionInthisDirection(this, right))
            count ++;
    }

    return count >= 2;
}

// USED WHEN Bowser and Boo turning around
// USED WHEN PART1 PLAYER LOGIC
void Moving_Actor:: findANewValidDirection(){
    int direction = getMovingDirection();
    if(direction == left || direction == right){
        // prefers up over down
        if(getWorld()->checkValidPositionInthisDirection(this, up)){
            setMovingDirection(up);
            setDirection(0);
        }else{
            // set sprite_direction to default value;
            setDirection(0);
            setMovingDirection(down);
        }
    }
    else if(direction == up  || direction == down){
        // prefers right over left
        setMovingDirection(right);
        if(getWorld()->checkValidPositionInthisDirection(this, right)){
            setMovingDirection(right);
            setDirection(0);
        }
        else{
            setMovingDirection(left);
            // if left, has to change sprite_direction
            setDirection(180);
        }
    }

}

void Moving_Actor:: selectValid_RandomMovingDirection(){
    int poss_dir[] = {0, 90, 180, 270};
    for(;;){
        int random = randInt(0, 3);
        if(getWorld()->checkValidPositionInthisDirection(this, poss_dir[random])){
            setMovingDirection(poss_dir[random]);
            // set Sprite Direction accordingly
            if(poss_dir[random] == left){
                setDirection(180);
            }
            else {
                setDirection(0);
            }
            break;
        }
    }
}

void Avatar:: teleportToAnotherSquare(){
    // TELEPORT TO RANDOM LOCATION
    Actor* square = getWorld()->choose_random_square();
    moveTo(square->getX(), square->getY());
    m_teleported = true;
}


void Avatar:: doSomething(){
    if(m_state == 0){
        
        // Due to teleport, choose a random direction if the current one is not valid
        if(m_teleported && !getWorld()->checkValidPositionInthisDirection(this, getMovingDirection())){
            int poss_dir[] = {0, 90, 180, 270};
            for(;;){
                int random = randInt(0, 3);
                if(getWorld()->checkValidPositionInthisDirection(this, poss_dir[random])){
                    setMovingDirection(poss_dir[random]);
                    if(poss_dir[random] == left){
                        setDirection(180);
                    }
                    else {
                        setDirection(0);
                    }
                    break;
                }
            }
            // m_teleported IS SET TO FALSE at the end to bypass the logic of FORK after teleporting to random Square
            
        }
        
        int action = getWorld()->getAction(m_playerNumber);
        
        switch(action){
            // if action is anything else
            default:
                return;
            case ACTION_FIRE:
                if(hasVortex()){
                    int nx, ny;
                    getPositionInThisDirection(getMovingDirection(), 16, nx, ny);
                    getWorld()->create_vortex(nx, ny, getMovingDirection());
                    deleteVortexFromPlayer();
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                }
                break;
            case ACTION_ROLL:
                int dice_roll = randInt(1, 10);
                m_ticksToMove = dice_roll * 8;
                m_state = 1;
                //continue with STEP 2
                break;
        }
    }
    
    // STEP 2
    if(m_state == 1){
        std::string result_direction;
        if(m_ticksToMove %  8 == 0){
            // Following the msg from *Directional Square*
            result_direction = getWorld()->getSquareBeahvior(true, getX(), getY());
            if(result_direction == "0" || result_direction == "90" || result_direction == "180" || result_direction == "270"){
                int forcing_direction = 0;
                for (int i = 0; i< result_direction.size(); i++){
                    forcing_direction = forcing_direction* 10 + (result_direction[i] - '0');
                }
                setMovingDirection(forcing_direction);
                if(forcing_direction == left){
                    setDirection(180);
                }
                else {
                    setDirection(0);
                }

            }
        }
        // Not Directional Square Case:
        if(result_direction == ""){
            // FORK logic:
            // firstMove && just_teleported are two special case
            if(!firstMoveOrNot && (m_ticksToMove %  8 == 0) && isAtFork() && m_teleported==false){
                int temp_direction;
                switch(getWorld()->getAction(m_playerNumber)){
                    case ACTION_UP:
                        temp_direction = 90;
                        break;
                    case ACTION_DOWN:
                        temp_direction = 270;
                        break;
                    case ACTION_RIGHT:
                        temp_direction = 0;
                        break;
                    case ACTION_LEFT:
                        temp_direction = 180;
                        break;
                    default:
                        return;
                }
                
                // get 3 possible direction (the reverse of the current direction is not valid)
                int cur_direction = getMovingDirection();
                int possible_direction1 = (cur_direction + 90 + 360) % 360;
                int possible_direction2 = (cur_direction - 90 + 360) % 360;
                
                // check if the temp direction is one of the three
                if(temp_direction == cur_direction || temp_direction == possible_direction1 || temp_direction == possible_direction2) {
                    // check if the temp directions is valid
                    if(getWorld()->checkValidPositionInthisDirection(this, temp_direction)){
                        // set SPRITE DIRECTION
                        if(temp_direction == left){
                            setDirection(180);
                        }else{
                            setDirection(0);
                        }
                        setMovingDirection(temp_direction);
                    }
                    
                    else{
                        return;
                    }
                }
                else{
                    return;
                }
                
            }
            
            // if cannot moving forward
            else if(m_ticksToMove %  8 == 0 && !getWorld()->checkValidPosition(this)){
                findANewValidDirection();
            }
        }
        
        m_teleported = false;
        m_ticksToMove -- ;
        firstMoveOrNot = false;
        
        // move 2 pixels at a time
        moveAtAngle(getMovingDirection(), 2);
        
         // If Players happens to land on Directional Squares:
        if(m_ticksToMove == 0){
            // return to waiting to roll status
            m_state = 0;
            // Check for Direction Square
            std::string result_direction1 = getWorld()->getSquareBeahvior(true, getX(), getY());
            if(result_direction1 == "0" || result_direction1 == "90" || result_direction1 == "180" || result_direction1 == "270"){
                int forcing_direction = 0;
                // convert direction information from string to int
                for (int i = 0; i< result_direction1.size(); i++){
                    forcing_direction = forcing_direction* 10 + (result_direction1[i] - '0');
                }
                setMovingDirection(forcing_direction);
                if(forcing_direction == left){
                    setDirection(180);
                }
                else {
                    setDirection(0);
                }
            }
        }
    }

}


void CoinSquare:: doSomething(){
    // if dead, return
    if(getAliveOrDeath() == false)
        return;
    
    // separately act on different players
    for(int i=1;i<3;i++){
        bool isPlayerLands = isAvatarLandOnSquare(getWorld()->getPlayer(i));
        // CHECK IF NEWLY ARRIVE
        if(checkPlayerNewlyArrive(i) && isPlayerLands){
            setPlayerNewlyArrive(i, false);
            switch(m_type){
                case 0:
                    getWorld()->getPlayer(i)->setCoinByValue(-3);
                    getWorld()->playSound(SOUND_TAKE_COIN);
                    break;
                case 1:
                    getWorld()->getPlayer(i)->setCoinByValue(3);
                    getWorld()->playSound(SOUND_GIVE_COIN);
                    break;
            }
        }
        // update status if player leaves
        else if(!isPlayerLands && !checkPlayerNewlyArrive(i)){
            setPlayerNewlyArrive(i, true);
        }
    }
    

}


void StarSquare:: doSomething() {
    for(int i=1;i<3;i++){
        bool isPlayerArrives = MoveOrLandOnSquare(getWorld()->getPlayer(i));
        if(checkPlayerNewlyArrive(i) && isPlayerArrives){
            setPlayerNewlyArrive(i, false);
            if(getWorld()->getPlayer(i)->getCoin()>=20){
                getWorld()->getPlayer(i)->setCoinByValue(-20);
                getWorld()->getPlayer(i)->setStarByValue(1);
                getWorld()->playSound(SOUND_GIVE_STAR);
            }else{
                continue;
            }
        }
        else if (!isPlayerArrives) {
            setPlayerNewlyArrive(i, true);
        }
    }

}

void BankSquare:: doSomething(){

    for(int i=1;i<3;i++){
        // 1 for peach
        // 2 for yoshi
        
        // LandOn Logic:  Get Money from bank
        bool player_landOn = isAvatarLandOnSquare(getWorld()->getPlayer(i));

        if(checkPlayerNewlyArrive(i) && player_landOn){
            setPlayerNewlyArrive(i,false);
            int amount_money = getWorld()->get_bank_amount();
            getWorld()->getPlayer(i)->setCoinByValue(amount_money);
            getWorld()->set_bank_amount_to_zero();
            getWorld()->playSound(SOUND_WITHDRAW_BANK);
            continue;
        }
        else if (!checkPlayerNewlyArrive(i) && !player_landOn){
            setPlayerNewlyArrive(i,true);
        }

        
        
        // MoverOver Logic:: Send out money
        bool player_moveOver = isAvatarMoveOverSquare(getWorld()->getPlayer(i));

        if(checkPlayerNewlyArrive(i) && player_moveOver) {
            setPlayerNewlyArrive(i,false);

            int player_amount = getWorld()->getPlayer(i)->getCoin();

            if(player_amount>=5) {
                getWorld()->add_bank_amount_by_value(5);
                getWorld()->getPlayer(i)->setCoinByValue(-5);
            }
            else{
                getWorld()->add_bank_amount_by_value(player_amount);
                // default set it to zero in SetCoinByValue
                getWorld()->getPlayer(i)->setCoinByValue(-5);
            }
            
            getWorld()->playSound(SOUND_DEPOSIT_BANK);
            continue;
        }
        
        
        // IF Player LEAVES (coordinates no longer match), that's not a new player
        else if(getWorld()->getPlayer(i)->getX()!=getX() || getWorld()->getPlayer(i)->getY()!=getY())
            setPlayerNewlyArrive(i,true);
    }
    
}



void EventSquare::doSomething(){
    
    for(int i=1;i<3;i++){
        
        // Prevent the Avatar that has been swapped to activate Event Square logic
        // Update if that player leaves
        if(i == which_player_cannot_use_it_until_move_away){
            if(!isAvatarLandOnSquare(getWorld()->getPlayer(i))){
                which_player_cannot_use_it_until_move_away = -1;
            }
        }
        
        // that Avatar cannot re-activate the Event Square until it moves away
        // continue to the next avatar
        if(i == which_player_cannot_use_it_until_move_away)
            continue;
        
        bool isAvatarLandOn = isAvatarLandOnSquare(getWorld()->getPlayer(i));
        if(checkPlayerNewlyArrive(i) && isAvatarLandOn){
            setPlayerNewlyArrive(i,false);
            int rand_action = randInt(1, 3);
            switch (rand_action){
                case 1:
                    getWorld()->getPlayer(i)->teleportToAnotherSquare();
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    break;
                case 2:
                    if(getWorld()->getPlayer(i)->hasVortex() == false){
                        getWorld()->getPlayer(i)->setVortex();
                    }
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                    break;
                case 3:
                    getWorld()->swapPosition();
                    getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                    // if Peach is On it
                    // update Yoshi to which_player_cannot_use_it
                    if(i == 1){
                        which_player_cannot_use_it_until_move_away = 2;
                        setPlayerNewlyArrive(2, false);
                        setPlayerNewlyArrive(1, true);
                    }
                    if(i == 2){
                        which_player_cannot_use_it_until_move_away = 1;
                        setPlayerNewlyArrive(1, false);
                        setPlayerNewlyArrive(2, true);
                    }
                    break;
            }
        }
        else if (!isAvatarLandOn){
            setPlayerNewlyArrive(i,true);
        }
        
    }
}

// When Baddies is hit, it is teleported to a random Square
void Baddies::hit_by_vortex(){
    
    Actor* square = getWorld()->choose_random_square();
    int newX = square->getX();
    int newY = square->getY();
    moveTo(newX, newY);
    ResetPauseCounterTo180();
    setState(0);
    setTicksToMove(0);
}


void Bowser::doSomething(){
    
    //Bowser can turn the INITIAL Square to Dropping Square
    if(m_firstMove){
        int randI = randInt(1, 4);
        if(randI == 1){
            getWorld()->setSquareToDeath(getX(), getY());
            getWorld()->create_dropping_square(getX(), getY());
            getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
        }
        m_firstMove = false;
    }
    
    // Paused State:
    if(getState() == 0){
        // IF Avatar Lands on Bowser, it have 50% chance to perform an action
        for(int i=1; i<3; i++){
            bool playerLandsOn = isAvatarLandOnBaddies(getWorld()->getPlayer(i));
            if(playerLandsOn && checkPlayerNewlyArrive(i)){
                setPlayerNewlyArrive(i, false);
                int rand_I = randInt(1, 2);
                if(rand_I == 1){
                    getWorld()->getPlayer(i)->setCoinToZERO();
                    getWorld()->getPlayer(i)->setStarsToZERO();
                    getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
                }
            }
        }
    
        //decrease pause counter ByOne
        setPauseCounterByOne();
        
        if(getPauseCounter() == 0){
            int square_to_move = randInt(1, 10);
            setTicksToMove(square_to_move * 8);
            // Pick a random valid direction  && set Sprite Direction as well
            selectValid_RandomMovingDirection();
            // Set to walkig state
            setState(1);
        }
    }
    
    for(int i=1; i<3; i++){
        if(checkPlayerNewlyArrive(i) == false){
            if(!isAvatarLandOnBaddies(getWorld()->getPlayer(i))){
                setPlayerNewlyArrive(i, true);
            }
        }
    }
    
    if(getState() == 1){

        
        if(getTicksToMove() % 8 == 0 && isAtFork()){
            selectValid_RandomMovingDirection();
        }

        // TURNING POINT
        else if(getTicksToMove() % 8 == 0 && !getWorld()->checkValidPositionInthisDirection(this,getMovingDirection())){
            findANewValidDirection();
        }

        moveAtAngle(getMovingDirection(), 2);
        
        // decrease ticks to move by one
        setTicksToMoveByOne();
        
        if(getTicksToMove() == 0){
            // Reset Staet & Pose Counter
            setState(0);
            ResetPauseCounterTo180();

            // DROPPING SQUARE CONSTRUCTION !!!
            int randI = randInt(1, 4);
            if(randI == 1){
                getWorld()->setSquareToDeath(getX(), getY());
                getWorld()->create_dropping_square(getX(), getY());
                getWorld()->playSound(SOUND_DROPPING_SQUARE_CREATED);
            }
        }
    }

}



void DroppingSquare::doSomething(){
    
    for(int i=1;i<3;i++){
        bool isPlayerLands = isAvatarLandOnSquare(getWorld()->getPlayer(i));
        if(checkPlayerNewlyArrive(i) && isPlayerLands){
            setPlayerNewlyArrive(i, false);
       
            int randI = randInt(1, 2);
            if(randI == 1){
                // if less than 10 coins, will automatically be zero in helper function
                getWorld()->getPlayer(i)->setStarByValue(-1);
            }else {
                // if less than 10 coins, will automatically be zero in helper function
                getWorld()->getPlayer(i)->setCoinByValue(-10);
            }
            getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
        }
        else if (!isPlayerLands && !checkPlayerNewlyArrive(i)) {
            setPlayerNewlyArrive(i, true);
        }
    }
}


bool Baddies::isAvatarLandOnBaddies(Avatar* player){
    if (player->getState() == 0 && player->getX() == getX() && player->getY() == getY()){
            return true;
    }
    return false;
}

void Boo::doSomething(){
    // PAUSED STATE
    if(getState() == 0){
        for(int i=1; i<3; i++){
            bool playerLandsOn = isAvatarLandOnBaddies(getWorld()->getPlayer(i));
            if(playerLandsOn && checkPlayerNewlyArrive(i)){
                setPlayerNewlyArrive(i, false);
                int rand_I = randInt(1, 2);
                if(rand_I == 1){
                    getWorld()->swapCoins();
                }
                else{
                    getWorld()->swapStars();
                }
                getWorld()->playSound(SOUND_BOO_ACTIVATE);
            }
            // CANNOT UPDATE PLAYER ARRIVE STATE HERE
            // NEED TO HANDLE EVERY TICK
        }

        //Decrease Pose Counter by One
        setPauseCounterByOne();

        if(getPauseCounter() == 0){
            int square_to_move = randInt(1, 3);
            setTicksToMove(square_to_move * 8);
            // Pick a random direction  && set Sprite Direction
            selectValid_RandomMovingDirection();
            // Set to walkig state
            setState(1);
        }
    }
    
    // Update PLAYER_NEWLY_ARRIVE logic
    for(int i=1; i<3; i++){
        if(checkPlayerNewlyArrive(i) == false){
            if(!isAvatarLandOnBaddies(getWorld()->getPlayer(i))){
                setPlayerNewlyArrive(i, true);
            }
        }
    }

    
    // WALKING STATE HERE:
    if(getState() == 1){
        if(getTicksToMove() % 8 == 0 && isAtFork()){
            selectValid_RandomMovingDirection();
        }
        // TURNING POINT
        else if(getTicksToMove() % 8 == 0 && !getWorld()->checkValidPositionInthisDirection(this,getMovingDirection())){
            findANewValidDirection();
        }

        //Move TWO PIXELS
        moveAtAngle(getMovingDirection(), 2);
        
        // decrease ticks to move by one
        setTicksToMoveByOne();

        if(getTicksToMove() == 0){
            setState(0);
            ResetPauseCounterTo180();
        }
    }

}

void Vortex::doSomething(){
    // if vortex is dead, immediately return
    if(getAliveOrDeath() == false){
        return;
    }
    
    // every tick it moves 2
    moveAtAngle(getMovingDirection(), 2);
    
    // if it is outside, setDeath()
    if(getX() >= VIEW_WIDTH || getX() < 0){
        setToDeath();
    }
    else if(getY() >= VIEW_WIDTH || getY() < 0){
        setToDeath();
    }
    
    // if it hits, set Vortex to Death
    if(getWorld()->VortexHitBaddies(getX(),getY())){
        setToDeath();
    }
        
    
}

