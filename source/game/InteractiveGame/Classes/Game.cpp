//
//  Game
//  InteractiveGame
//
//  Created by Marek Foltýn on 18.02.16.
//
//

#include "GameplayDefinitions.h"

#include "Game.h"
#include "StadiumScene.h"

#include "AccelerationBoxHandler.h"
#include "NewPlayerHandler.h"
#include "DisconnectHandler.h"
#include "ExitGameHandler.h"
#include "LogHandler.h"
#include "KickHandler.h"
#include "PlayerCollisionHandler.h"
#include "GoalCollisionHandler.h"
#include "TeamSelectHandler.h"
#include "GameStateHandler.h"
#include "CountdownHandler.h"
#include "BonusHandler.h"

#include "BoxFactory.h"

Game * Game::instance = nullptr;

Game::Game()
{
    director = cocos2d::Director::getInstance();
    connector = GameNet::Connector::getInstance();
    handlerMap = HandlerMap::create();
    admin = nullptr;
    players = std::map<int, Player *>();
}


void Game::initGameState()
{
    gameState = GameState();
    gameState.set_pitchsize(GameState_PitchSize_SIZE_SMALL);
    gameState.set_duration(GameState_MatchDuration_DURATION_MEDIUM);
    gameState.set_state(GameState_State_STATE_LOBBY);
    setName(SERVER_NAME_DEFAULT);
}


Game * Game::getInstance()
{
    if(instance == nullptr)
    {
        instance = new Game();
    }
    return instance;
}



void Game::run()
{
    auto origin = director->getVisibleOrigin();
    auto visibleSize = director->getVisibleSize();
    auto directorScene = StadiumScene::createScene();
    int secondsLeft = durationToSeconds(GameState_MatchDuration_DURATION_MEDIUM);
    
    // show the scene
    director->runWithScene(directorScene);
    stadium = directorScene->getChildByTag<StadiumScene*>(StadiumScene::SCENE_TAG);
    stadium->initPitch(origin, visibleSize);

    // set up handlers and protobuf data
    registerHandlers();
    initGameState();

    // set up game and other related things
    stadium->setSecondsLeft(secondsLeft);
    stadium->showExitButton(new ExitGameHandler(this));
    playing = false;
}



void Game::end()
{
    connector->stop();
    director->end();
}



void Game::addPlayer(Player * player)
{
    int id = player->getId();
    players[id] = player;
}



void Game::removePlayer(Player * player)
{
    auto iterator = players.find( player->getId() );
    if( iterator != players.end())
    {
        std::string name = player->getName();
        players.erase(iterator);
        
        CCLOG("Player %s was removed from game", name.c_str());
    }
}



Player * Game::getPlayer(int id)
{
    auto iterator = players.find(id);
    if( iterator != players.end())
    {
        return iterator->second;
    }
    return nullptr;
}


Player * Game::getRandomPlayer()
{
    return players.begin()->second;
}


bool Game::startNetworking()
{
    // run  Connector (as a server)
    bool started = connector->startAsServer(MAX_PLAYERS);
    connector->setServerName(SERVER_NAME_DEFAULT);
    
    if(!started)
    {
        CCLOG("Server not started!");
        return false;
    }

    director->getScheduler()->schedule(CC_CALLBACK_0(Game::receiveBoxes, this),
    this, RECEIVE_TIMEOUT, CC_REPEAT_FOREVER, 0.0f, false, "receiveBoxes");
    
    return true;
}



void Game::registerHandlers()
{
    bonusHandler = new BonusHandler(this, director);
    auto logHandler = new LogHandler();
    auto disconnectHandler = new DisconnectHandler(this);
    auto playerHandler = new PlayerCollisionHandler(this);
    
    handlerMap->add(BOX_PLAYER_NAME, new NewPlayerHandler(this, director));
    handlerMap->add(BOX_ACCELERATION, new AccelerationBoxHandler(this));
    handlerMap->add(BOX_DISCONNECTED, disconnectHandler);
    handlerMap->add(BOX_CONNECTION_LOST, disconnectHandler);
    handlerMap->add(BOX_PING, logHandler);
    handlerMap->add(BOX_NEW_CONNECTION, logHandler);
    handlerMap->add(BOX_KICK, new KickHandler(this));
    handlerMap->add(BOX_TEAM_SELECT, new TeamSelectHandler(this));
    handlerMap->add(BOX_ADMIN, new GameStateHandler(this));
    
    handlerMap->add(VOID_COUNTDOWN_FINISHED, new CountdownHandler(this));
    
    stadium->addCollisionHandler(BITMASK_PLAYER, playerHandler );
    stadium->addCollisionHandler(BITMASK_INVISIBLE_PLAYER, playerHandler );
    stadium->addCollisionHandler(BITMASK_SCORE, new GoalCollisionHandler(this, director) );
    stadium->addCollisionHandler(BITMASK_BONUS, bonusHandler);
}



void Game::receiveBoxes()
{
    GameNet::Box * box;
    int type;
    bool finish = false;
    
    // c->receive() returns 0, if no received packet is in the queue
    while( (box = connector->receive()) != nullptr )
    {
        type = box->getType();
        finish = handlerMap->getBoxHandler(type)->execute(box); // Handler map returs always a valid handler
        box->deallocate();
        
        if(finish) break;
    }
}

std::string Game::getAutoTeam()
{
    int red  = 0;
    int blue = 0;
    
    // get team balance
    for(auto it = players.begin(); it != players.end(); ++it)
    {
        if(it->second->getTeam().compare(TEAM_BLUE) == 0)
        {
            blue++;
        }
        else if(it->second->getTeam().compare(TEAM_RED) == 0)
        {
            red++;
        }
    }
    
    if(red < blue)
    {
        return TEAM_RED;
    }
    else if( blue < red)
    {
        return TEAM_BLUE;
    }
    else
    {
        // randomly select team
        int random = RandomHelper::random_int(0, 1);
        return (random == 0) ? TEAM_BLUE : TEAM_RED;
    }
}



void Game::setAsAdmin(Player * player)
{
    admin = player;
    player->setAsAdmin();
    stadium->setAdminName( player->getName() );

    auto box = GameNet::BoxFactory::admin( player->getAddress(), gameState );
    box->send();
    
    CCLOG("%s", gameState.DebugString().c_str());
}



void Game::setName(std::string name)
{
    connector->setServerName(name);
    gameState.set_name(name);
    stadium->setServerName(name);
    
    CCLOG("GAME: name = %s", gameState.name().c_str());
    CCLOG("%s", gameState.DebugString().c_str());
}



void Game::setSize(GameState_PitchSize size)
{
    gameState.set_pitchsize(size);
//  stadiumManager->setSize(size);
    CCLOG("size: %d", gameState.pitchsize());
}



void Game::setDuration(GameState_MatchDuration duration)
{
    int seconds = durationToSeconds(duration);
    gameState.set_duration(duration);
    stadium->setSecondsLeft( seconds );
    CCLOG("duration: %d", gameState.duration());
}



int Game::durationToSeconds(GameState_MatchDuration duration)
{
    if(duration == GameState_MatchDuration_DURATION_SHORT)
    {
        return Definitions::TIME_MATCH_SHORT;
    }

    if(duration == GameState_MatchDuration_DURATION_MEDIUM)
    {
        return Definitions::TIME_MATCH_MEDIUM;
    }

    if(duration == GameState_MatchDuration_DURATION_LONG)
    {
        return Definitions::TIME_MATCH_LONG;
    }

    return 0;
}



void Game::setCountdownEnabled(bool enabled)
{
    if(!enabled)
    {
        stadium->unschedule(SCHEDULE_COUNTDOWN);
        return;
    }
    
    // enabled == true
    stadium->schedule([&](float dt)
    {
        int secondsLeft = stadium->getSecondsLeft();
        secondsLeft--;
        stadium->setSecondsLeft(secondsLeft);

        // countdown finished
        if(secondsLeft == 0)
        {
            handlerMap->getVoidHandler(VOID_COUNTDOWN_FINISHED)->execute();
            gameState.set_state(GameState_State_STATE_LOBBY);
            
            // update admin's game state
            auto box = GameNet::BoxFactory::admin( admin->getAddress(), gameState );
            box->send();
        }
    },
    1, CC_REPEAT_FOREVER, 0.0f, SCHEDULE_COUNTDOWN);

}



void Game::startMatch()
{
    playing = true;
    gameState.set_state(GameState_State_STATE_RUNNING);
    stadium->setSecondsLeft(durationToSeconds(gameState.duration()));
    stadium->setMatchMode(true);
    stadium->setBallInGame(true);
    stadium->resetScore();
    setCountdownEnabled(true);
    bonusHandler->setBonusDaemonEnabled(true);
}



void Game::stopMatch()
{
    playing = false;
    gameState.set_state(GameState_State_STATE_LOBBY);
    stadium->setMatchMode(false);
    stadium->setBallInGame(false);
    setCountdownEnabled(false);
    bonusHandler->setBonusDaemonEnabled(false);
}






