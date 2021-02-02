/*
** Hash function.
*/


#ifndef __HASH_H_
#define __HASH_H_

#include <string>

namespace ADT {

    int hash(int key);
    int hash(bool key);
    int hash(char key);
    int hash(unsigned int key);
    int hash(long key);
    int hash(unsigned long key);
    int hash(short key);
    int hash(unsigned short key);

    int hash(double key);
    int hash(float key);
    int hash(long double key);

    int hash(const char* str);
    int hash(const std::string& str);
    int hash(void* key);

    int hash_seed();
    int hash_mult();


// hash for a pair
    template <typename T, typename U>
    int hash(T&& t, U&& u) {
        return hash(hash(t) + hash_mult() * (hash(u) + hash_seed()));
    }

}


#endif // __HASH_H_
