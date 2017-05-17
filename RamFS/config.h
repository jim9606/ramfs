#pragma once
#include<stdint.h>
constexpr size_t BLOCK_SIZE = 1024;
constexpr size_t FS_SIZE = 16 * 1024 * 1024;
constexpr size_t INODE_PER_TUPLE = 50;
constexpr size_t INODE_TUPLE_COUNT = 125;
constexpr size_t NAME_SIZE = 65;
typedef unsigned int24_t;
typedef uint32_t addr_t;
typedef uint16_t block_no_t;	//14b
typedef uint32_t timestamp_t;
typedef uint16_t inode_no_t;
