//
//  engine.h
//  mini-sql
//
//  Created by Дмитрий Маслюков on 17.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef engine_h
#define engine_h
#include "db.h"
#include "parser.h"

class engine {
    DB database;
    // not to perform strcmp each time as it will have complexity O(m*n), where n is amount of records in db and m is complexity of the query
    void optimize_leaf(Parser::astnode * leafnode){
        //we assume that leafnode holds the name;
        char * fieldname = (char*)leafnode->val;
        leafnode->field_offt = -1;
        for(int i = 0; i < database.getData().size(); ++i){
            if( strcmp(database.getData()[i].name, fieldname) == 0 ){
                leafnode->field_offt = i;
            }
        }
    }
    //recursive call
    void optimize_node_with_offsets(Parser::astnode * expr){
        if( expr->type == Parser::GREATER || expr->type == Parser::EQUALS || expr->type == Parser::LESS){
            optimize_leaf(expr->children[0]);
        } else {
            for( auto& child: expr->children ){
                optimize_node_with_offsets(child);
            }
        }
    }
    
    bool compute_query_for_index(Parser::astnode * expr, int index){
        if( expr->type == Parser::GREATER || expr->type == Parser::LESS || expr->type == Parser::EQUALS ){
            int fieldNo = expr->children[0]->field_offt;
            if( fieldNo == -1 ){
                // no such field
                return false;
            }
            //type mismatch
            if( expr->children[1]->type == Parser::STRVAL && database.getData()[fieldNo].type == DB::row::num_entry ){
                return false;
            }
            if( expr->children[1]->type == Parser::NUMVAL && database.getData()[fieldNo].type == DB::row::str_entry ){
                return false;
            }
            int reskind = 0;
            if( expr->children[1]->type == Parser::STRVAL ){
                reskind = strcmp((char*)expr->children[1]->val, database.getData()[fieldNo].data[index].str);
            } else {
                long numval = database.getData()[fieldNo].data[index].number;
                long comp_with = *(long*)expr->children[1]->val;
                if( numval > comp_with )
                    reskind = -1;
                if( numval == comp_with )
                    reskind = 0;
                if( numval < comp_with ){
                    reskind = 1;
                }
            }
            
            if(expr->type == Parser::GREATER){
                return reskind < 0;
            }
            if(expr->type == Parser::EQUALS){
                return reskind == 0;
            }
            return reskind > 0;
        }
        if( expr->type == Parser::AND || expr->type == Parser::OR ){
            bool left_res = compute_query_for_index(expr->children[0], index);
            bool right_res = compute_query_for_index(expr->children[1], index);
            if( expr->type == Parser::AND ){
                return left_res and right_res;
            } else {
                return left_res or right_res;
            }
        }
        if( expr->type ==Parser::NOT ){
            bool ch_res = compute_query_for_index(expr->children[0], index);
            return not ch_res;
        }
        return false;
    }
    
public:
    std::vector<int> evaluate_query(const char * q){
        std::vector<int> result;
        Parser::astnode * expr = Parser::parse_query(q);
        optimize_node_with_offsets(expr);
        for(int i = 0; i < database.getData()[0].data.size(); ++i){
            if( compute_query_for_index(expr, i) )
                result.push_back(i);
        }
        Parser::delete_node(expr);
        return result;
    }
    
    std::string serialize_evaluation(const char * q){
        std::vector<int> indexes = evaluate_query(q);
        std::string result = "[\n";
        for( auto i: indexes ){
            result += database.serialize(i);
        }
        return result + "]";
    }
    
    void startEngine (int inpfd, int outfd){
        FILE * inp = fdopen(inpfd, "r");
        FILE * out_ = fdopen(outfd, "w");
        char cmdbuf [20000];
        int request_count = 0;
        long total_time = 0;
        std::cout<< "engine started\n";
        while(fgets(cmdbuf, 20000, inp)){
            long t = clock();
            cmdbuf[strlen(cmdbuf) - 1] = 0;
            std::string q_res = serialize_evaluation(cmdbuf);
            request_count ++;
            total_time += clock() - t;
            fprintf(out_, "got result in %g nanoseconds %s\n", double(clock() - t)/CLOCKS_PER_SEC * 1e6, q_res.c_str());
            
        }
        std::cout << request_count << " requests in " << double(total_time)/CLOCKS_PER_SEC << " sec\nAverage :" << double(total_time)/CLOCKS_PER_SEC/request_count;
        
    }
    
};

#endif /* engine_h */
