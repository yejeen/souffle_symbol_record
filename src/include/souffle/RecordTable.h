/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2020, The Souffle Developers. All rights reserved.
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file RecordTable.h
 *
 * Data container implementing a map between records and their references.
 * Records are separated by arity, i.e., stored in different RecordMaps.
 *
 ***********************************************************************/

#pragma once

#include "souffle/CompiledTuple.h"
#include "souffle/RamTypes.h"
#include <cassert>
#include <cstddef>
#include <limits>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_hash_map.h>

namespace souffle {

/** @brief Bidirectional mappping between records and record references */
class RecordMap {
    /** arity of record */
    const size_t arity;

    /** hash function for unordered record map */
    struct RecordHash {
        static size_t hash( const std::vector<RamDomain>& record ) {
            std::size_t seed = 0;
            std::hash<RamDomain> domainHash;
            for (RamDomain value : record) {
                seed ^= domainHash(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
        static bool equal( const std::vector<RamDomain>& a, const std::vector<RamDomain>& b ) {
            return a==b;
        }
    };

    /** map from records to references */
    tbb::concurrent_hash_map<std::vector<RamDomain>, RamDomain, RecordHash> recordToIndex;

    /** array of records; index represents record reference */
    tbb::concurrent_vector<const RamDomain*> indexToRecord;

public:
    explicit RecordMap(size_t arity) : arity(arity), indexToRecord(1) {}  // note: index 0 element left free

    /** @brief converts record to a record reference */
    // TODO (b-scholz): replace vector<RamDomain> with something more memory-frugal
    RamDomain pack(const std::vector<RamDomain>& vector) {
        tbb::concurrent_hash_map<std::vector<RamDomain>, RamDomain, RecordHash>::accessor accessor;
        RamDomain index;
        if (recordToIndex.find(accessor, vector)) {
            index = accessor->second;
        } else {
            index = static_cast<RamDomain>(indexToRecord.size());
            recordToIndex.insert(accessor, vector);
            accessor->second = index;
            indexToRecord.push_back((&accessor->first)->data());

            // assert that new index is smaller than the range
            assert(index != std::numeric_limits<RamDomain>::max());
        }
        return index;
    }

    /** @brief convert record pointer to a record reference */
    RamDomain pack(const RamDomain* tuple) {
        // TODO (b-scholz): data is unnecessarily copied
        // for a successful lookup. To avoid this, we should
        // compute a hash of the pointer-array and traverse through
        // the bucket list of the unordered map finding the record.
        // Note that in case of non-existence, the record still needs to be
        // copied for the newly created entry but this will be the less
        // frequent case.
        std::vector<RamDomain> tmp(arity);
        for (size_t i = 0; i < arity; i++) {
            tmp[i] = tuple[i];
        }
        return pack(tmp);
    }

    /** @brief convert record reference to a record pointer */
    const RamDomain* unpack(RamDomain index) const {
        const RamDomain* res;
        res = indexToRecord[index];
        return res;
    }
};

class RecordTable {
public:
    RecordTable() = default;
    virtual ~RecordTable() = default;

    /** @brief convert record to record reference */
    RamDomain pack(RamDomain* tuple, size_t arity) {
        return lookupArity(arity).pack(tuple);
    }
    /** @brief convert record reference to a record */
    const RamDomain* unpack(RamDomain ref, size_t arity) const {
        tbb::concurrent_hash_map<size_t, RecordMap>::const_accessor accessor;

        bool result = maps.find(accessor, arity);
        assert(result && "Attempting to unpack record for non-existing arity");

        return (accessor->second).unpack(ref);
    }

private:
    /** @brief lookup RecordMap for a given arity; if it does not exist, create new RecordMap */
    RecordMap& lookupArity(size_t arity) {
        tbb::concurrent_hash_map<size_t, RecordMap>::accessor accessor;

        // This will create a new map if it doesn't exist yet.
        maps.emplace(accessor, arity, arity);

        return accessor->second;
    }

    /** Arity/RecordMap association */
    tbb::concurrent_hash_map<size_t, RecordMap> maps;
};

/** @brief helper to convert tuple to record reference for the synthesiser */
template <std::size_t Arity>
inline RamDomain pack(RecordTable& recordTab, Tuple<RamDomain, Arity> tuple) {
    return recordTab.pack(static_cast<RamDomain*>(tuple.data), Arity);
}

}  // namespace souffle
