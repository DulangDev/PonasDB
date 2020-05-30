//
//  Asmify.cpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 29.05.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include "Asmify.hpp"
#include <iostream>

json&(json::*getjsstringprop)(const char*)  = &json::operator[];
json&(json::*getjsnumprop)(int) = &json::operator[];
long (json::*optolong)(void) const = &json::operator long;
uint8_t callrax[] = {0xff, 0xd0};
uint8_t pushthis [] = { 0x4c, 0x89, 0xf7 };
uint8_t stackplus [] = { 0x49, 0x83, 0xC5, 0x08 };
uint8_t stackminus [] = { 0x49, 0x83, 0xED, 0x08 };
uint8_t movabstorax [] = { 0x48, 0xb8};


void printjson(json * j){
    std::cout << *j << std::endl;
    printf("%p\n", j);
}

void AsmWriter::writeOp(CodeGen::op operand){
    switch (operand.type) {
        case CodeGen::op::load_global:{
        
            const char * name = (const char*)(mfr)->statics[operand.arg];
            uint8_t arr[] = {
                0x48, 0xBE
            };
            //mov rsi name
            writeMem(arr, sizeof(arr));
            writeMem((uint8_t*)&name, 8);
            
            writeMem(pushthis, sizeof(pushthis));
            
            uint8_t push_rax[] = {
                0x48, 0xB8
            };
            writeMem(push_rax, 2);
            writeMem((uint8_t*)&getjsstringprop, 8);
            writeMem(callrax, sizeof(callrax)); //call rax
            uint8_t pushres [] ={ 0x49, 0x89, 0x45, 0x00 };
            writeMem(pushres, sizeof(pushres));
            writeMem(stackplus, sizeof(stackplus));
            /*const char * fname = (const char*)frame.statics[ op.arg ];
            const json * var = &object[fname];
            *stackptr++ = const_cast<json*>(var);*/
        }break;
        case CodeGen::op::load_static:{
            void * staticc = mfr->statics[ operand.arg ];
            writeMem(movabstorax, sizeof(movabstorax));
            writeMem((uint8_t*)&staticc, 8);
            uint8_t movtostackfromrax [] = { 0x49, 0x89, 0x45, 0x00 };
            writeMem(movtostackfromrax, sizeof(movtostackfromrax));
            writeMem(stackplus, sizeof(stackplus));
            
           /* *stackptr++ = frame.statics[ op.arg ];*/
        }break;
        case CodeGen::op::and_op:{
            /*long lhs = (long)*--stackptr;
            long rhs = (long)*--stackptr;
            *stackptr++ = (void*)(lhs & rhs);*/
        }break;
        case CodeGen::op::eq_comp_int:{
            writeMem(stackminus, sizeof(stackminus));
#warning TODO: add typecheck
            uint8_t first_part [] = {  0x49, 0x8B, 0x7D, 0x00, 0x48, 0xB8, };
            writeMem(first_part, sizeof(first_part));
            writeMem((uint8_t*)&optolong, 8);
            writeMem(callrax, sizeof(callrax));
            
            //here we have in rax longified var
            writeMem(stackminus, sizeof(stackminus));
            uint8_t movtostackfromrbx [] = {0x49, 0x8B, 0x75, 0x00  };
            writeMem(movtostackfromrbx, sizeof(movtostackfromrbx));
            uint8_t rshift [] = {0x48, 0xd1, 0xee };
            writeMem(rshift, sizeof(rshift));
            //var in rax and val in rbx
            //we will sub them  rax therefore the value will be negated witwise
            uint8_t cmp [] = {0x48, 0x39, 0xF0, 0x41, 0x0F, 0x94, 0xC0, 0x49, 0x0F, 0xB6, 0xC0 };
            writeMem(cmp, sizeof(cmp));
            
            
            uint8_t pushres [] ={ 0x49, 0x89, 0x45, 0x00 };
            writeMem(pushres, sizeof(pushres));
            writeMem(stackplus, sizeof(stackplus));
            
            /*const json * var = (json*)*--stackptr;
            
            if(var->getType() != json::number){
                *stackptr++ = (void*)(long)(false);
                break;
            }
            double lhs = (double)(*var);
            long rhs = long(*--stackptr) >> 1;
            *stackptr++ = (void*)(long)(rhs == lhs);*/
        }break;
        case CodeGen::op::less_comp_int:{
            /*const json * var = (json*)*--stackptr;
            
            if(var->getType() != json::number){
                *stackptr++ = (void*)(long)(false);
                break;
            }
            double lhs = (double)(*var);
            long rhs = long(*--stackptr) >> 1;
            *stackptr++ = (void*)(rhs > lhs ? 1l : 0l);*/
        }break;
        case CodeGen::op::gt_comp_int:{
           /* const json * var = (json*)*--stackptr;
            
            if(var->getType() != json::number){
                *stackptr++ = (void*)(long)(false);
                break;
            }
            double lhs = (double)(*var);
            long rhs = long(*--stackptr) >> 1;
            *stackptr++ = (void*)(rhs < lhs ? 1l : 0l);*/
        }break;
        case CodeGen::op::eq_comp_str:{
            /*const json * var = (json*)*--stackptr;
            if(var->getType() != json::string){
                *stackptr++ = (void*)(long)(false);
                break;
            }
            const char *lhs = var->c_str();
            char * rhs = (char*)*--stackptr;
            *stackptr++ = (void*)(long)(strcmp(lhs, rhs) == 0);*/
        }break;
        case CodeGen::op::load_prop:{
            /*
            //ic lookup
            const json * result = 0;
           
                const json * var = (json*)*--stackptr;
                if( var->getType() != json::dict ){
                    *stackptr++ = const_cast<json*>(&json::undefined);
                    break;
                }
                const char * rhs = (const char*)frame.statics[op.arg];
                result = &(*var)[rhs];

            
            *stackptr++ = const_cast<json*>(result);
             */
        }break;
        case CodeGen::op::ret:{
            writeMem(stackminus, sizeof(stackminus));
            uint8_t pushstacktorax [] = { 0x49, 0x89, 0x45, 0x00};
            writeMem(pushstacktorax, sizeof(pushstacktorax));
            /*
            return (long)*--stackptr == 1;*/
        };
            
        default:
            break;
    }
}
