//
//  db.h
//  mini-sql
//
//  Created by Дмитрий Маслюков on 17.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef db_h
#define db_h
#include <vector>
#include <cstring>
#include <string>

#include "json.h"
class engine;
class DB{
    friend class engine;
    struct row{
        const char * name;
        enum types {
            str_entry,
            num_entry
        } type;
        union data_t{
            char * str;
            long number;
        };
        std::vector<data_t> data;
        
        row(const char * name, types t){
            this->name = strdup(name);
            type = t;
        }
        
        void add_entry( const char * v ){
            if( type == str_entry )
                add_str_entry(v);
        }
        
        void add_entry(long num){
            if(type == num_entry){
                add_num_entry(num);
            }
        }
    private:
        void add_str_entry(const char * val){
            data_t entry;
            entry.str = strdup(val);
            data.push_back(entry);
        }
        void add_num_entry(long num){
            data_t entry;
            entry.number = num;
            data.push_back(entry);
        }
    public:
        std::string serialize(int pos) const {
            std::string result = "\""+std::string(name)+"\":";
            if(type == str_entry){
                result += "\"";
                result += data[pos].str;
                result += "\"";
            } else {
                result += std::to_string(data[pos].number);
            }
            return result + ", ";
        };
    };
    std::vector<row> data;
public:
    DB(){
        //push some dummy data
       
    }
    
    DB(const json & j){
        for(int i = 0; i < j.length(); ++i){
            const json & record= j[i];
            //skip all non-dict entries, here we prevent exception of incorrect type and dont need to catch it
            if( record.getType() != json::dict ){
                continue;
            }
            const auto & entries = record.get_entries();
            for( auto & entry: entries ){
                for( auto & r: data ){
                    if( strcmp(r.name, entry.name) == 0 ){
                        if(entry.val.getType() == json::number){
                            r.add_entry((double)entry.val);
                        }
                        if(entry.val.getType() == json::string){
                            r.add_entry(entry.val.c_str());
                        }
                        goto ct;
                    }
                }
                {
                    row::types type = row::str_entry;
                    if( entry.val.getType() == json::number ){
                        type = row::num_entry;
                    }
                    row r( entry.name, type );
                    if(entry.val.getType() == json::number){
                        r.add_entry((double)entry.val);
                    }
                    if(entry.val.getType() == json::string){
                        r.add_entry(entry.val.c_str());
                    }
                    data.push_back(r);
                }
                
            ct:;
            }
        
        }
    }
    
    std::vector<row>& getData() {
        return data;
    }
    
    std::string serialize(int pos) const {
        std::string result = "{ ";
        for( const row& r: data ){
            result += r.serialize(pos);
        }
        result.erase(result.length() - 2, 2);
        return result + " }\n";
    };
    
};

#endif /* db_h */
