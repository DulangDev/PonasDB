//
//  codegen.hpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 25.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef codegen_hpp
#define codegen_hpp

#include "json.h"
#include "parser.h"
//walks ast and produces linear code

enum value_types {
    string,
    integer
};

static value_types get_type_by_last_bit(void * val){
    return (value_types)((long)(val) & 1);
}

static const char * optype_repr [] = {
    "load_prop",
    "int ==",
    "int <",
    "int >",
    "str ==",
    "str <",
    "str >",
    "or",
    "and",
    "in",
    "not",
    "load_stat",
    "return",
    "load_glob"
};
class CodeGen{
public:
    struct op{
        enum op_type{
            load_prop,
            eq_comp_int,
            less_comp_int,
            gt_comp_int,
            eq_comp_str,
            less_comp_str,
            gt_comp_str,
            or_op,
            and_op,
            in_op,
            not_op,
            load_static,
            ret,
            load_global
        } type;
        int arg;
        long i_cache [3];
    };
    struct frame{
        std::vector<op> operands;
        //static is a long number or a ptr to string
        // number << 1 + 1, string has last bit as 0
        std::vector<void*> statics;
        void * stack [10000];
        bool is_empty() const {
            return operands.size() == 0;
        }
        void print (std::ostream & ostream = std::cout) const {
            for(int i = 0; i < operands.size(); i++){
                const op & _op = operands[i];
                ostream << i << " " << optype_repr[ (int)_op.type ] << " " << _op.arg;
                if( _op.type == op::load_prop || _op.type == op::load_static || _op.type == op::load_global){
                    ostream << " ";
                    value_types prop_type = get_type_by_last_bit(statics[_op.arg]);
                    if( prop_type == integer ){
                        ostream << ((long)statics[_op.arg] >> 1);
                    } else {
                        ostream << (const char*)statics[_op.arg];
                    }
                }
                ostream << "\n";
            }
        }
    };
    static frame generate(Parser::astnode * root){
        frame fr;
        if( root != 0 ){
            write_node(root, &fr);
            write_op(&fr, op::ret);
        }
        return fr;
    }
    static void write_node (Parser::astnode *, frame *);
    static void write_op   (frame * fr, op::op_type t, int arg = 0){
        op new_op = {
            t,
            arg,
            { 0, 0, 0}
        };
        fr->operands.push_back(new_op);
    }
};

#endif /* codegen_hpp */
