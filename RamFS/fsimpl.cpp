#include "fsimpl.h"
#include <time.h>

block_no_t fsimpl::getFreeBlock() const {
	return (block_no_t)(dev.superblock.block_bitset.size() - dev.superblock.block_bitset.count());
}

inode_t* fsimpl::getInode(inode_no_t no) {
	return &(dev.inode_tuple[no / INODE_PER_TUPLE].rec[no % INODE_PER_TUPLE]);
}

bool fsimpl::getFileStackByPath(vector<inode_no_t> &files, path_t path){
	file_t tempFile;
	files.clear();
	files.push_back(0);
	for (size_t i = 0; i < path.size(); ++i) {
		tempFile = file_t(&dev, getInode(files.back()));
		if (!tempFile.isDir()) return false;//Not a directory
		auto &sublist = tempFile.sub_inode_rec;
		inode_no_t nextInode = 0;
		for (auto it = sublist.cbegin(); it != sublist.cend(); ++it) {
			if (it->name == path.get(i)) {
				nextInode = it->inode_no;
				break;
			}
		}
		if (nextInode == 0) return false;//Cannot find such file
		files.push_back(nextInode);
	}
	return true;
}

inode_no_t fsimpl::allocInode() {
	inode_no_t inum = 1;
	while (inum < INODE_COUNT && dev.superblock.inode_bitset[inum]) ++inum;
	return 0;
}

void fsimpl::createRootFile() {
	inode_t *ri = getInode(0);
	ri->flags = inode_t::f_valid | inode_t::f_dir;
	ri->size = 0;
	ri->ctime = time(0);
	ri->atime = time(0);
	ri->rec_count = 0;

	dev.superblock.inode_bitset.set(0);
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

file_t fsimpl::getFileByName(string name) {
	return getFileByInodeNo(getFile(name));
}
