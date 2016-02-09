#ifndef  _DEFINITIONS_H_
#define  _DEFINITIONS_H_

#include "MessageIdentifiers.h"
#include "cocos2d.h"

// time (ms) during when reliable packet was not able to be received -> connection lost
#define CONNECTION_LOST_TIMEOUT 5000

// logging function
#define LOG(...) CCLOG(__VA_ARGS__)

// port used for all communication
#define SERVER_PORT 54027

// client port used for all communication
#define CLIENT_PORT 54023

// maximum players connected to server
#define MAX_PLAYERS 8

// sleep time between receiving packets (ms)
#define RAKNET_SLEEP 30

// find servers repeat time (in seconds)
#define FIND_SERVER_REPEAT_TIME 0.5

// server lifetime - time to disappear from menu after not responsing to ping (in seconds)
#define SERVER_MENU_LIFETIME 5

// how often to run packet receive loop (in seconds)
#define RECEIVE_TIMEOUT 0.03

// scene transition speed (in seconds)
#define SCENE_TRANSITION 0.2

/////////////////// Message types  ///////////////////

#define P_PING                        ID_UNCONNECTED_PING_OPEN_CONNECTIONS
#define P_SERVER_NAME                 ID_UNCONNECTED_PONG
#define P_NEW_CONNECTION              ID_NEW_INCOMING_CONNECTION
#define P_CONNECTED                   ID_CONNECTION_REQUEST_ACCEPTED
#define P_DISCONNECTED                ID_DISCONNECTION_NOTIFICATION
#define P_CONNECTION_FAILED           ID_CONNECTION_ATTEMPT_FAILED
#define P_CONNECTION_LOST             ID_CONNECTION_LOST

/**
 * #define PACKET_CUSTOM              ID_USER_PACKET_ENUM+1 // +2, +3 and so on
 */

#define P_ACCELERATION                ID_USER_PACKET_ENUM+1
#define P_COLLISION                   ID_USER_PACKET_ENUM+2
#define P_PLAYER_NAME                 ID_USER_PACKET_ENUM+3
#define P_KICK                        ID_USER_PACKET_ENUM+4
#define P_TACKLE                      ID_USER_PACKET_ENUM+5
#define P_ADMIN                       ID_USER_PACKET_ENUM+6
#define P_RESET_SCORE                 ID_USER_PACKET_ENUM+7

#endif // _DEFINITIONS_H_
