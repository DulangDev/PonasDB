//
//  json-imp.cpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 23.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "json.h"
const json json::undefined = json();


const json& json::operator[](const char * s) const{
    if(type != json::dict){
        return undefined;
    }
    for(auto & entry: properties){
        if(strcmp(entry.name, s) == 0){
            return entry.val;
        }
    }
    return json::undefined;
}

json& json::operator[](const char * s){
    if(type != dict){
        err new_err;
        new_err.type = incorrect_type;
        throw new_err;
    }
    for(auto & entry: properties){
        if(strcmp(entry.name, s) == 0){
            return entry.val;
        }
    }
    prop_entry entry;
    entry.name = strdup(s);
    properties.push_back(entry);
    return (properties.back()).val;
}




std::ostream& operator << (std::ostream & ostream, const json & object){
    switch (object.type) {
        case json::number:
            ostream << *(double*)&object.inher_val;
            break;
        case json::string:
            ostream << '"'<<(const char*)object.inher_val << '"';
            break;
        case json::array:
            ostream << "[";
            if( object.elements.size() > 0 ){
                ostream << object.elements[0];
            }
            for(int i = 1; i < object.elements.size(); i++){
                ostream << ", " << object.elements[i];
            }
            ostream << "]";
            break;
        case json::dict:
            ostream << "{";
            if( object.properties.size() > 0 ){
                const json::prop_entry & first = object.properties[0];
                ostream << "\"" <<first.name << "\": " << first.val;
            
            }
            for(int i = 1; i < object.properties.size(); i++){
                const json::prop_entry & item = object.properties[i];
                ostream << ", \"" <<item.name << "\": " << item.val;
            }
            ostream << "}";
            break;
        default:
            ostream << "null";
            break;
    }
    return ostream;
}
