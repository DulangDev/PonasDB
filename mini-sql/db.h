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
        void add_str_entry(const char * val){
#warning TODO: typechecks
            data_t entry;
            entry.str = strdup(val);
            data.push_back(entry);
        }
        void add_num_entry(long num){
            data_t entry;
            entry.number = num;
            data.push_back(entry);
        }
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
        row name("name", row::types::str_entry);
        for(int i = 0; i < 1e5; ++i){
            name.add_str_entry("PONASENKOV");
        }
        row telephone("telephone", row::types::num_entry);
        for(int i = 0; i < 1e5; ++i){
            telephone.add_num_entry(i);
        }
        data.push_back(name);
        data.push_back(telephone);
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
