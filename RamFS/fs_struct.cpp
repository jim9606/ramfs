#include "fs_struct.h"

superblock_t::superblock_t() {
	memset(this, 0, sizeof(superblock_t));
	strcpy(magic, magic_val);
	for (size_t i = 0; i < PROVISION_BLOCK; ++i)
		block_bitset.set(i);
	inode_bitset.set(0);
}