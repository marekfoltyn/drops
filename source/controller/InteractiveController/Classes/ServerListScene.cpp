#include "ServerListScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"
#include "Connector.h"
#include "Definitions.h"

USING_NS_CC;

using namespace cocostudio::timeline;

Scene * ServerListScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = ServerListScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool ServerListScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    //auto rootNode = CSLoader::createNode("MainScene.csb");
    //addChild(rootNode);
    
    auto c = Connector::getInstance();
    
    // search for servers every second
    auto foundCallback = CallFunc::create(CC_CALLBACK_0(ServerListScene::findServers, this));
    auto delay = DelayTime::create(FIND_SERVER_REPEAT_TIME);
    auto sequence = Sequence::create(delay, foundCallback, nullptr);
    auto infiniteSearch = RepeatForever::create(sequence);
    this->runAction(infiniteSearch);
    
    initGraphics();
    
    CCLOG("Searching for servers");
    c->addPacketCallback(P_SERVER_NAME, RAKNET_CALLBACK_1(ServerListScene::serverFound, this));
    
    return true;
}

void ServerListScene::initGraphics(){
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    // background
    auto background = Sprite::create("Background.png");
    //background->set( cocos2d::Size( visibleSize.width, visibleSize.height ) );
    background->setPosition(Vec2( origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 ));
    background->setScaleX((visibleSize.width / background->getContentSize().width));
    background->setScaleY((visibleSize.height / background->getContentSize().height));
    this->addChild(background);
    
    // monitor
    auto monitor = Sprite::create("monitor.png");
    monitor->setAnchorPoint(Vec2(0.5, 0.5));
    monitor->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
    this->addChild(monitor);
    
    
    // exit button
    auto btnExit = MenuItemImage::create("exit_button.png", "exit_button_pressed.png", CC_CALLBACK_1(ServerListScene::exitGame, this));
    btnExit->setScale(2);
    btnExit->setPosition( origin.x + visibleSize.height + 7*(visibleSize.width - visibleSize.height)/8 - btnExit->getContentSize().width, origin.y + visibleSize.height - btnExit->getContentSize().height );
    //this->addChild(btnExit); // no, because it will be part of menu
    
    // menu
    auto menu = Menu::create(btnExit, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
    
    // Searching for servers... label
    lblSearching = Label::createWithTTF("", "8-Bit-Madness.ttf", visibleSize.height/12);
    lblSearching->setAnchorPoint(Vec2(0,1));
    lblSearching->setPosition(Vec2(origin.x + visibleSize.width/2 - 160, origin.y + visibleSize.height - 20));
    this->addChild(lblSearching);
    // dots changing
    auto delay = DelayTime::create(0.5);
    auto sequence = Sequence::create(
        CallFunc::create(CC_CALLBACK_0(ServerListScene::searchLabelNoDot,this)),
        delay,
        CallFunc::create(CC_CALLBACK_0(ServerListScene::searchLabelOneDot,this)),
        delay->clone(),
        CallFunc::create(CC_CALLBACK_0(ServerListScene::searchLabelTwoDots,this)),
        delay->clone(),
        CallFunc::create(CC_CALLBACK_0(ServerListScene::searchLabelThreeDots,this)),
        delay->clone(),
        nullptr );
    auto infinite = RepeatForever::create(sequence);
    this->runAction(infinite);
    
    // Server names menu
    serversView = ui::ScrollView::create();
    serversView->setDirection( ui::ScrollView::Direction::VERTICAL );
    serversView->setContentSize( cocos2d::Size(monitor->getContentSize().width, monitor->getContentSize().width - 100) );
    serversView->setInnerContainerSize( cocos2d::Size(monitor->getContentSize().width, 800) );

}

void ServerListScene::searchLabelNoDot(){
    lblSearching->setString("Searching servers");
}
void ServerListScene::searchLabelOneDot(){
    lblSearching->setString("Searching servers.");
}
void ServerListScene::searchLabelTwoDots(){
    lblSearching->setString("Searching servers..");
}
void ServerListScene::searchLabelThreeDots(){
    lblSearching->setString("Searching servers...");
}

void ServerListScene::findServers(){
    CCLOG("Searching for servers...");
    Connector::getInstance()->FindServers();
}

void ServerListScene::serverFound(RakNet::Packet * p){
    
    int overhead = sizeof(unsigned char) + sizeof(RakNet::TimeMS);
    const char * serverNameInPacket = (char *) p->data + overhead; // skip message type and response time
    auto name = __String::create(serverNameInPacket);
    
    /*if(lblServerName != nullptr){
        lblServerName->setString(name->getCString());
    }*/
    
    //TODO: addServerToMenu(name, p->systemAddress);
    
    CCLOG("%s (%s) in %dms", name->getCString(), p->systemAddress.ToString(), (RakNet::TimeMS) *p->data+1);
    CCLOG("Connecting...");
    Connector::getInstance()->connect(p->systemAddress);
}

void ServerListScene::btnServerClicked(Ref * pSender){
    
    //CCLOG("Trying to connect...");
    //Connector::getInstance()->connect(address);
    
}

void ServerListScene::addOrUpdateServer(cocos2d::__String serverName, RakNet::SystemAddress * address){
    
    
    
}

void ServerListScene::exitGame(Ref * pSender){
    Connector::getInstance()->stopClient();
    Director::getInstance()->end();
    exit(0);
}