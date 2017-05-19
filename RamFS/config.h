#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdint.h>
constexpr size_t BLOCK_SIZE = 1024;
constexpr size_t FS_SIZE = 16 * 1024 * 1024;
constexpr size_t INODE_PER_TUPLE = 42;
constexpr size_t INODE_TUPLE_COUNT = 144;
constexpr size_t INODE_REC_PER_DIRBLOCK = 15;
constexpr size_t NAME_SIZE = 65;
constexpr size_t INODE_COUNT = INODE_PER_TUPLE * INODE_TUPLE_COUNT;
constexpr size_t MAX_PATH_LENGTH = 1024;
typedef uint32_t addr_t;
typedef uint16_t block_no_t;	//14b
typedef uint32_t timestamp_t;
typedef uint16_t inode_no_t;
