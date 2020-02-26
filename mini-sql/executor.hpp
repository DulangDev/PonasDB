//
//  executor.hpp
//  mini-sql
//
//  Created by Дмитрий Маслюков on 25.02.2020.
//  Copyright © 2020 Дмитрий Маслюков. All rights reserved.
//

#ifndef executor_hpp
#define executor_hpp

#include "codegen.hpp"
void test_execution();
static bool execute_on_object(const json &, CodeGen::frame &);
std::vector<int> execute_on_collection( const json & coll, CodeGen::frame & frame );
#endif /* executor_hpp */
