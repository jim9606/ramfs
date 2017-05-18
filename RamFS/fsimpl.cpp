#include "fsimpl.h"
#include <time.h>

block_no_t fsimpl::getFreeBlock() const {
	return dev.superblock.block_bitset.size() - dev.superblock.block_bitset.count();
}

bool fsimpl::getFileStackByPath(filestack_t &files, path_t path) const {
	file_t tempFile = rootFile;
	files.clear();
	files.push_back(rootFile);
	for (size_t i = 0; i < path.size(); ++i) {
		auto &sublist = tempFile.getSubNode();
		for (auto it = sublist.cbegin(); it != sublist.cend(); ++it) {
			if (it->name == path.get(i))
				//TODO:
		}
	}
}

fsimpl::fsimpl() {
	//Create rootFile
}

path_t fsimpl::getCurrentDir() const {
	return currentDirPath;
}

bool fsimpl::setCurrentDir(path_t path) {

}

addr_t fsimpl::getFreeSpace() const {
	return getFreeBlock() * BLOCK_SIZE;
}
