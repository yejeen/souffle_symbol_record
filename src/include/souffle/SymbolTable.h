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
    std::vector<const std::string*> numToStr;

    /** Map strings to indices. */
    std::unordered_map<std::string, size_t> strToNum;

    /** Convenience method to place a new symbol in the table, if it does not exist, and return the index of
     * it; otherwise return the index */
    inline size_t newSymbolOfIndex(const std::string& symbol) {
        size_t index;
        auto it = strToNum.find(symbol);
        if (it == strToNum.end()) {
            access.lock();
            it = strToNum.find(symbol);
            if(it == strToNum.end()) { // check if the symbol is added by other threads before getting lock.
                index = numToStr.size();
                strToNum[symbol] = index;
                it = strToNum.find(symbol);
                numToStr.push_back(&it->first);
            } else {
                index = it->second;
            }
            access.unlock();
        } else {
            index = it->second;
        }
        return index;
    }

public:
    SymbolTable() = default;

    SymbolTable(std::initializer_list<std::string> symbols) {
        strToNum.reserve(symbols.size());
        for (const auto& symbol : symbols) {
            strToNum[symbol] = numToStr.size();
            auto it = strToNum.find(symbol);
            numToStr.push_back(&it->first);
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
