#ifndef  _BonusHandler_H_
#define  _BonusHandler_H_

#include "Box.h"
#include "AbstractHandlers.h"
#include "StadiumScene.h"
#include "BonusInterface.h"
#include <string>
class Game;

/**
 * combined handler - implements three events:
 * 1) bonus generation - try to generate a bonus and place it into the stadium
 * 2) bonus collision - take the bonus and apply effect
 */
class BonusHandler: public VoidHandler, public CollisionHandler{
public:
    
    BonusHandler();
    
    /**
     * generation and placement
     * - try to generate - random generation
     */
    void execute();
    
    /**
     * take the bonus and apply the effect
     */
    void execute(PhysicsBody * first, PhysicsBody * second);
    
private:
    
    Game * game;
    StadiumScene * scene;
    Director * director;
    
    BonusInterface * generateBonus();

    Vec2 generatePosition();
    
};

#endif // _BonusHandler_H_