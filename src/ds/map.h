
#pragma once

// don't store addresses of elements in a map - at all. Pls.

const f32 MAP_MAX_LOAD_FACTOR = 0.9f;

// from Thomas Wang, http://burtleburtle.net/bob/hash/integer.html
inline u32 hash_u32(u32 key);
inline u32 hash_u64(u64 key);

template<typename K, typename V>
struct map_element {
	K key;
	V value;
	// TODO(max): test if storing hashes in a separate array performs better
	// TODO(max): use less storage than a bool to signify occupation
	bool occupied = false;
	u32 hash_bucket = 0;
};

template<typename K, typename V>
struct map {
	vector<map_element<K, V>> contents;
	u32 size	 		= 0;
	allocator* alloc 	= null;
	u32 (*hash)(K key) 	= null;
	bool use_u32hash 	= false;
	u32 max_probe		= 0;
};

template<typename K, typename V> map<K,V> make_map(i32 capacity = 16, u32 (*hash)(K) = null);
template<typename K, typename V> map<K,V> make_map(i32 capacity, allocator* a, u32 (*hash)(K) = null);
template<typename K, typename V> void destroy_map(map<K,V>* m);

template<typename K, typename V> V* map_insert(map<K,V>* m, K key, V value, bool grow_if_needed = true);
template<typename K, typename V> V* map_insert_if_unique(map<K,V>* m, K key, V value, bool grow_if_needed = true);
template<typename K, typename V> void map_erase(map<K,V>* m, K key);
template<typename K, typename V> void map_clear(map<K,V>* m);

template<typename K, typename V> V* map_get(map<K,V>* m, K key);
template<typename K, typename V> V* map_try_get(map<K,V>* m, K key);

// this is expensive, avoid at all costs. Try to create maps with enough capacity in the first place.
// it will allocate & free another map-sized block for copying from the map's allocator
// this is called from map_insert if the map needs to grow...be wary
template<typename K, typename V> void map_grow_rehash(map<K,V>* m);
template<typename K, typename V> void map_trim_rehash(map<K,V>* m);

