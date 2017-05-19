#include "fsimpl.h"
#include "utils.h"
#include <time.h>

block_no_t fsimpl::getFreeBlock() const {
	return (block_no_t)(dev.superblock.block_bitset.size() - dev.superblock.block_bitset.count());
}

inode_t* fsimpl::getInode(inode_no_t no) {
	return &(dev.inode_tuple[no / INODE_PER_TUPLE].rec[no % INODE_PER_TUPLE]);
}

indirect_block_t* fsimpl::getIBlock(block_no_t no) {
	return (indirect_block_t*) &(dev.data_block[no - PROVISION_BLOCK * BLOCK_SIZE]);
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

block_no_t fsimpl::allocBlock(block_no_t no) {
	while (no < TOTAL_BLOCK_COUNT && dev.superblock.block_bitset[no]) ++no;
	dev.superblock.block_bitset.set(no);
	return no;
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


void fsimpl::addNewSubForCD(const inode_no_t sub,const string subName) {
	auto &CDInode = currentDirFile.inode;
	auto &CDrecCount = CDInode->rec_count;
	dir_block_t *DBlock = nullptr;
	const size_t recOrd = CDrecCount % INODE_REC_PER_DIRBLOCK;

	if (recOrd == 0) {
		//需要新数据块
		block_no_t newBlockNo = allocBlock(PROVISION_BLOCK);
		if (CDrecCount == 0) 
			CDInode->d_ref = newBlockNo;
		else
			CDInode->i_ref = newBlockNo;
		DBlock = (dir_block_t*)getIBlock(newBlockNo);
		DBlock->init();
	}
	else {
		assert(CDrecCount >= 2 * INODE_REC_PER_DIRBLOCK);
		if (CDrecCount < INODE_REC_PER_DIRBLOCK)
			dir_block_t *DBlock = (dir_block_t*)getIBlock(CDInode->d_ref);	//在第一数据块追加
		else
			dir_block_t *DBlock = (dir_block_t*)getIBlock(CDInode->i_ref);	//在第二数据块追加
	}
	DBlock->rec[recOrd].inode_no = sub;
	strcpy(DBlock->rec[recOrd].name, subName.c_str());
	++CDrecCount;
	currentDirFile.init(&dev, CDInode, currentDirFile.getName());
}

bool fsimpl::deleteDirTree(inode_no_t no) {
	inode_t *inode = getInode(no);
	inode->flags &= ~inode_t::f_valid;
	dev.superblock.inode_bitset.set(no, false);
	//TODO:
	if (inode->rec_count == 0) return true;
	//擦除第二数据块
	if (inode->rec_count > INODE_REC_PER_DIRBLOCK) {
		dir_block_t *DBlock = (dir_block_t*)getIBlock(inode->i_ref);
		auto n = inode->rec_count - INODE_REC_PER_DIRBLOCK;
		for (size_t i = 0; i < n; ++i)
			deleteDirTree(DBlock->rec[i].inode_no);
		inode->rec_count = INODE_REC_PER_DIRBLOCK;
	}
	//擦除第一数据块
	dir_block_t *DBlock = (dir_block_t*)getIBlock(inode->d_ref);
	for (size_t i = 0; i < inode->rec_count; ++i)
		deleteDirTree(DBlock->rec[i].inode_no);
}

fsimpl::fsimpl() {
	//Create rootFile
	createRootFile();
	currentDirFileStack.clear();
	currentDirFileStack.push_back(0);
}

int fsimpl::checkFileName(string name) {
	if (name.size() > NAME_SIZE) return -1;	//ERR:文件名过长
	if (name.find('/') != std::string::npos) return -2; //ERR:包含非法字符
	return 0;
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
	if (checkFileName(name) != 0) return 0; //ERR:文件名非法
	for (auto const &it : currentDirFile.sub_inode_rec) {
		if (it.name == name)
			return 0;	//ERR:重名
	}

	//计算需要的块数目
	inode_no_t wantedBlockNum = size / BLOCK_SIZE;
	if (size % BLOCK_SIZE > 0) ++wantedBlockNum;
	//ERR:超出最大子文件限制
	if (currentDirFile.inode->rec_count == 2 * INODE_REC_PER_DIRBLOCK) return 0;
	//需要间接块
	if (size > BLOCK_SIZE) ++wantedBlockNum;
	//ERR:超出单文件限制
	if (wantedBlockNum > BLOCKNO_PER_IBLOCK + 1) return 0;

	//判断工作目录是否需要扩充
	if (currentDirFile.inode->rec_count % INODE_REC_PER_DIRBLOCK == 0) ++wantedBlockNum;
	//ERR:空间不足
	if (wantedBlockNum > getFreeBlock()) return 0;
	//减去工作目录扩充块
	if (currentDirFile.inode->rec_count % INODE_REC_PER_DIRBLOCK == 0) --wantedBlockNum;
	//减去间接块
	if (size > BLOCK_SIZE) --wantedBlockNum;

	inode_no_t newInodeNo = allocInode();
	inode_t *newInode = getInode(newInodeNo);
	newInode->init();
	newInode->flags = inode_t::f_valid | inode_t::f_file;
	newInode->size = size;
	newInode->rec_count = wantedBlockNum;
	
	//修改工作目录inode
	addNewSubForCD(newInodeNo, name);

	//分配数据块
	block_no_t blockNo = PROVISION_BLOCK;
	if (size > BLOCK_SIZE) {	//分配间接块
		while (blockNo < TOTAL_BLOCK_COUNT && dev.superblock.block_bitset[blockNo]) ++blockNo;
		newInode->i_ref = blockNo;
		dev.superblock.block_bitset.set(blockNo);
	}
	if (size > 0) {		//分配直接块
		while (blockNo < TOTAL_BLOCK_COUNT && dev.superblock.block_bitset[blockNo]) ++blockNo;
		newInode->d_ref = blockNo;
		dev.superblock.block_bitset.set(blockNo);
		--wantedBlockNum;
	}
	if (size > BLOCK_SIZE) {
		indirect_block_t *indBlock = getIBlock(newInode->i_ref);
		memset(indBlock, 0, sizeof(indirect_block_t));
		for (block_no_t i = 0; i < wantedBlockNum; ++i) {
			while (blockNo < TOTAL_BLOCK_COUNT && dev.superblock.block_bitset[blockNo]) ++blockNo;
			indBlock->block_no[i] = blockNo;
			dev.superblock.block_bitset.set(blockNo);
		}
	}

	return newInodeNo;
}

bool deleteFile(inode_no_t no) {

}

addr_t fsimpl::getFreeSpace() const {
	return getFreeBlock() * BLOCK_SIZE;
}

