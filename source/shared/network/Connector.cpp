#include "Connector.h"
#include "Definitions.h"
#include "Block.h"

#include "MessageIdentifiers.h"
#include "RakPeerInterface.h"

#include <assert.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <stdlib.h>

Connector * Connector::instance = nullptr;


Connector * Connector::getInstance(){
    if(instance == nullptr) {
        instance = new Connector();
    }
    return instance;
}


bool Connector::start(){
    return startAsServer(1);
}


bool Connector::startAsServer(unsigned short maxPlayers){

    // raknet interface configuration
    interface = RakNet::RakPeerInterface::GetInstance();
    interface->SetTimeoutTime(CONNECTION_LOST_TIMEOUT, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
    interface->SetIncomingPassword(0,0); // nullptr, pass length
    interface->SetOccasionalPing(true);
    interface->SetUnreliableTimeout(CONNECTION_LOST_TIMEOUT);
    
    // socket descriptor settings
    RakNet::SocketDescriptor socketDescriptors[1];
    socketDescriptors[0].port = (maxPlayers>1) ? SERVER_PORT : CLIENT_PORT;
    socketDescriptors[0].socketFamily=AF_INET; // IPV4
    bool result = interface->Startup(maxPlayers, socketDescriptors, 1 ) == RakNet::RAKNET_STARTED; // last arg is socketDescriptor count
    interface->SetMaximumIncomingConnections(maxPlayers);
    if (result == false){
        LOG("Server failed to start.\n");
        RakNet::RakPeerInterface::DestroyInstance(interface);
        interface = nullptr;
        return false;
    }
    
    // logging used socket addresses
    
    DataStructures::List< RakNet::RakNetSocket2* > sockets;
    interface->GetSockets(sockets);
    
    /*LOG("Socket addresses used by Connector:\n==========================\n");
    for (unsigned int i=0; i < sockets.Size(); i++)
    {
        LOG("%i. %s\n", i+1, sockets[i]->GetBoundAddress().ToString(true));
    }*/
    
    LOG("\nMy IP addresses:\n");
    for (unsigned int i=0; i < interface->GetNumberOfAddresses(); i++)
    {
        RakNet::SystemAddress sa = interface->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, i);
        LOG("%i. %s:%i\n", i+1, sa.ToString(false), sa.GetPort());
    }
    
    // server name
    if(maxPlayers>1){
        interface->SetOfflinePingResponse("Unnamed Server", strlen("Unnamed Server")+1); // '\0' is automatically added in "hello"
    }
    
    return true;
}


void Connector::stop(){
    
    if (interface == nullptr) {
        return;
    }
    
    interface->Shutdown(0); // 0 ... dont wait for stopping connections
    RakNet::RakPeerInterface::DestroyInstance(interface);
    interface = nullptr;
}


void Connector::connect( RakNet::SystemAddress server ){
   
    if(interface==nullptr){
        LOG("Client is not started!");
        return;
    }
    
    char ip[4];
    server.ToString(false, ip);
    interface->Connect(ip, server.GetPort(), nullptr, 0); // no password -> nullptr, 0
}


void Connector::disconnect( RakNet::SystemAddress address ){
    if (nullptr) {
        return;
    }
    interface->CloseConnection(address, true); // true ... send disconnection notification
}


void Connector::send(Block * b){
    if(interface == nullptr){
        return;
    }
    
    interface->Send( b->getPacketData(), b->getPacketLength(), b->getPriority(), b->getReliability(), 0, b->getAddress(), false);
}


RakNet::Packet * Connector::receive(){
    
    if(interface == nullptr) {
        return 0;
    }
    
    return interface->Receive();
}


void Connector::setServerName(std::string name){
    if(interface == nullptr){
        return;
    }
    
    interface->SetOfflinePingResponse(name.c_str(), (unsigned int) name.length()+1 );
}


std::string Connector::getServerName(){
    if(interface == nullptr){
        LOG("Server not started - no name!");
        return "";
    }
    
    char * name;
    unsigned int len;
    interface->GetOfflinePingResponse(&name, &len);
    return name;
}

void Connector::ping(){

    if(interface==nullptr){
        return;
    }
    
    interface->Ping("255.255.255.255", SERVER_PORT, true); // true ... reply only if server is not full
}