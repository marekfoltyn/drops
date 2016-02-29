//
//  ServerNameHandler.hpp
//  InteractiveController
//
//  Created by Marek Foltýn on 29.02.16.
//
//

#ifndef ServerNameHandler_hpp
#define ServerNameHandler_hpp

#include "AbstractHandlers.h"

class ServerNameHandler: public BoxHandler{
public:

    ServerNameHandler();
    
    virtual void execute( GameNet::Box * box );
    
private:
    
};

#endif /* ServerNameHandler_hpp */
