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
#include "jsondb.h"
#include "codegen.hpp"
#include "executor.hpp"
#include "App.h"

static void test_parser(){
    FILE * f = fopen("tests/source/parser_tests", "r");
    char cmdbuf [2048];
    int ctr = 0;
    while(fgets(cmdbuf, 2048, f)){
        cmdbuf[strlen(cmdbuf) - 1] = 0;
        Parser::astnode * node = Parser::parse_query(cmdbuf);
        std::ofstream writer("test_parser_res"+std::to_string(ctr));
        writer << "TESTING PARSE QUERY INTO AST\nINPUT: "<< cmdbuf << "\nOUTPUT:\n";
        if(node){
            node->print(writer, 0);
            writer << "------------\n";
            CodeGen::generate(node).print(writer);
            
        }
        else writer << "incorrect request";
        
        ctr++;
        writer.close();
    }
}


static void stress_test() {
    engine eng((DB()));
    FILE * in = fopen("tests/source/stress_test", "r");
    FILE * out = fopen("log.out", "w");
    engine(DB()).startEngine(fileno(in), fileno(out));
}

static void test_json(){
    FILE * fin = fopen("tests/source/json.txt", "r");
    char * rdbuf = (char*)malloc(1e8);
    int len = fread(rdbuf, 1, 1e8 - 1, fin);
    rdbuf[len] = 0;
    std::ofstream o("tests/jsout.txt");
    o<<Parser::parse_json(rdbuf);
}




int main(int argc, const char * argv[]) {
    
#define run_tests 0
#if run_tests
    test_parser();
    test_execution();
    //stress_test();
#else
    
#endif
    App app;
    app.route("/", [](auto& conn){
        conn.send_file("index.html");
    });
    app.route("/query", [&](auto& conn){
        auto query_result = app.getDB().execute_query(conn.get_query());
        conn.write_answer(query_result.c_str());
    });
    app.launch();
    
    //eng.startEngine(0, 1);
    
    
    return 0;
}
