#ifndef __MainMenu_SCENE_H__
#define __MainMenu_SCENE_H__

#include <map>
#include <atomic>

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"
#include "Connector.h"

#include "HandlerMap.h"

USING_NS_CC;
USING_NS_CC_EXT;
using namespace GameNet;

/**
 * Simple structure for storing information about servers in LAN
 */
struct ServerMapEntry{
    
    /**
     * server address (IP and port)
     */
    RakNet::SystemAddress * address;

    /**
     * how many second the server did not responed
     * after SERVER_MENU_LIFETIME seconds the server entry
     * will be deleted
     */
    std::atomic<int> inactiveSeconds;

    /**
     * position in scrollview
     * necessary for item position (when deleted)
     */
    int position;
};

/**
 * First scene - showing the main menu with the list of servers in LAN
 */
class MainMenuScene : public cocos2d::Layer
{
public:
    
    static const int SCENE_TAG = 1;
    
    /**
     * Cocos2d-x:
     * there's no 'id' in cpp, so we recommend returning the class instance pointer
     */
    static cocos2d::Scene * createScene();

    /**
     * Cocos2d-x:
     * Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
     */
    virtual bool init();

    /**
     * Cocos2d-x:
     * implement the "static create()" method manually
     */
    CREATE_FUNC(MainMenuScene);
    
    /**
     * load and set up cocos nodes and graphics in init()
     */
    void initGraphics();
    
    /**
     * decrease lifetime of found servers and find more servers
     * this method is called periodically
     */
    void findServersStep();
    
    
    /**
     * This method is called when user interacts with the player name textfield
     */
    void txtNameEvent(Ref * sender, ui::TextField::EventType type);
    
    /**
     * try to connect to server
     */
    void btnServerClicked(Ref * pSender);
    
    /**
     * a server has responded to the ping
     * if the controller knows about it, resets its lifetime
     * if not, add this server to the server list
     */
    void refreshServer(Box * box);
    
    /**
     * Connection to server failed
     */
    void connectionFailed(Box * box);
    
    /**
     * Exit game
     */
    void btnLeaveClicked(Ref * pSender);
    
    
private:
    
    /**
     * map of available servers
     * key - hash of the system address
     */
    std::map<int, ServerMapEntry*> serverMap;
    
    HandlerMap * handlerMap;
    
    /**
     * count of active servers (serverMap.size() doesn't work, because inactive servers items are not deleted due to memory issues)
     */
    int serverCount;
    
    /**
     * start receiving boxs
     */
    void startReceiveBoxes();
        
    /**
     * periodically send broadcast ping to find servers
     */
    void startFindServers();
    
    /**
     * send a ping broadcast message to find servers
     */
    void pingServers();
        
    /**
     * add new menu item to scroll view or updates if exists
     */
    void addOrUpdateServer(std::string serverName, RakNet::SystemAddress address);
    
    /**
     * every "server search" decreases servers lifetimes (when not responding -> delete from menu)
     */
    void decreaseServersLifetime();
};

#endif // __MainMenu_SCENE_H__
