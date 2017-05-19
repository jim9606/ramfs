#include "fsimpl.h"
#include <time.h>

block_no_t fsimpl::getFreeBlock() const {
	return (block_no_t)(dev.superblock.block_bitset.size() - dev.superblock.block_bitset.count());
}

inode_t* fsimpl::getInode(inode_no_t no) {
	return &(dev.inode_tuple[no / INODE_PER_TUPLE].rec[no % INODE_PER_TUPLE]);
}

bool fsimpl::getFileStackByPath(filestack_t &files, path_t path){
	file_t tempFile = rootFile;
	files.clear();
	files.push_back(rootFile);
	for (size_t i = 0; i < path.size(); ++i) {
		auto &sublist = tempFile.getSubNode();
		for (auto it = sublist.cbegin(); it != sublist.cend(); ++it) {
			if (it->name == path.get(i))
				files.push_back(file_t(&dev, getInode(it->inode_no)));
		}
	}
	return true;
}

inode_no_t fsimpl::allocInode() {
	inode_no_t inum = 1;
	while (inum < INODE_COUNT && dev.superblock.inode_bitset[inum]) ++inum;
	return 0;
}

void fsimpl::createRootFile() {
	
}

fsimpl::fsimpl() {
	//Create rootFile
}

path_t fsimpl::getCurrentDir() const {
	return currentDirPath;
}

bool fsimpl::setCurrentDir(path_t path) {
	return true;
}

addr_t fsimpl::getFreeSpace() const {
	return getFreeBlock() * BLOCK_SIZE;
}
