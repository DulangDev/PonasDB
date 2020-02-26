//
//  parser.cpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 17.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "parser.h"
#define jsondb 1
#include <cstdarg>

Parser::astnode * create_node_va(Parser::asttype type, int children, va_list va){
    Parser::astnode * node = new Parser::astnode();
    node->type = type;
    for(int i = 0; i < children; ++i){
        Parser::astnode * _arg = va_arg(va, Parser::astnode*);
        node->children.push_back(_arg);
        _arg->parent = node;
    }
    return node;
}

Parser::astnode * create_node(Parser::asttype type, int children, ...){
    va_list va;
    va_start(va, children);
    Parser::astnode * n = create_node_va(type, children, va);
    va_end(va);
    return n;
}



Parser::astnode * Parser::parse_expr(lwalker &walker){
    astnode* left = parse_logor(walker);
    if( (*walker).type == or_lex ){
        walker ++;
        astnode * right = parse_lognot(walker);
        if(! right)
            return 0;
        return create_node(Parser::asttype::OR, 2, left, right);
    }
    return left;
}

Parser::astnode * Parser::parse_logor(lwalker &walker){
    astnode* left = parse_logand(walker);
    if( (*walker).type == and_lex ){
        walker ++;
        astnode * right = parse_logand(walker);
        return create_node(Parser::asttype::AND, 2, left, right);
    }
    return left;
}

Parser::astnode * Parser::parse_logand(lwalker &walker){
    if((*walker).type == not_lex){
        walker++;
        astnode * node = parse_lognot(walker);
        return create_node(NOT, 1, node);
    }
    return parse_lognot(walker);
}

Parser::astnode * Parser::parse_lognot(lwalker &walker){
    if((*walker).type == open_bracket){
        walker++;
        astnode * subexp = parse_expr(walker);
        if( (*walker).type != close_bracket ){
            return 0;
        }
        return subexp;
    }
    lexem left = *walker;
    if(left.type != name){
        return 0;
    }
    astnode * _left = parse_symbol(walker);
#if jsondb
    if(1){
        //load global
        astnode * gl = create_node(SYMBOL, 0);
        gl->val = strdup("this");
        _left = create_node(SUBSCR, 2, gl, _left);
    }
#endif
    lexem comp = * walker;
    walker++;
    lexem right = * walker;
    walker ++;
    asttype rtype = STRVAL;
    if( right.type == strlit )
        rtype = STRVAL;
    if( right.type == numlit )
        rtype = NUMVAL;
    astnode * _right = create_node(rtype, 0);
    _right->val = right.val;
    asttype comptype = (asttype)(EQUALS + ((int)comp.type - (int)equals));
    if( comptype != EQUALS && comptype != GREATER && comptype != LESS ){
        return 0;
    }
    return create_node(comptype, 2, _left, _right);
}

Parser::astnode * Parser::parse_symbol       (lwalker & walker){
    lexem & l = *walker;
    if( l.type != name ){
        return 0;
    }
    astnode * _left = create_node(SYMBOL, 0);
    _left->val = l.val;
    walker++;
    l = *walker;
    if( l.type == dot ){
        walker ++;
        astnode * right = parse_symbol(walker);
        if( !right )
            return 0;
        return create_node(SUBSCR, 2, _left, right);
    }
    return _left;
}

json Parser::parse_js_literal        (lwalker & walker){
    lexem & l = *walker;
    walker++;
    if(l.type == numlit){
        return json( *(long*)l.val );
    }
    if(l.type == strlit){
        return json((const char*)l.val);
    }
    if(l.type == _true){
        return json(1.0);
    }
    if(l.type == _false){
        return json(0.0);
    }
    return json::undefined;
}
json Parser::parse_js_array          (lwalker & walker){
    //assume that we stand at [
    walker++;
    json arr = json::Array();
    json j = parse_js_anything(walker);
    arr.push(j);
    while( (*walker).type != arr_cl ){
        if((*walker).type != comma){
            return json::undefined;
        }
        walker++;
        json elem = parse_js_anything(walker);
        arr.push(elem);
    }
    walker++;
    return arr;
}
json::prop_entry Parser::parse_entry(lwalker & walker){
    Parser::lexem & l = *walker;
    if(l.type != Parser::strlit){
        return json::prop_entry();
    }
    char * name = strdup((char*)l.val);
    walker ++;
    if( (*walker).type != Parser::colon ){
        return json::prop_entry();
    }
    walker++;
    json val = parse_js_anything(walker);
    json::prop_entry entry;
    entry.name = name;
    entry.val = val;
    return entry;
    
}
json Parser::parse_js_dict           (lwalker & walker){
    //assume that we stand at {
    walker++;
    json dict = json::Dict();
    json::prop_entry entry = parse_entry(walker);
    dict.push(entry);
    while( (*walker).type != cbrace ){
        if((*walker).type != comma){
            return json::undefined;
        }
        walker++;;
        dict.push(parse_entry(walker));
        
    }
    walker++;
    return dict;
}
json Parser::parse_js_anything       (lwalker & walker){
    lexem & l = *walker;
    switch (l.type) {
        case numlit:
        case strlit:
        case _true:
        case _false:
            return parse_js_literal(walker);
        case arr_op:
            return parse_js_array(walker);
        case obrace:
            return parse_js_dict(walker);
        default:
            return json::undefined;
            break;
    }
}
