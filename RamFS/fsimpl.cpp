#include "fsimpl.h"
#include <time.h>

block_no_t fsimpl::getFreeBlock() const {
	return (block_no_t)(dev.superblock.block_bitset.size() - dev.superblock.block_bitset.count());
}

inode_t* fsimpl::getInode(inode_no_t no) {
	return &(dev.inode_tuple[no / INODE_PER_TUPLE].rec[no % INODE_PER_TUPLE]);
}

bool fsimpl::getFileStackByPath(files_t &files, path_t path){
	file_t tempFile = file_t(&dev, 0, "");
	files.clear();
	files.push_back(0);
	for (size_t i = 0; i < path.size(); ++i) {
		if (!tempFile.isDir()) return false;//Not a directory
		auto &sublist = tempFile.sub_inode_rec;
		auto nextInode = sublist.cend();
		for (auto it = sublist.cbegin(); it != sublist.cend(); ++it) {
			if (it->name == path.get(i)) {
				nextInode = it;
				break;
			}
		}
		if (nextInode == sublist.cend()) return false;//Cannot find such file
		files.push_back(nextInode->inode_no);
		tempFile = file_t(&dev, getInode(files.back()),nextInode->name);
	}
	return true;
}

inode_no_t fsimpl::allocInode() {
	inode_no_t inum = 1;
	while (inum < INODE_COUNT && dev.superblock.inode_bitset[inum]) ++inum;
	dev.superblock.inode_bitset.set(inum);
	return inum;
}

void fsimpl::createRootFile() {
	inode_t *ri = getInode(0);
	ri->flags = inode_t::f_valid | inode_t::f_dir;
	ri->size = 0;
	ri->ctime = time(0);
	ri->atime = time(0);
	ri->rec_count = 0;

	dev.superblock.inode_bitset.set(0);
	rootInode = ri;
	currentDirFile = file_t(&dev, ri, "");
}

fsimpl::fsimpl() {
	//Create rootFile
	createRootFile();
	currentDirFileStack.clear();
	currentDirFileStack.push_back(0);
}

path_t fsimpl::getCurrentDir() const {
	return currentDirPath;
}

bool fsimpl::setCurrentDir(path_t path) {
	//拼接相对路径
	if (path.front() == "//") {
		path_t rootPath = getCurrentDir();
		path = rootPath.append(path);
	}
	
	files_t npwd;
	bool b = getFileStackByPath(npwd, path);
	if (b) {
		if (!(getInode(npwd.back())->flags & inode_t::f_dir)) return false;//It's a file
		currentDirFileStack = npwd;
		currentDirPath = path;
	}
	return b;
}

inode_no_t fsimpl::getFile(string name) const{
	for (auto const &it : currentDirFile.sub_inode_rec) {
		if (it.name == name) {
			return it.inode_no;
		}
	}
	return 0;
}

file_t fsimpl::getFileByName(string name) {
	for (auto const &it : currentDirFile.sub_inode_rec) {
		if (it.name == name) {
			return file_t(&dev, getInode(it.inode_no), it.name);
		}
	}
	return file_t();
}

inode_no_t fsimpl::createFile(string name, addr_t size) {
	
}

addr_t fsimpl::getFreeSpace() const {
	return getFreeBlock() * BLOCK_SIZE;
}

