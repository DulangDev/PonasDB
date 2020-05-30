//
//  Asmify.hpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 29.05.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef Asmify_hpp
#define Asmify_hpp

#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.hpp"

void printjson(json * j);
static void(*pjs)(json*) = printjson;
class AsmWriter{
    uint8_t * mem;
    int memsize;
    int memcap;
    
    void writeMem(uint8_t * _mem, int size){
        if(size + memsize > memcap){
            memcap += size + 1024;
            mem = (uint8_t*)realloc(mem, memcap);
        }
        memcpy(mem+memsize, _mem, size);
        memsize += size;
    }
    
    
    void writeOp(CodeGen::op operand);
    const CodeGen::frame * mfr;
    void writeHeaders(){
        uint8_t hdr []= {
            0x55, 0x49, 0x89, 0xFF, 0x49, 0x89, 0xF6, 0x49, 0x89, 0xfd
            //rdi is first arg and now r15 and rsi is second arg in r14 (this)
            //then push real stack ptr into r13
        };
        writeMem(hdr, sizeof(hdr));
       
        
    }
    void removeRedundant(){
        uint8_t * memplace;
        char * redStackManipulate = "\x49\x83\xC5\x08\x49\x83\xED\x08";
        int l = sizeof("\x49\x83\xC5\x08\x49\x83\xED\x08");
        while((memplace = (uint8_t*)memmem(mem, memsize, redStackManipulate, l))){
            memmove(memplace, memplace + l, memsize - l - (memplace - mem));
            memsize -= l;
        }
    }
    
public:
    void print(){
        for(int i = 0; i < memsize; i++){
            unsigned char c = mem[i];
            printf("%02X", (int)c);
        }
    }
    void writeFrame(const CodeGen::frame & fr){
        mfr = &fr;
        for(auto op:fr.operands){
            writeOp(op);
        }
        uint8_t footer [] = {0x5d, 0xc3};
        writeMem(footer, sizeof(footer));
        removeRedundant();
    }
    
    using executable = long(*)(void * stack, json* object);
    AsmWriter(){
        memcap = 1024;
        memsize = 0;
        mem = (uint8_t*)malloc(memcap);
        writeHeaders();
    }
    ~AsmWriter(){
        free(mem);
    }
    
    executable goExec() const{
        void * m = mmap(0, memsize, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
        memcpy(m, mem, memsize);
        int status = mprotect(m, memsize, PROT_EXEC|PROT_READ);
        if(status != 0){
            perror("mprotect");
        }
        return (executable)m;
    }
    
};


#endif /* Asmify_hpp */
