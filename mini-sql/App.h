//
//  App.h
//  mini-sql
//
//  Created by Дмитрий Маслюков on 26.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef App_h
#define App_h
#include "jsondb.h"
#include "server.h"
#include <map>
class App{
    JsonDB db;
    Server serv;
    
public:
    JsonDB& getDB() {
        return db;
    }

    void route( const char * path,Server::handler h ){
        serv.route(path, h);
    }
    App():db(), serv(9000){
       
    }
    void launch(){
        serv.serve();
    }
    
};

#endif /* App_h */
