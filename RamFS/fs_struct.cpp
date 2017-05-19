#include "fs_struct.h"
#include "time.h"

inode_t::inode_t() {
	init();
}

void inode_t::init() {
	memset(this, 0, sizeof(inode_t));
	magic = magic_val;
	flags = 0;
	ctime = time(0);
	atime = ctime;
}

sub_inode_rec_t::sub_inode_rec_t() {
	inode_no = 0;
	strcpy(name, "");
}

dir_block_t::dir_block_t() {
	init();
}

void dir_block_t::init() {
	memset(this, 0, sizeof(dir_block_t));
	strcpy(magic, magic_val);
}

inode_tuple_t::inode_tuple_t() {
	strcpy(magic, magic_val);
}

superblock_t::superblock_t() {
	memset(this, 0, sizeof(superblock_t));
	strcpy(magic, magic_val);
	for (size_t i = 0; i < PROVISION_BLOCK; ++i)
		block_bitset.set(i);
	inode_bitset.set(0);
}
