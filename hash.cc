/*
** Hash function.
*/

#include <cstring>
#include "hash.hh"



namespace ADT {

    const constexpr int HASH_SEED = 5381;
    const constexpr int HASH_MULT = 33;
    const constexpr int HASH_MASK = unsigned(-1) >> 1; 

    int hash_seed() {
        return HASH_SEED;
    }

    int hash_mult() {
        return HASH_MULT;
    }

    int hash(int key) {
        return key & HASH_MASK;
    }

    int hash(bool key) {
        return hash(static_cast<int>(key));
    }

    int hash(char key) {
        return hash(static_cast<int>(key));
    }

    int hash(unsigned int key) {
        return hash(static_cast<int>(key));
    }

    int hash(long key) {
        return hash(static_cast<int>(key));
    }

    int hash(unsigned long key) {
        return hash(static_cast<int>(key));
    }

    int hash(short key) {
        return hash(static_cast<int>(key));
    }

    int hash(unsigned short key) {
        return hash(static_cast<int>(key));
    }

    //non-int types
    int hash_other(const char* start, size_t k) {
        unsigned hash_code = HASH_SEED;
        for (size_t i = 0; i < k; i++) {
            hash_code = HASH_MULT * hash_code + start[i];
        }
        return hash(hash_code);
    } 

    int hash(double key) {
        return hash(reinterpret_cast<const char *>(&key), sizeof(double));
    }

    int hash(float key) {
        return hash(reinterpret_cast<const char *>(&key), sizeof(float));
    }

    int hash(long double key) {
        return hash(reinterpret_cast<const char *>(&key), sizeof(long double));
    }

    int hash(const char* str) {
        return hash(str, std::strlen(str));
    }

    int hash(const std::string& str) {
        return hash(str.data(), str.length());
    }

    int hash(void* key) {
        return hash(reinterpret_cast<uintptr_t>(key));
    }
    
    
}
