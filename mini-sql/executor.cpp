//
//  executor.cpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 25.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//
#define ic_enable 0
#include "executor.hpp"
#include "Asmify.hpp"

std::vector<int> execute_on_collection( const json & coll, CodeGen::frame & frame );
void test_execution(){
    json j = Parser::load_json_from_file("test_exec.json");
    CodeGen::frame fr = CodeGen::generate(Parser::parse_query("name.first == 1 and num < 100"));
    auto r = execute_on_collection(j, fr);
    for(auto idx: r){
        std::cout << j[idx] << "\n";
    }
}

std::vector<int> execute_on_collection( const json & coll, CodeGen::frame & frame ){
    //coll is guaranteed an array
    std::vector<int> results;
    clock_t t = clock();
    AsmWriter wr;
    wr.writeFrame(frame);
    AsmWriter::executable f = wr.goExec();
    //wr.print();
    
    for(int i = 0; i < coll.length(); i++){
        json * obj = const_cast<json*>(&(coll[i]));
        if( f(frame.stack, obj) )
            results.push_back(i);
    }
    std::cout << "executed " << coll.length() << " entries in " << (double)(clock() - t)/CLOCKS_PER_SEC << "\n";
    return results;
}


bool execute_on_object(const json & object, CodeGen::frame & frame){
#if 0
    void ** stackptr = frame.stack;
    bzero(frame.stack, sizeof(frame.stack));
    for( auto & op: frame.operands ){
        switch (op.type) {
            case CodeGen::op::load_global:{
                const char * fname = (const char*)frame.statics[ op.arg ];
                const json * var = &object[fname];
                *stackptr++ = const_cast<json*>(var);
            }break;
            case CodeGen::op::load_static:{
                *stackptr++ = frame.statics[ op.arg ];
            }break;
            case CodeGen::op::and_op:{
                long lhs = (long)*--stackptr;
                long rhs = (long)*--stackptr;
                *stackptr++ = (void*)(lhs & rhs);
            }break;
            case CodeGen::op::eq_comp_int:{
                const json * var = (json*)*--stackptr;
                
                if(var->getType() != json::number){
                    *stackptr++ = (void*)(long)(false);
                    break;
                }
                double lhs = (double)(*var);
                long rhs = long(*--stackptr) >> 1;
                *stackptr++ = (void*)(long)(rhs == lhs);
            }break;
            case CodeGen::op::less_comp_int:{
                const json * var = (json*)*--stackptr;
                
                if(var->getType() != json::number){
                    *stackptr++ = (void*)(long)(false);
                    break;
                }
                double lhs = (double)(*var);
                long rhs = long(*--stackptr) >> 1;
                *stackptr++ = (void*)(rhs > lhs ? 1l : 0l);
            }break;
            case CodeGen::op::gt_comp_int:{
                const json * var = (json*)*--stackptr;
                
                if(var->getType() != json::number){
                    *stackptr++ = (void*)(long)(false);
                    break;
                }
                double lhs = (double)(*var);
                long rhs = long(*--stackptr) >> 1;
                *stackptr++ = (void*)(rhs < lhs ? 1l : 0l);
            }break;
            case CodeGen::op::eq_comp_str:{
                const json * var = (json*)*--stackptr;
                if(var->getType() != json::string){
                    *stackptr++ = (void*)(long)(false);
                    break;
                }
                const char *lhs = var->c_str();
                char * rhs = (char*)*--stackptr;
                *stackptr++ = (void*)(long)(strcmp(lhs, rhs) == 0);
            }break;
            case CodeGen::op::load_prop:{
                
                //ic lookup
                const json * result = 0;
                if( op.i_cache[0] != 0 ){
                    result = (const json*)op.i_cache[0];
                    
                } else {
                    const json * var = (json*)*--stackptr;
                    if( var->getType() != json::dict ){
                        *stackptr++ = const_cast<json*>(&json::undefined);
                        break;
                    }
                    const char * rhs = (const char*)frame.statics[op.arg];
                    result = &(*var)[rhs];

                }
                *stackptr++ = const_cast<json*>(result);
            }break;
            case CodeGen::op::ret:{
                return (long)*--stackptr == 1;
            };
            default:
                break;
        }
    }
#endif
    AsmWriter writer;
    writer.writeFrame(frame);
    AsmWriter::executable f = writer.goExec();
    json & j = const_cast<json&>(object);
    return f(frame.stack, &j);
    
    return false;
}
