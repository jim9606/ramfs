#pragma once
//This file contains persistent objects
#include "config.h"
#include <bitset>

using std::bitset;

struct inode_t {
	static constexpr char magic_val = 'I';
	char magic;
	
	static constexpr uint8_t f_valid = 0b00000001;
	static constexpr uint8_t f_file = 0b00000010;
	static constexpr uint8_t f_dir = 0b00000100;
	uint8_t flags;

	addr_t size;
	timestamp_t ctime;
	timestamp_t atime;
	/*
	for file inode, it means number of data_block
	for directory inode, it means number of children
	*/
	uint16_t rec_count;
	block_no_t d_ref;
	block_no_t i_ref;

};

struct sub_inode_rec_t {
	inode_no_t inode_no;
	char name[NAME_SIZE];
};

//Its size is BLOCK_SIZE
struct dir_block_t {
	static constexpr char magic_val[4] = "DIR";
	char magic[4];

	sub_inode_rec_t rec[INODE_REC_PER_DIRBLOCK];
};

//Its size is BLOCK_SIZE
struct indirect_block_t {
	block_no_t block_no[512];
};

//Its size is BLOCK_SIZE
struct data_block_t {
	char b[BLOCK_SIZE];
};

//Its size is BLOCK_SIZE
struct inode_tuple_t {
	static constexpr char magic_val[8] = "INTUPLE";
	char magic[8];
	char padding[8];

	inode_t rec[INODE_PER_TUPLE];
};

//Its size is 3*BLOCK_SIZE
struct superblock_t {
	static constexpr char magic_val[8] = "FSSUPER";
	char magic[8];
	char padding[256];

	bitset<INODE_PER_TUPLE * INODE_TUPLE_COUNT> inode_bitset;
	bitset<FS_SIZE / BLOCK_SIZE> block_bitset;

	superblock_t();
};

constexpr size_t MAX_DATA_BLOCK = (FS_SIZE - sizeof(superblock_t) - BLOCK_SIZE * INODE_TUPLE_COUNT) / BLOCK_SIZE;
constexpr size_t PROVISION_BLOCK = FS_SIZE / BLOCK_SIZE - MAX_DATA_BLOCK;

struct block_dev {
	superblock_t superblock;
	inode_tuple_t inode_tuple[INODE_TUPLE_COUNT];
	data_block_t data_block[MAX_DATA_BLOCK];
};