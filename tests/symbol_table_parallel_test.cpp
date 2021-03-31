/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file symbol_table_parallel_test.cpp
 *
 * Tests souffle's symbol table in parallel.
 *
 ***********************************************************************/

#include "tests/test.h"

#include "souffle/SymbolTable.h"
#include "souffle/utility/MiscUtil.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace souffle::test {

TEST(SymbolTable, Basics) {
    SymbolTable table;
    const int N = 10;

#pragma omp parallel for num_threads(4)
    for (int i = 0; i < N; i++) {

        EXPECT_STREQ("Hello", table.resolve(table.lookup(table.resolve(table.lookup("Hello")))));

        EXPECT_EQ(table.lookup("Hello"), table.lookup(table.resolve(table.lookup("Hello"))));

        EXPECT_STREQ("Hello", table.resolve(table.lookup(table.resolve(table.lookup("Hello")))));

        EXPECT_EQ(table.lookup("Hello"),
            table.lookup(table.resolve(table.lookup(table.resolve(table.lookup("Hello"))))));
    }
}

TEST(SymbolTable, Basics_DifferentInputs) {
    SymbolTable table;
    const int N = 100;

    for (int i = 0; i < N; i++) {
        table.lookup("Hello" + std::to_string(i));
    }

#pragma omp parallel for num_threads(4)
    for (int i = 0; i < N; i++) {
        const std::string str = "Hello" + std::to_string(i);
        EXPECT_STREQ(str, table.resolve(table.lookup(table.resolve(table.lookup(str)))));

        EXPECT_EQ(table.lookup(str), table.lookup(table.resolve(table.lookup(str))));

        EXPECT_STREQ(str, table.resolve(table.lookup(table.resolve(table.lookup(str)))));

        EXPECT_EQ(table.lookup(str),
            table.lookup(table.resolve(table.lookup(table.resolve(table.lookup(str))))));
    }
}

}  // namespace souffle::test
