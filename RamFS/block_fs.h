#pragma once
//This file contains persistent objects
#include"config.h"
#include<bitset>

using std::bitset;

struct inode_t {
	char magic;
	static constexpr char magic_val = 'I';

	uint8_t flags;
	static constexpr uint8_t f_valid = 0b00000001;
	static constexpr uint8_t f_file  = 0b00000010;
	static constexpr uint8_t f_dir   = 0b00000100;
	addr_t size;
	timestamp_t ctime;
	timestamp_t atime;
	block_no_t d_ref;
	block_no_t i_ref;
};

struct sub_inode_rec_t {
	inode_no_t inode_no;
	char name[NAME_SIZE];
};

//Its size is BLOCK_SIZE
struct dir_block_t {
	char magic[2];
	static constexpr char magic_val[2] = "D";
	uint8_t rec_count;
	sub_inode_rec_t rec[15];
};

//Its size is BLOCK_SIZE
struct indirect_block_t {
	uint16_t rec_count;
	block_no_t block_no[511];
};

//Its size is BLOCK_SIZE
struct data_block_t {
	char b[BLOCK_SIZE];
};

//Its size is BLOCK_SIZE
struct inode_tuple_t {
	char magic[8];
	static constexpr char magic_val[8] = "ITUPLE-";
	char padding[16];
	inode_t rec[INODE_PER_TUPLE];
};

//Its size is 3*BLOCK_SIZE
struct superblock_t {
	char magic[8];
	static constexpr char magic_val[8] = "RAMFS--";
	char padding[229];
	bitset<INODE_PER_TUPLE * INODE_TUPLE_COUNT> inode_bitset;
	bitset<FS_SIZE / BLOCK_SIZE> block_bitset;
};

struct block_dev {
	superblock_t superblock;
	inode_tuple_t inode_tuple[INODE_TUPLE_COUNT];
	data_block_t data_block[(FS_SIZE - sizeof(superblock_t) - BLOCK_SIZE * INODE_TUPLE_COUNT) / BLOCK_SIZE];

};