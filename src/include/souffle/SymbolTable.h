/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file SymbolTable.h
 *
 * Data container to store symbols of the Datalog program.
 *
 ***********************************************************************/

#pragma once

#include "souffle/RamTypes.h"
#include "souffle/utility/MiscUtil.h"
#include "souffle/utility/ParallelUtil.h"
#include "souffle/utility/StreamUtil.h"
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <initializer_list>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

namespace souffle {

/**
 * @class SymbolTable
 *
 * Global pool of re-usable strings
 *
 * SymbolTable stores Datalog symbols and converts them to numbers and vice versa.
 */
class SymbolTable {
private:
    /** A lock to synchronize parallel accesses */
    mutable Lock access;

    /** Map indices to string pointers. */
    tbb::concurrent_vector<const std::string*> numToStr;

    /** Map strings to indices. */
    tbb::concurrent_hash_map<std::string, size_t> strToNum;

    /** Convenience method to place a new symbol in the table, if it does not exist, and return the index of
     * it; otherwise return the index */
    inline size_t newSymbolOfIndex(const std::string& symbol) {
        tbb::concurrent_hash_map<std::string, size_t>::accessor accessor;
        size_t index;
        if(strToNum.find(accessor, symbol)) {
            index = accessor->second;
        } else {
            access.lock();
            if(strToNum.find(accessor, symbol)) {
                index = accessor->second;
            } else {
                index = numToStr.size();
                strToNum.insert(accessor, symbol);
                accessor->second = index;
                numToStr.push_back(&accessor->first);
            }
            access.unlock();
        }
        return index;
    }

public:
    SymbolTable() = default;

    SymbolTable(std::initializer_list<std::string> symbols) {
        tbb::concurrent_hash_map<std::string, size_t>::accessor accessor;
        for (const auto& symbol : symbols) {
            strToNum.insert(accessor, symbol);
            accessor->second = numToStr.size();
            numToStr.push_back(&accessor->first);
        }
    }

    virtual ~SymbolTable() = default;

    /** Find the index of a symbol in the table, inserting a new symbol if it does not exist there
     * already. */
    RamDomain lookup(const std::string& symbol) {
        {
            return static_cast<RamDomain>(newSymbolOfIndex(symbol));
        }
    }

    /** Find the index of a symbol in the table, inserting a new symbol if it does not exist there
     * already. */
    RamDomain unsafeLookup(const std::string& symbol) {
        return static_cast<RamDomain>(newSymbolOfIndex(symbol));
    }

    /** Find a symbol in the table by its index, note that this gives an error if the index is out of
     * bounds.
     */
    const std::string& resolve(const RamDomain index) const {
        {
            auto pos = static_cast<size_t>(index);
            if (pos >= size()) {
                // TODO: use different error reporting here!!
                fatal("Error index out of bounds in call to `SymbolTable::resolve`. index = `%d`", index);
            }
            auto result = numToStr[pos];
            return *result;
        }
    }

    const std::string& unsafeResolve(const RamDomain index) const {
        return *numToStr[static_cast<size_t>(index)];
    }

    /* Return the size of the symbol table, being the number of symbols it currently holds. */
    size_t size() const {
        return numToStr.size();
    }
};

}  // namespace souffle
