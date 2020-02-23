//
//  json.h
//  mini-sql
//
//  Created by Дмитрий Маслюков on 23.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef json_h
#define json_h
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

static const char * type_repres [] = {
    "null",
    "string",
    "number",
    "array",
    "object"
};

class json{
public:
    struct prop_entry;
    enum _t{
        nothing,
        string,
        number,
        array,
        dict
    } type;
private:
   
    
    
    typedef std::vector<json> arr;
    typedef std::vector<prop_entry> d_internals;
    
    union {
        void * inher_val;
        std::vector<json> elements;
        std::vector<prop_entry> properties;
    };
    
    json( const std::vector<prop_entry> & from_props):properties(from_props){
        type = dict;
    }
    
    json( const std::vector<json> & from_elems ):elements(from_elems){
        type = array;
    }
    
public:
    json(_t _type = nothing){
        type = _type;
    }
    const static json undefined;
    enum exceptions{
        index_out_of_bounds,
        incorrect_type
    };
    
    struct err{
        exceptions type;
        char msg [2000];
    };
    
    json( const json & rhs ){
        
        type = rhs.type;
        switch(type){
            case number:
                memcpy(&inher_val, &rhs.inher_val, 8);
                break;
            case string:
                inher_val = strdup((char*)rhs.inher_val);
                break;
            case array:{
                arr a;
                memcpy(&elements, &a, sizeof(a));
                elements = rhs.elements;
            }break;
            case dict:{
                d_internals d;
                memcpy(&elements, &d, sizeof(d));
                properties = rhs.properties;
            }break;
            default:break;
        }
    }
    
    ~json(){
        switch (type) {
            case string:
                free(inher_val);
                break;
            case array:
                elements.~arr();
                break;
            case dict:
                properties.~d_internals();
                break;
            default:
                break;
        }
    }
    
    json& operator=(const json& rhs){
        type = rhs.type;
        switch(type){
            case number:
                memcpy(&inher_val, &rhs.inher_val, 8);
                break;
            case string:
                inher_val = strdup((char*)rhs.inher_val);
                break;
            case array:{
                arr a;
                memcpy(&elements, &a, sizeof(a));
                elements = rhs.elements;
            }break;
            case dict:{
                d_internals d;
                memcpy(&elements, &d, sizeof(d));
                properties = rhs.properties;
                
            }break;
            default:break;
        }
        return *this;
    }
    
    json(double numval){
        type = number;
        memcpy(&inher_val, &numval, 8);
    }
    json(const char * strval){
        type = string;
        inher_val = strdup(strval);
    }
    
    static json Array(){
        return json(arr());
    }
    static json Dict(){
        return json(d_internals());
    }
    
    const json& operator[](int idx) const {
        if(type != array){
            return undefined;
        }
        if(idx < 0 or idx >= elements.size()){
            return undefined;
        }
        return elements[idx];
    }
    
    json& operator[](int idx){
        if(type != array){
            err new_err;
            new_err.type = incorrect_type;
            throw new_err;
        }
        if( idx < 0 ){
            err new_err;
            new_err.type = index_out_of_bounds;
            throw new_err;
        }
        if( idx >= elements.size() ){
            elements.resize(idx + 1);
        }
        return elements[idx];
    }
    
    operator double() const{
        if(type != number){
            err new_err;
            new_err.type = incorrect_type;
            throw new_err;
        }
        return *(double*)&inher_val;
    }
    
    const char * c_str() const {
        if(type != string){
            err new_err;
            new_err.type = incorrect_type;
            throw new_err;
        }
        return (const char *)inher_val;
    }
    
    const json& operator[](const char * s) const;
    json& operator[](const char *s);
    
    void push(const json & rhs){
        if(type != array)
            return;
        (*this)[(int)elements.size()] = rhs;
    }
    void push(const prop_entry & entry){
        if(type != dict){
            return;
        }
        properties.push_back(entry);
    }
    
    size_t length() const {
        if(type == array){
            return elements.size();
        }
        if(type == dict){
            return properties.size();
        }
        return 0;
    }
    
    friend std::ostream& operator << (std::ostream & ostream, const json & object);
    std::string stringify() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }
    _t getType() const {
        return type;
    }
    const d_internals & get_entries() const{
        if(type != dict)
            throw incorrect_type;
        return properties;
    }
};

struct json::prop_entry{
    char * name;
    json val;
    prop_entry(const prop_entry & rhs){
        name = strdup(rhs.name);
        val = rhs.val;
    }
    
    prop_entry():val(){
        name = 0;
    }
    ~prop_entry(){
        if(name)
            free(name);
    }
} ;

#endif /* json_h */
