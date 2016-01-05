#ifndef  _CONNECTOR_H_
#define  _CONNECTOR_H_

#include "raknet/MessageIdentifiers.h"
#include "raknet/RakPeerInterface.h"
#include "raknet/RakSleep.h"
#include <thread>
#include <map>

// callback function type for packet processing
// char * ... binary packet data
// int    ... size of the data (in bytes)
typedef void(*callbackFuncType)(unsigned char*, int);

/**
 @brief     Singleton class responsible for entire network communication.
 
 Represents both server and client
 
 usage: Connector::getInstance()-> ...
 
 */
class Connector {
public:
    
    static Connector * getInstance();
    
    bool startServer(int port);
    bool startClient(int port);
    
    void stopServer();
    void stopClient();
    
    // add callback to packet type
    void addPacketCallback(int packetType, const std::function<void(RakNet::Packet*)>& callback );
    void removePacketCallback(int packetType);
    
    //// Client ////
    void PingServers(int timeoutMS);
    
private:
    
    // Hidden singleton constructor
    Connector();
    
    // Packet processing runnable for packetProcessor thread
    void infiniteReceiveLoop();
    
    // PacketProcessor control
    void startPacketProcessor();
    void stopPacketProcessor();
    
    // RakNet interfaces
    RakNet::RakPeerInterface * server = nullptr;
    RakNet::RakPeerInterface * client = nullptr;
    
    // Singleton instance pointer
    static Connector * instance;
    
    std::thread packetProcessorThread;
    
    // Map of callback functions for packet types
    std::map<int, std::function<void(RakNet::Packet*)>> callbackMap;
    
    // If true, receive loop will continue; if false, loop finishes before next loop
    bool loopIsActive;
    
};

// New callbacks based on C++11
// Number at the end tells how many arguments the callback function has
#define RAKNET_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define RAKNET_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define RAKNET_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define RAKNET_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)


#endif // _CONNECTOR_H_