#include "fs_struct.h"
#include "time.h"

inode_t::inode_t() {
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
