//
//  main.cpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 17.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#include <iostream>
#include "engine.h"
#include <fstream>
#include <string>


void test_parser(){
    FILE * f = fopen("tests/source/parser_tests", "r");
    char cmdbuf [2048];
    int ctr = 0;
    while(fgets(cmdbuf, 2048, f)){
        cmdbuf[strlen(cmdbuf) - 1] = 0;
        Parser::astnode * node = Parser::parse_query(cmdbuf);
        std::ofstream writer("test_parser_res"+std::to_string(ctr));
        writer << "TESTING PARSE QUERY INTO AST\nINPUT: "<< cmdbuf << "\nOUTPUT:\n";
        node->print(writer, 0);
        ctr++;
        writer.close();
    }
}


int main(int argc, const char * argv[]) {
    // insert code here...
    engine eng;
#define run_tests 1
#if run_tests
    test_parser();
    FILE * in = fopen("tests/source/stress_test", "r");
    FILE * out = fopen("log.out", "w");
    engine().startEngine(fileno(in), fileno(out));
#endif
    eng.startEngine(0, 1);
    
    
    return 0;
}
