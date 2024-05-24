
typedef struct HashMap {

    void* cells;
    int cellCount;

} HashMap;



int hashMapInit(HashMap** map, int size);

int hashMapInsert(HashMap* map, char* key, int keyLen, void* data);
void hashMapAccess(HashMap* map, char* key, int keyLen, void** data);
void hashMapRemove(HashMap* map, char* key, int keyLen);
