#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"

struct hashtable {
    int num_buckets;
    int size;
    linkedlist_t **array;
    // TODO: define hashtable struct to use linkedlist as buckets
};

/**
 * Hash function to hash a key into the range [0, max_range)
 */
static int hash(int key, int max_range) {
    // TODO: feel free to write your own hash function (NOT REQUIRED)
    key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets) {
    hashtable_t *table = malloc(sizeof(hashtable_t));
    table -> array = malloc(num_buckets * sizeof(linkedlist_t*));
    for (int i = 0; i < num_buckets; i++)
        table -> array[i] = ll_init();
    table -> size = 0;
    table -> num_buckets = num_buckets;
    return table;
    // TODO: create a new hashtable
}

void ht_add(hashtable_t *table, int key, int value) {
    int bucket = hash(key, table -> num_buckets);
    int size = ll_size(table -> array[bucket]);
    ll_add(table -> array[bucket], key, value);
    if (ll_size(table -> array[bucket]) > size) table -> size++;
    // TODO: create a new mapping from key -> value.
    // If the key already exists, replace the value.
}

int ht_get(hashtable_t *table, int key) {
    int bucket = hash(key, table -> num_buckets);
    return ll_get(table -> array[bucket], key);
    // TODO: retrieve the value mapped to the given key.
    // If it does not exist, return 0
}

int ht_size(hashtable_t *table) {
    return table -> size;
    // TODO: return the number of mappings in this hashtable
}
