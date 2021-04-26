/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2020, The Souffle Developers. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file record_table_test.cpp
 *
 * Tests the record table.
 *
 ***********************************************************************/

#include "tests/test.h"

#include "souffle/CompiledTuple.h"
#include "souffle/RamTypes.h"
#include "souffle/RecordTable.h"
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include <cstddef>

namespace souffle::test {

#define NUMBER_OF_TESTS 1000

TEST(PackUnpack, Parallel) {
    constexpr size_t vectorSize = 10;

    RecordTable recordTable;

    // Tuples that will be packed
    std::vector<std::vector<RamDomain>> toPack(NUMBER_OF_TESTS);

    // Tuple reference after they are packed.
    std::vector<RamDomain> tupleRef(NUMBER_OF_TESTS);

    // Generate and pack the tuples
    for (size_t i = 0; i < NUMBER_OF_TESTS; ++i) {
        toPack[i] = testutil::generateRandomVector<RamDomain>(10);
        tupleRef[i] = recordTable.pack(toPack[i].data(), vectorSize);
    }

    // pack and unpack
#pragma omp parallel for num_threads(4)
    for (size_t i = 0; i < NUMBER_OF_TESTS; ++i) {
        const RamDomain* unpacked{recordTable.unpack(recordTable.pack(toPack[i].data(), vectorSize), vectorSize)};
        for (size_t j = 0; j < vectorSize; ++j) {
            EXPECT_EQ(toPack[i][j], unpacked[j]);
        }
    }
}

}  // namespace souffle::test
