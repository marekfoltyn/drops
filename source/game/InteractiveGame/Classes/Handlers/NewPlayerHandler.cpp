#include "NewPlayerHandler.h"

#include "Game.h"
#include "Player.h"
#include "PlayerCollisionHandler.h"
#include "GameplayDefinitions.h"

bool NewPlayerHandler::execute(GameNet::Box * box)
{
    CCLOG("New player: %s", box->getData().c_str() );
    
    Player * player = Player::create(box->getAddress(), box->getData());
    game->addPlayer(player);
    
    // add to the stadium
    game->getStadiumManager()->addNewPlayer(player);
    
    if( game->playersCount() == 1 ) // only this player is in the stadium
    {
        player->setAsAdmin();
        game->getStadiumManager()->setAdminName( player->getName() );
    }
    
    return false;
}