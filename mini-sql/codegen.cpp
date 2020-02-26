//
//  codegen.cpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 25.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "codegen.hpp"

void CodeGen::write_node (Parser::astnode * node, frame * fr){
    switch (node->type) {
        case Parser::NUMVAL:{
            long val = (*(long*)node->val << 1) | 1;
            fr->statics.push_back((void*)val);
            write_op(fr, op::load_static, (int)fr->statics.size() - 1);
        }break;
        case Parser::STRVAL:{
            fr->statics.push_back((void*)node->val);
            write_op(fr, op::load_static, (int)fr->statics.size() - 1);
        }break;
        case Parser::SYMBOL:{
            fr->statics.push_back((void*)node->val);
            write_op(fr, op::load_prop, (int)fr->statics.size() - 1);
        }break;
        case Parser::EQUALS:{
            write_node(node->children[1], fr);
            write_node(node->children[0], fr);
            if( node->children[1]->type == Parser::STRVAL ){
                write_op(fr, op::eq_comp_str);
            } else {
                write_op(fr, op::eq_comp_int);
            }
        }break;
        case Parser::LESS:{
            write_node(node->children[1], fr);
            write_node(node->children[0], fr);
            if( node->children[1]->type == Parser::STRVAL ){
                write_op(fr, op::less_comp_str);
            } else {
                write_op(fr, op::less_comp_int);
            }
        }break;
        case Parser::GREATER:{
            write_node(node->children[1], fr);
            write_node(node->children[0], fr);
            if( node->children[1]->type == Parser::STRVAL ){
                write_op(fr, op::gt_comp_str);
            } else {
                write_op(fr, op::gt_comp_int);
            }
        }break;
        case Parser::AND:{
            write_node(node->children[1], fr);
            write_node(node->children[0], fr);
            write_op(fr, op::and_op);
        }break;
        case Parser::OR:{
            write_node(node->children[1], fr);
            write_node(node->children[0], fr);
            write_op(fr, op::or_op);
        }break;
        case Parser::NOT:{
            write_node(node->children[0], fr);
            write_op(fr, op::not_op);
        }break;
        case Parser::SUBSCR:{
            bool is_global = strcmp( (const char*)node->children[0]->val, "this" ) == 0;
            if(!is_global){
                write_node(node->children[0], fr);
                write_node(node->children[1], fr);
            } else {
                size_t curpos = fr->operands.size();
                write_node(node->children[1], fr);
                fr->operands[curpos].type = op::load_global;
            }
            
        }break;
        default:
            break;
    }
}
