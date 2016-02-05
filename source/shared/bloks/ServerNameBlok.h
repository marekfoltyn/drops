//
//  ServerNameBlok.h
//  InteractiveController
//
//  Created by Marek Foltýn on 26.01.16.
//
//

#ifndef ServerNameBlok_h
#define ServerNameBlok_h

#include <string>
#include "Connector.h"


class ServerNameBlok{
public:
    
    /**
     * parse the string
     */
    static std::string parseServerName(Blok * blok);
    
    /**
     * create ServerNameBlok from string
     */
    static Blok * create(std::string str);
    
};


#endif /* ServerNameBlok_h */
