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
class App{
    JsonDB db;
    Server serv;
public:
    App():db(), serv(9000){
       
    }
    void launch(){
        do{
            typename Server::Connection c = serv.connect();
            const char * q = c.get_query();
            std::string res = db.execute_query(q);
            c.write_answer(res.c_str());
        } while(1);
    }
    
};

#endif /* App_h */
