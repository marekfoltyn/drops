//
//  StadiumManager
//  InteractiveGame
//
//  Created by Marek Foltýn on 05.02.16.
//
//

#include "GameplayDefinitions.h"
#include "StadiumManager.h"
#include "AbstractHandlers.h"
#include "PlayerCollisionHandler.h"
#include "cocos2d.h"
#include "KickBonus.h"

USING_NS_CC;

StadiumManager * StadiumManager::create()
{
    return new StadiumManager();
}



StadiumManager::StadiumManager()
{
    this->scene = nullptr;
    this->director = cocos2d::Director::getInstance();
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
}



void StadiumManager::drawPitch()
{
    auto visibleSize = director->getVisibleSize();
    auto origin = director->getVisibleOrigin();
    auto c = GameNet::Connector::getInstance();
    
    // background color
    //auto background = cocos2d::LayerColor::create(COLOR_GREEN);
    auto background = Sprite::create("grass.png");
    background->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    scene->addChild(background);
    
    // center circle
    auto line = Sprite::create("center.png");
    line->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    scene->addChild(line);
    
    float scaleX = visibleSize.width  / 100.0 - 2.0*BORDER/100.0;
    float scaleY = visibleSize.height / 100.0 - 2.0*BORDER/100.0;
    
    // center line
    line = Sprite::create("line.png");
    line->setScaleX(scaleY);
    line->setRotation(90);
    line->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));
    scene->addChild(line);
    
    
    // top line
    line = Sprite::create("line.png");
    line->setScaleX(scaleX);
    line->setAnchorPoint(Vec2(0,1));
    line->setPosition(Vec2( origin.x + BORDER, origin.y + visibleSize.height - BORDER));
    scene->addChild(line);
    
    // bottom line
    line = Sprite::create("line.png");
    line->setScaleX(scaleX);
    line->setAnchorPoint(Vec2(0,0));
    line->setPosition(Vec2( origin.x + BORDER, origin.y + BORDER));
    scene->addChild(line);
    
    // left line
    line = Sprite::create("line.png");
    line->setScaleX(scaleY);
    line->setRotation(90);
    line->setAnchorPoint(Vec2(0,0));
    line->setPosition(Vec2( origin.x + BORDER, origin.y + visibleSize.height - BORDER));
    scene->addChild(line);
    
    // right line
    line = Sprite::create("line.png");
    line->setScaleX(scaleY);
    line->setRotation(90);
    line->setAnchorPoint(Vec2(0,1));
    line->setPosition(Vec2( origin.x + visibleSize.width - BORDER, origin.y + visibleSize.height - BORDER ));
    scene->addChild(line);
    
    scaleX = visibleSize.width/800; // line.jpg is 100px wide
    scaleY *= 0.5;
    
    // left lines near the goal
    line = Sprite::create("line.png");
    line->setScaleX(scaleX);
    line->setAnchorPoint(Vec2(0,1));
    line->setPosition(Vec2( origin.x + BORDER, origin.y + visibleSize.height/2 + scaleY*100/2 ));
    scene->addChild(line);
    line = Sprite::create("line.png");
    line->setScaleX(scaleX);
    line->setAnchorPoint(Vec2(0,1));
    line->setPosition(Vec2( origin.x + BORDER, origin.y + visibleSize.height/2 - scaleY*100/2 ));
    scene->addChild(line);
    line = Sprite::create("line.png");
    line->setScaleX(scaleY);
    line->setRotation(90);
    line->setAnchorPoint(Vec2(0,1));
    line->setPosition(Vec2( origin.x + BORDER + scaleX*100, origin.y + visibleSize.height/2 + scaleY*100/2 ));
    scene->addChild(line);
    
    // right lines near the goal
    line = Sprite::create("line.png");
    line->setScaleX(scaleX);
    line->setAnchorPoint(Vec2(0,1));
    line->setPosition(Vec2( origin.x + visibleSize.width - BORDER - scaleX*100, origin.y + visibleSize.height/2 + scaleY*100/2 ));
    scene->addChild(line);
    line = Sprite::create("line.png");
    line->setScaleX(scaleX);
    line->setAnchorPoint(Vec2(0,1));
    line->setPosition(Vec2( origin.x + visibleSize.width - BORDER - scaleX*100, origin.y + visibleSize.height/2 - scaleY*100/2 ));
    scene->addChild(line);
    line = Sprite::create("line.png");
    line->setScaleX(scaleY);
    line->setRotation(90);
    line->setAnchorPoint(Vec2(0,0));
    line->setPosition(Vec2( origin.x + visibleSize.width - BORDER - scaleX*100, origin.y + visibleSize.height/2 + scaleY*100/2 ));
    scene->addChild(line);
    
    // left goal
    auto goal = Sprite::create("goal.png");
    goal->setScale(SCALE_GOAL);
    goal->setPosition(Vec2( origin.x, origin.y + visibleSize.height/2));
    scene->addChild(goal, 2);
    
    // right goal
    goal = Sprite::create("goal.png");
    goal->setRotation(180);
    goal->setScale(SCALE_GOAL);
    goal->setPosition(Vec2( origin.x + visibleSize.width, origin.y + visibleSize.height/2));
    scene->addChild(goal, 2);
    
    // goal physics
    // 4 horizontal lines (each goal has two lines - top and down)
    for(int i=0; i<4; i++)
    {
        auto top = Node::create();
        auto body = PhysicsBody::createBox(cocos2d::Size( SCALE_GOAL*goal->getContentSize().width/2, 10), MATERIAL_SOLID);
        body->setDynamic(false);
        body->setCategoryBitmask(BITMASK_SOLID);
        body->setCollisionBitmask(BITMASK_SOLID | BITMASK_BALL | BITMASK_PLAYER);
        body->setContactTestBitmask(BITMASK_SOLID | BITMASK_BALL | BITMASK_PLAYER);
        top->setPhysicsBody(body);
        top->setPosition(Vec2(
                              origin.x + (i>1)*(visibleSize.width) + (1-(i>1)*2)*0.25*SCALE_GOAL*goal->getContentSize().width,
                              origin.y + visibleSize.height/2 + (1 - 2*(i%2)) * SCALE_GOAL*(goal->getContentSize().height/2
                                                                                            )));
        scene->addChild(top);
    }
    // score point detectors
    for(int i=0; i<2; i++)
    {
        auto top = Node::create();
        auto body = PhysicsBody::createBox(cocos2d::Size(
            SCALE_GOAL*goal->getContentSize().width - 2*Sprite::create("ball.png")->getContentSize().width*SCALE_BALL,
            SCALE_GOAL*goal->getContentSize().height - Sprite::create("ball.png")->getContentSize().width*SCALE_BALL));
        body->setDynamic(false);
        body->setCategoryBitmask(BITMASK_SCORE);
        body->setCollisionBitmask(0);
        body->setContactTestBitmask(BITMASK_BALL);
        top->setAnchorPoint(Vec2( i, 0.5 ));
        top->setPhysicsBody(body);
        top->setTag(i); // LEFT and RIGHT (#defined)
        top->setPosition(Vec2(
                              origin.x + (i%2)*(visibleSize.width),
                              origin.y + visibleSize.height/2
                              ));
        scene->addChild(top);
    }
    
    
    // physics boundary
    auto edgeBody = PhysicsBody::createEdgeBox(cocos2d::Size(visibleSize.width + 4*BORDER, visibleSize.height + 4*BORDER), MATERIAL_SOLID, BORDER);
    edgeBody->setDynamic(false);
    edgeBody->setCategoryBitmask(BITMASK_SOLID);
    edgeBody->setCollisionBitmask(BITMASK_SOLID | BITMASK_BALL | BITMASK_PLAYER);
    edgeBody->setContactTestBitmask(BITMASK_SOLID | BITMASK_BALL | BITMASK_PLAYER);
    auto edgeNode = Node::create();
    edgeNode->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    edgeNode->setPhysicsBody(edgeBody);
    scene->addChild(edgeNode);
    
    // ball-only physics boundary
    edgeBody = PhysicsBody::createEdgeBox(cocos2d::Size(visibleSize.width, visibleSize.height), MATERIAL_SOLID, BORDER);
    edgeBody->setDynamic(false);
    edgeBody->setCategoryBitmask(BITMASK_BALL_BOUNDARY);
    edgeBody->setCollisionBitmask(BITMASK_BALL);
    edgeBody->setContactTestBitmask(BITMASK_BALL);
    edgeNode = Node::create();
    edgeNode->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    edgeNode->setPhysicsBody(edgeBody);
    scene->addChild(edgeNode);
    
    // 4 anti-corner boxes
    for(int i=0; i<4; i++)
    {
        auto box = PhysicsBody::createBox(cocos2d::Size( 90, 90 ));
        box->setDynamic(false);
        box->setCategoryBitmask(BITMASK_BALL_BOUNDARY);
        box->setCollisionBitmask(BITMASK_BALL);
        box->setContactTestBitmask(BITMASK_BALL);
        edgeNode = Node::create();
        edgeNode->setPosition(Vec2( origin.x + (i%2) * visibleSize.width, origin.y + (i>=2) * visibleSize.height ));
        edgeNode->setPhysicsBody(box);
        edgeNode->setRotation(45);
        scene->addChild(edgeNode);
    }
    
    // 4 anti corner boxes near goals
    for(int i=0; i<4; i++)
    {
        auto box = PhysicsBody::createBox(cocos2d::Size( 70, 70 ));
        box->setDynamic(false);
        box->setCategoryBitmask(BITMASK_BALL_BOUNDARY);
        box->setCollisionBitmask(BITMASK_BALL);
        box->setContactTestBitmask(BITMASK_BALL);
        edgeNode = Node::create();
        edgeNode->setPosition(Vec2( origin.x + (i%2) * visibleSize.width, origin.y + visibleSize.height/2 + (((i>=2)*2)-1) * goal->getContentSize().height*SCALE_GOAL/2 ));
        edgeNode->setPhysicsBody(box);
        edgeNode->setRotation(45);
        scene->addChild(edgeNode);
    }
    
    
    auto * txtServerName = TextFieldTTF::createWithTTF(c->getServerName(), "Vanilla.ttf", 64);
    txtServerName->setColor(Color3B(54, 72, 99));
    txtServerName->setAnchorPoint(Vec2(0.5, 0.5));
    txtServerName->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height - txtServerName->getContentSize().height ));
    //this->addChild(txtServerName);
    
    // score labels
    auto left = Label::createWithTTF("0", "Vanilla.ttf", 100);
    float circle = Sprite::create("center.png")->getContentSize().width/4;
    left->setPosition(Vec2( - circle + origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    left->setTextColor(COLOR_FONT_TRANSPARENT);
    left->setName(LABEL_SCORE_LEFT);
    scene->addChild(left);
    auto right = Label::createWithTTF("0", "Vanilla.ttf", 100);
    right->setPosition(Vec2( circle + origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    right->setTextColor(COLOR_FONT_TRANSPARENT);
    right->setName(LABEL_SCORE_RIGHT);
    scene->addChild(right);
    
    // admin
    auto lblAdmin = Label::createWithTTF("Admin:", "Vanilla.ttf", 30);
    lblAdmin->setAlignment(TextHAlignment::CENTER);
    lblAdmin->setTextColor(COLOR_FONT_TRANSPARENT);
    lblAdmin->setAnchorPoint(Vec2(0.5, 0));
    lblAdmin->setPosition(Vec2( origin.x + visibleSize.width/2 - circle, origin.y + 2*BORDER ));
    scene->addChild(lblAdmin,0);

    // admin name
    auto lblAdminName = Label::createWithTTF("", "Vanilla.ttf", 30);
    lblAdminName->setName(LABEL_ADMIN);
    lblAdminName->setAlignment(TextHAlignment::LEFT);
    lblAdminName->setTextColor(COLOR_FONT_TRANSPARENT);
    lblAdminName->setAnchorPoint(Vec2(0, 0));
    lblAdminName->setPosition(Vec2( origin.x + visibleSize.width/2 + BORDER, origin.y + 2*BORDER ));
    scene->addChild(lblAdminName,0);
    
    
    // ball sprite
    auto ball = Sprite::create("ball.png");
    ball->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    ball->setScale(SCALE_BALL);
    ball->setName(NODE_BALL);
    auto spriteBody = PhysicsBody::createCircle( ball->getContentSize().width/2, MATERIAL_BALL );
    spriteBody->setCategoryBitmask(BITMASK_BALL);
    spriteBody->setCollisionBitmask(BITMASK_SOLID | BITMASK_BALL | BITMASK_PLAYER | BITMASK_BALL_BOUNDARY);
    spriteBody->setContactTestBitmask(BITMASK_SOLID | BITMASK_BALL | BITMASK_PLAYER | BITMASK_SCORE | BITMASK_BALL_BOUNDARY);
    spriteBody->setLinearDamping(BALL_DAMPING);
    spriteBody->setAngularDamping(BALL_DAMPING);
    ball->setPhysicsBody(spriteBody);
    //scene->addChild(ball);
    
    //prevForce = Vec2(0,0);
    
    // collision listener
    //auto contactListener = EventListenerPhysicsContact::create();
    //contactListener->onContactBegin = CC_CALLBACK_1(StadiumScene::onContactBegin, this);
    //this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
}


void StadiumManager::addExitButton( VoidHandler * handler )
{
    VoidHandler * h = handler;
    float circle = Sprite::create("center.png")->getContentSize().width/4;
    
    // leave button
    auto disconnect = Label::createWithTTF("Exit", "Vanilla.ttf", 50);
    disconnect->setAlignment(TextHAlignment::CENTER);
    disconnect->setTextColor(COLOR_FONT_TRANSPARENT);
    auto item = MenuItemLabel::create(disconnect, [h](cocos2d::Ref* ref){ // [h] captures h variable (and stores?)
        h->execute();
    });
    item->setAnchorPoint(Vec2(0.5, 1));
    item->setPosition(Vec2( circle + origin.x + visibleSize.width/2, origin.y + visibleSize.height - 2*BORDER ));
    auto menu = Menu::create(item, NULL);
    menu->setPosition(Vec2::ZERO);
    scene->addChild(menu,0);
}



void StadiumManager::addNewPlayer(Player * player)
{
    auto sprite = player->getSprite();
    sprite->setPosition(Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));
    sprite->setScale(SCALE_BALL);
    sprite->setVisible(false);;                       // not playing yet
    
    auto body = sprite->getPhysicsBody();
    body->setCollisionBitmask(PLAYER_COLLIDES_WITH);
    body->setContactTestBitmask(BITMASK_ALL);
    body->setRotationEnable(false);
    body->setEnabled(false);                           // not playing yet
    
    scene->addChild(sprite);
}



void StadiumManager::showPlayer(Player * player)
{
    player->getSprite()->setVisible(true);
    player->getSprite()->getPhysicsBody()->setEnabled(true);
}



void StadiumManager::removePlayer(Player * player)
{
    scene->removeChild( player->getSprite() );
}



void StadiumManager::setAdminName(std::string name)
{
    auto label = static_cast<Label*>( scene->getChildByName(LABEL_ADMIN) );
    label->setString(name);
}



void StadiumManager::resetScore()
{
    auto left = static_cast<Label*>( scene->getChildByName(LABEL_SCORE_LEFT) );
    auto right = static_cast<Label*>( scene->getChildByName(LABEL_SCORE_RIGHT) );
    
    left->setString("0");
    right->setString("0");

}


void StadiumManager::runStadium()
{
    auto scene = StadiumScene::createScene();
    director->runWithScene(scene);
    
    // get the StadiumScene object (StadiumScene::createScene() creates a general Scene*)
    this->scene = dynamic_cast<StadiumScene*>(scene->getChildByTag(StadiumScene::SCENE_TAG));
    
    drawPitch();
}


void StadiumManager::addCollisionHandler(int bitmask, CollisionHandler * handler)
{
    scene->addCollisionHandler(bitmask, handler);
}
