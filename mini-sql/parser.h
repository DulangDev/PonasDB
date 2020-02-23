//
//  parser.h
//  mini-sql
//
//  Created by Дмитрий Маслюков on 17.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef parser_h
#define parser_h
#include <vector>
#include <iostream>
#include "json.h"
#warning names of rows cannot contain and, or, not


static const char * lexems [] = {
    "==",
    "<",
    ">",
    "and",
    "or",
    "not",
    "(",
    ")",
    "{",
    "}",
    ",",
    ":",
    "[",
    "]",
    "true",
    "false"
};
class Parser{
    
public:
    enum asttype{
        EQUALS,
        LESS,
        GREATER,
        STRVAL,
        NUMVAL,
        SYMBOL,
        AND,
        OR,
        NOT
    };
    struct astnode {
        asttype type;
        std::vector<astnode *> children;
        astnode * parent;
        void * val;
        //for optimisation
        int field_offt;
        
        void print( std::ostream& o, int offt ){
            for(int i = 0; i < offt; ++i){
                std::cout<< " ";
            }
            switch (type) {
                case EQUALS:
                    o<< "EQUALS: \n";
                    children[0]->print(o, offt + 4);
                    children[1]->print(o, offt + 4);
                    break;
                case LESS:
                    o<< "LESS: \n";
                    children[0]->print(o, offt + 4);
                    children[1]->print(o, offt + 4);
                    break;
                case GREATER:
                    o<< "GREATER: \n";
                    children[0]->print(o, offt + 4);
                    children[1]->print(o, offt + 4);
                    break;
                case AND:
                    o<< "AND: \n";
                    children[0]->print(o, offt + 4);
                    children[1]->print(o, offt + 4);
                    break;
                case OR:
                    o<< "OR: \n";
                    children[0]->print(o, offt + 4);
                    children[1]->print(o, offt + 4);
                    break;
                case NOT:
                    o<< "NOT: ";
                    children[0]->print(o, 0);
                    o << "\n";
                    break;
                case SYMBOL:
                case STRVAL:
                    o<< (char*)val << "\n";
                    break;
                case NUMVAL:
                    o << *(long*)val << "\n";
                    break;
                default:
                    break;
            }
        }
    };
    
    static void delete_node(astnode* node){
        if(node->val)
            free(node->val);
        for(auto child: node->children){
            delete_node(child);
        }
        delete node;
    }
    
    enum lextype{
        equals,
        less,
        greater,
        and_lex,
        or_lex,
        not_lex,
        open_bracket,
        close_bracket,
        obrace,
        cbrace,
        comma,
        colon,
        arr_op,
        arr_cl,
        _true,
        _false,
        strlit,
        numlit,
        name,
        error
    };
    
    struct lexem {
        lextype type;
        void * val;
        size_t size;
        
        std::string print() const{
            if( type < strlit ){
                return std::string("KW: ") + lexems[type];
            }
            if( type == strlit ){
                return std::string("STRING: ") + (char*)val;
            }
            if( type == numlit ){
                return std::string("NUMBER: ") + std::to_string(*(long*)val);
            }
            if( type == name ){
                return std::string("SYMBOL: ") + (char*)val;
            }
            return "none";
        }
    };
    
    static lexem get_lexem(const char * q){
        const char * reader = q;
        while( *reader == ' ' or *reader == '\n'){
            reader ++;
        }
        if( *reader == '"' ){
            const char * end_str = reader + 1;
            while( *end_str != '"' ){
                end_str ++;
            }
            lexem result;
            result.type = strlit;
            result.val = strndup(reader + 1, end_str - reader - 1);
            result.size = end_str - q + 1;
            return result;
        }
        for(int i = 0; i < strlit; ++i){
            if(strncmp(lexems[i], reader, strlen(lexems[i])) == 0){
                lexem result;
                result.type = (lextype)i;
                result.val = 0;
                result.size = strlen(lexems[i]) + (reader - q);
                return result;
            }
        }
        const char * iden_reader = reader;
        while(isalpha(*iden_reader)){
            iden_reader ++;
        }
        if( iden_reader > reader ){
            lexem result;
            result.type = name;
            result.val = strndup(reader, iden_reader - reader);
            result.size = iden_reader - q;
            return result;
        }
        char * num_reader;
        long num = strtol(reader, &num_reader, 10);
        lexem result;
        result.type = numlit;
        result.val = malloc(8);
        memcpy(result.val, &num, 8);
        result.size = num_reader - q;
        return result;
    }
    
    static std::vector<lexem> split_to_lexems (const char * query){
        //keyword is not being parsed here
        std::vector<lexem> ls;
        const char * rdr = query;
        int l = strlen(query);
        while( rdr < query + l ){
            lexem l = get_lexem(rdr);
            ls.push_back(l);
            rdr+= l.size;
        }
        return ls;
    }
    
    typedef std::vector<lexem>::iterator lwalker;
    
    
    
    static json parse_json(const char * json_string){
        std::vector<lexem> ls = split_to_lexems(json_string);
        auto iter = ls.begin();
        return parse_js_anything(iter);
    }
private:
    static json parse_js_literal        (lwalker & walker);
    static json parse_js_array          (lwalker & walker);
    static json parse_js_dict           (lwalker & walker);
    static json parse_js_anything       (lwalker & walker);
    static json::prop_entry parse_entry        (lwalker & walker);
    static astnode * parse_logor        (lwalker & walker);
    static astnode * parse_logand       (lwalker & walker);
    static astnode * parse_lognot       (lwalker & walker);
    static astnode * parse_expr         (lwalker & walker);
public:
    
    
    
    static astnode * parse_query(const char * query){
        std::vector<lexem> lstream = split_to_lexems(query);
        auto iter = lstream.begin();
        return parse_expr(iter);
    }
    
    
    
};

#endif /* parser_h */
