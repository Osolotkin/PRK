#include "hash_map.h"

#include <stdint.h>
#include <stdlib.h>

#define ERR_OK 0
#define ERR_MALLOC -1
#define ERR_NOT_EQUAL -2


typedef struct Cell Cell;
struct Cell {

    char* key;
    int keyLen;

    void* data;
    Cell* nextCell;

};



int the_strcmp(char* strA, int lenA, char* strB, int lenB) {

    if (lenA != lenB) return 0;
    for (int i = 0; i < lenA; i++) {
        if (strA[i] != strB[i]) return 0;
    }

    return 1;

}

// FNV-1
#include <stdio.h>
uint64_t hash(char* buff, int len) {

    const uint64_t fnvPrime = 0xcbf29ce484222325; 
    
    uint64_t h = fnvPrime;
    for (int i = 0; i < len; i++) {
        h = (h * fnvPrime) ^ buff[i];
    }

    return h;

}

int hashMapInit(HashMap** map, int size) {

    *map = (HashMap*) malloc(sizeof(HashMap));
    if (!*map) return ERR_MALLOC;
    
    (*map)->cells = calloc(size, sizeof(Cell));
    if (!(*map)->cells) return ERR_MALLOC;

    (*map)->cellCount = size;
    
    return ERR_OK;

}

int hashMapInsert(HashMap* map, char* key, int keyLen, void* data) {
    
    Cell* cell = ((Cell*) map->cells) + (hash(key, keyLen) % map->cellCount);
    while (1) {

        if (!(cell->key)) break;
        if (cell->nextCell) {
            cell = cell->nextCell;
            continue;
        }

        cell->nextCell = (Cell*) calloc(1, sizeof(Cell));
        if (!(cell->nextCell)) return -1;
        
        cell = cell->nextCell;
        
        break;
    
    }

    cell->key = key;
    cell->keyLen = keyLen;
    cell->data = data;

    return ERR_OK;

}

void hashMapAccess(HashMap* map, char* key, int keyLen, void** data) {
    
    Cell* cell = ((Cell*) map->cells) + (hash(key, keyLen) % map->cellCount);
    while (cell) {
        if (the_strcmp(key, keyLen, cell->key, cell->keyLen)) {
            *data = cell->data;
            break;
        } else {
            cell = cell->nextCell;
        }
    }

}

void hashMapRemove(HashMap* map, char* key, int keyLen) {
    
    Cell* cell = ((Cell*) map->cells) + (hash(key, keyLen) % map->cellCount);
    Cell* prevCell;
    
    if (the_strcmp(key, keyLen, cell->key, cell->keyLen)) {
        cell->keyLen = 0;
    }

    prevCell = cell;
    cell = cell->nextCell;

    while (cell) {
        if (the_strcmp(key, keyLen, cell->key, cell->keyLen)) {
            prevCell->nextCell = cell->nextCell;
        }
        cell = cell->nextCell;
    }

}

