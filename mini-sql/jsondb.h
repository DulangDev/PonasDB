//
//  jsondb.h
//  mini-sql
//
//  Created by Дмитрий Маслюков on 25.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef jsondb_h
#define jsondb_h
#include "json.h"
#include "codegen.hpp"
#include <mutex>
#include <future>
#include <sstream>
#include "parser.h"
#include <fstream>
#include <shared_mutex>
#include <sys/stat.h>
#include "executor.hpp"
class JsonDB{
    //the root array of records
    json collection;
    std::mutex bu_mux;
    
    std::shared_mutex wr;
    
    void backup()  {
        //executes at background thread
        std::async([&](){
            bu_mux.lock();
            std::ofstream writer("dbcache.json");
            writer << collection;
            writer.close();
            bu_mux.unlock();
        });
        
    };
    void insert(const json & value){
        collection.push(value);
        backup();
    }
    std::string search(Parser::astnode * query){
        if(query){
            std::stringstream ss;
            CodeGen::frame frame = CodeGen::generate(query);
            frame.print();
            wr.lock_shared();
            std::vector<int> res = execute_on_collection(collection, frame);
            wr.unlock_shared();
            ss << "[";
            if(res.size() >= 1){
                ss << collection[res[0]];
            }
            for(int i = 1; i < res.size(); ++i){
                ss << ", " << collection[res[i]];
            }
            ss << "]";
            return ss.str();
        } return "";
    }
public:
    
    std::string execute_query(const char * q){
        const char * body = strchr(q, ' ') + 1;
        size_t toklen = body - q - 1;
        if( strncmp(q, "select", toklen) == 0 ){
            if((long)body != 1){
                Parser::astnode * query = Parser::parse_query(body);
                std::string result = search(query);
                delete query;
                return result;
            }
            
        }
        if(strncmp(q, "insert", toklen) == 0){
            if((long)body != 1){
                json doc = Parser::parse_json(body);
                doc["_id"] = rand() % int(1e7);
                wr.lock();
                insert(doc);
                wr.unlock();
                return "ok";
            }
           
        }
        return "{\"status\":\"error\", \"reason\":\"unsupported operation\"}";
    }
    
    JsonDB():collection(json::Array()){
        struct stat buffer;
        int exists =stat("dbcache.json",&buffer);
        if(exists != -1){
            collection = Parser::load_json_from_file("dbcache.json");
        }
        
        //else no cache found, start with empty collection
    }
    
};

#endif /* jsondb_h */
