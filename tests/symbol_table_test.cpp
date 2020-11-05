/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file symbol_table_test.cpp
 *
 * Tests souffle's symbol table.
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

    table.insert("Hello");

    EXPECT_STREQ("Hello", table.resolve(table.lookup(table.resolve(table.lookup("Hello")))));

    EXPECT_EQ(table.lookup("Hello"), table.lookup(table.resolve(table.lookup("Hello"))));

    EXPECT_STREQ("Hello", table.resolve(table.lookup(table.resolve(table.lookup("Hello")))));

    EXPECT_EQ(table.lookup("Hello"),
            table.lookup(table.resolve(table.lookup(table.resolve(table.lookup("Hello"))))));
}

TEST(SymbolTable, Copy) {
    auto* a = new SymbolTable();
    a->insert("Hello");

    auto* b = new SymbolTable(*a);

    size_t a_idx = a->lookup("Hello");
    size_t b_idx = b->lookup("Hello");

    // hash should be the same
    EXPECT_EQ(a_idx, b_idx);

    EXPECT_STREQ("Hello", a->resolve(a_idx));
    EXPECT_STREQ("Hello", b->resolve(b_idx));

    // should be the same actual string
    EXPECT_STREQ(a->resolve(a_idx), b->resolve(b_idx));

    // b should survive
    delete a;
    EXPECT_STREQ("Hello", b->resolve(b_idx));

    delete b;
}

TEST(SymbolTable, Copy2) {
    std::vector<std::string> symbols = {"Hello", "Hi", "Greeting"};
    auto* a = new SymbolTable();
    for(int i = 0; i < symbols.size(); i++) {
        a->insert(symbols[i]);
    }

    auto* b = new SymbolTable(*a);
    auto* c = new SymbolTable(std::move(*a));

    size_t a_idx[symbols.size()];
    size_t b_idx[symbols.size()];
    size_t c_idx[symbols.size()];
    for(int i = 0; i < symbols.size(); i++) {
        a_idx[i] = static_cast<size_t>(a->lookup(symbols[i]));
        b_idx[i] = static_cast<size_t>(b->lookup(symbols[i]));
        c_idx[i] = static_cast<size_t>(c->lookup(symbols[i]));
    }

    // hash should be the same
    for(int i = 0; i < symbols.size(); i++) {
        EXPECT_EQ(a_idx[i], b_idx[i]);
        EXPECT_EQ(a_idx[i], c_idx[i]);
    }

    // should be the same actual string
    for(int i = 0; i < symbols.size(); i++) {
        EXPECT_STREQ(symbols[i], a->resolve(a_idx[i]));
        EXPECT_STREQ(symbols[i], b->resolve(b_idx[i]));
        EXPECT_STREQ(symbols[i], c->resolve(b_idx[i]));
        EXPECT_STREQ(a->resolve(a_idx[i]), b->resolve(b_idx[i]));
        EXPECT_STREQ(a->resolve(a_idx[i]), c->resolve(b_idx[i]));
    }

    // b and c should survive
    delete a;
    for(int i = 0; i < symbols.size(); i++) {
        EXPECT_STREQ(symbols[i], b->resolve(b_idx[i]));
        EXPECT_STREQ(symbols[i], c->resolve(c_idx[i]));
    }

    delete b;
    delete c;
}

TEST(SymbolTable, Assign) {
    auto* a = new SymbolTable();
    a->insert("Hello");

    SymbolTable b = *a;
    SymbolTable c;

    c = *a;

    size_t a_idx = a->lookup("Hello");
    size_t b_idx = b.lookup("Hello");
    size_t c_idx = c.lookup("Hello");

    // hash should be the same
    EXPECT_EQ(a_idx, b_idx);
    EXPECT_EQ(b_idx, c_idx);

    EXPECT_STREQ("Hello", a->resolve(a_idx));
    EXPECT_STREQ("Hello", b.resolve(b_idx));
    EXPECT_STREQ("Hello", c.resolve(c_idx));

    // b and c should survive
    delete a;
    EXPECT_STREQ("Hello", b.resolve(b_idx));
    EXPECT_STREQ("Hello", c.resolve(c_idx));
}

TEST(SymbolTable, Inserts) {
    // whether to print the recorded times to stdout
    // should be false unless developing
    const bool ECHO_TIME = false;

    // type for very big number
    using T = unsigned long long;
    time_point start;
    time_point end;

    T n = 0;        // counter
    T N = 1000000;  // number of symbols to insert
    // T N = 10000000;   // larger tables for debugging/timing
    // T N = 100000000;  // larger tables for debugging/timing

    SymbolTable X;
    std::string x;

    std::vector<std::string> A;
    A.reserve(N);

    for (T i = 0; i < N; ++i) {
        x = std::to_string(i) + "string";
        A.push_back(x);
    }
    start = now();
    for (T i = 0; i < N; ++i) {
        X.insert(A[i]);  // insert one at a time
    }
    end = now();
    n = duration_in_ns(start, end);  // record the time

    if (ECHO_TIME) {
        std::cout << "Time to insert " << N << " new elements:      " << n << " ns" << std::endl;
    }

    // try inserting all the elements that were just inserted
    start = now();
    X.insert(A);
    end = now();
    n = duration_in_ns(start, end);

    if (ECHO_TIME) {
        std::cout << "Time to insert " << N << " existing elements: " << n << " ns" << std::endl;
    }
}

}  // namespace souffle::test
