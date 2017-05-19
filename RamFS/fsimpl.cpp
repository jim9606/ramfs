#include "fsimpl.h"
#include "utils.h"
#include <time.h>
#include <algorithm>

using std::min;

block_no_t fsimpl::getUsedBlock() const {
	return TOTAL_BLOCK_COUNT - getFreeBlock();
}
block_no_t fsimpl::getFreeBlock() const {
	return (block_no_t)(dev.superblock.block_bitset.size() - dev.superblock.block_bitset.count());
}

inode_t* fsimpl::getInode(inode_no_t no) {
	return &(dev.inode_tuple[no / INODE_PER_TUPLE].rec[no % INODE_PER_TUPLE]);
}

indirect_block_t* fsimpl::getIBlock(block_no_t no) {
	return (indirect_block_t*) &(dev.data_block[no - PROVISION_BLOCK]);
}

bool fsimpl::getFileStackByPath(files_t &files, path_t path){
	file_t tempFile = file_t(&dev, rootInode, "");
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
	if (dev.superblock.inode_bitset.count() == INODE_COUNT) {
		logerr("fsimpl.allocInode", "No more Inode");
		return 0;
	}
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
		//��Ҫ�����ݿ�
		block_no_t newBlockNo = allocBlock(PROVISION_BLOCK);
		if (CDrecCount == 0) 
			CDInode->d_ref = newBlockNo;
		else
			CDInode->i_ref = newBlockNo;
		DBlock = (dir_block_t*)getIBlock(newBlockNo);
		DBlock->init();
	}
	else {
		assert(CDrecCount <= 2 * INODE_REC_PER_DIRBLOCK);
		if (CDrecCount < INODE_REC_PER_DIRBLOCK)
			DBlock = (dir_block_t*)getIBlock(CDInode->d_ref);	//�ڵ�һ���ݿ�׷��
		else
			DBlock = (dir_block_t*)getIBlock(CDInode->i_ref);	//�ڵڶ����ݿ�׷��
	}
	DBlock->rec[recOrd].inode_no = sub;
	strcpy(DBlock->rec[recOrd].name, subName.c_str());
	++CDrecCount;
	currentDirFile.init(&dev, CDInode, currentDirFile.getName());
}

void fsimpl::regenerateCDFile(file_t &f) {
	auto &subList = f.sub_inode_rec;
	f.inode->rec_count = subList.size();
	if (subList.size() == 0) {
		dev.superblock.block_bitset.set(f.inode->d_ref, false);
		memset(getIBlock(f.inode->d_ref), TRIM, BLOCK_SIZE);
		return;
	}
	if (subList.size() < INODE_REC_PER_DIRBLOCK) {
		dev.superblock.block_bitset.set(f.inode->i_ref, false);
		memset(getIBlock(f.inode->i_ref), TRIM, BLOCK_SIZE);
	}
	size_t n = min(subList.size(), INODE_REC_PER_DIRBLOCK);
	dir_block_t *DBlock = (dir_block_t*)getIBlock(f.inode->d_ref);
	for (size_t i = 0; i < n; ++i) {
		DBlock->rec[i] = subList.at(i);
	}
	if (subList.size() < INODE_REC_PER_DIRBLOCK)
		return;
	DBlock = (dir_block_t*)getIBlock(f.inode->i_ref);
	n = subList.size() - INODE_REC_PER_DIRBLOCK;
	for (size_t i = 0; i < n; ++i) {
		DBlock->rec[i] = subList.at(i + INODE_REC_PER_DIRBLOCK);
	}
}

bool fsimpl::deleteInode(inode_no_t no) {
	inode_t *inode = getInode(no);
	inode->flags &= ~inode_t::f_valid;
	dev.superblock.inode_bitset.set(no, false);
	if (inode->rec_count == 0) return true;
	if (inode->flags & inode_t::f_file) {	//ɾ���ļ�
		dev.superblock.block_bitset.set(inode->d_ref, false);
		memset(getIBlock(inode->d_ref), TRIM, BLOCK_SIZE);
		if (inode->rec_count == 1) return true;
		indirect_block_t *IBlock = getIBlock(inode->i_ref);
		for (size_t i = 0; i + 1 < inode->rec_count; ++i) {
			dev.superblock.block_bitset.set(IBlock->block_no[i], false);
			memset(getIBlock(inode->d_ref), TRIM, BLOCK_SIZE);
		}
	}
	else if (inode->flags & inode_t::f_dir) {	//ɾ��Ŀ¼
		//�����ڶ����ݿ�
		if (inode->rec_count > INODE_REC_PER_DIRBLOCK) {
			dir_block_t *DBlock = (dir_block_t*)getIBlock(inode->i_ref);
			auto n = inode->rec_count - INODE_REC_PER_DIRBLOCK;
			for (size_t i = 0; i < n; ++i)
				deleteInode(DBlock->rec[i].inode_no);
			inode->rec_count = INODE_REC_PER_DIRBLOCK;
		}
		//������һ���ݿ�
		dir_block_t *DBlock = (dir_block_t*)getIBlock(inode->d_ref);
		for (size_t i = 0; i < inode->rec_count; ++i)
			deleteInode(DBlock->rec[i].inode_no);
	}
	return true;
}

fsimpl::fsimpl() {
	//Create rootFile
	createRootFile();
	currentDirFileStack.clear();
	currentDirFileStack.push_back(0);
}

int fsimpl::checkFileName(string name) {
	if (name.size() > NAME_SIZE) return 8;	//ERR:�ļ�������
	if (name.find('/') != std::string::npos) return 1; //ERR:�����Ƿ��ַ�
	return 0;
}

path_t fsimpl::getCurrentDir() const {
	return currentDirPath;
}

bool fsimpl::setCurrentDir(path_t path) {
	//ƴ�����·��
	if (path.size() > 0 && path.front()=="//") {
		path_t rootPath = getCurrentDir();
		path = rootPath.append(path);
	}
	files_t npwd;
	bool b = getFileStackByPath(npwd, path);
	if (b) {
		if (!(getInode(npwd.back())->flags & inode_t::f_dir)) return false;//It's a file
		currentDirFileStack = npwd;
		currentDirPath = path;
		if (path.size() == 0)
			currentDirFile = file_t(&dev, rootInode, "");
		else {
			currentDirFile = file_t(&dev, getInode(npwd.back()), path.back());
		}
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

file_t fsimpl::getAnonFile(inode_no_t no) {
	return file_t(&dev, getInode(no), "");
}

int fsimpl::createFile(inode_no_t& inode_no, string name, addr_t size) {
	{
		int err = checkFileName(name);
		if (err != 0) return err;
	}
	for (auto const &it : currentDirFile.sub_inode_rec) {
		if (it.name == name)
			return 2;	//ERR:����
	}

	//������Ҫ�Ŀ���Ŀ
	inode_no_t wantedBlockNum = size / BLOCK_SIZE;
	if (size % BLOCK_SIZE > 0) ++wantedBlockNum;
	//ERR:����������ļ���Ŀ����
	if (currentDirFile.inode->rec_count == 2 * INODE_REC_PER_DIRBLOCK) return 3;
	//��Ҫ��ӿ�
	if (size > BLOCK_SIZE) ++wantedBlockNum;
	//ERR:�������ļ�����
	if (wantedBlockNum > BLOCKNO_PER_IBLOCK + 1) return 4;

	//�жϹ���Ŀ¼�Ƿ���Ҫ����
	if (currentDirFile.inode->rec_count % INODE_REC_PER_DIRBLOCK == 0) ++wantedBlockNum;
	//ERR:�ռ䲻��
	if (wantedBlockNum > getFreeBlock()) return 5;
	//��ȥ����Ŀ¼�����
	if (currentDirFile.inode->rec_count % INODE_REC_PER_DIRBLOCK == 0) --wantedBlockNum;
	//��ȥ��ӿ�
	if (size > BLOCK_SIZE) --wantedBlockNum;

	inode_no_t newInodeNo = allocInode();
	//ERR:INODE�ľ�
	if (newInodeNo == 0) return 6;
	inode_t *newInode = getInode(newInodeNo);
	newInode->init();
	newInode->flags = inode_t::f_valid | inode_t::f_file;
	newInode->size = size;
	newInode->rec_count = wantedBlockNum;
	
	//�޸Ĺ���Ŀ¼inode
	addNewSubForCD(newInodeNo, name);

	//�������ݿ�
	block_no_t blockNo = PROVISION_BLOCK;
	if (size > BLOCK_SIZE) {	//�����ӿ�
		blockNo = allocBlock(blockNo);
		newInode->i_ref = blockNo;
	}
	if (size > 0) {		//����ֱ�ӿ�
		blockNo = allocBlock(blockNo);
		newInode->d_ref = blockNo;
		--wantedBlockNum;
	}
	if (size > BLOCK_SIZE) {
		indirect_block_t *indBlock = getIBlock(newInode->i_ref);
		memset(indBlock, 0, sizeof(indirect_block_t));
		for (block_no_t i = 0; i < wantedBlockNum; ++i) {
			blockNo = allocBlock(blockNo);
			indBlock->block_no[i] = blockNo;
		}
	}
	inode_no = newInodeNo;
	return 0;
}

bool fsimpl::deleteFile(string name) {
	auto f = getFileByName(name);
	if (!f.isValid()) return false;	//ERR:û�ҵ��ļ�

	size_t recOrd = 0;
	auto &subList = currentDirFile.sub_inode_rec;
	while (subList.at(recOrd).name != name) ++recOrd;
	auto rec = subList.at(recOrd);
	deleteInode(rec.inode_no);
	subList.erase(subList.begin() + recOrd);
	regenerateCDFile(currentDirFile);
	return true;
}

addr_t fsimpl::getFreeSpace() const {
	return getFreeBlock() * BLOCK_SIZE;
}

bool fsimpl::updateFile(string name) {
	auto f = getFileByName(name);
	if (!f.isValid()) return false;	//ERR:û�ҵ��ļ�

	f.inode->atime = time(0);
	return updateFile();
}

bool fsimpl::updateFile() {
	for (auto &it : currentDirFileStack) {
		getInode(it)->atime = time(0);
	}
	return true;
}

inode_no_t fsimpl::createDir(string name) {
	inode_no_t inodeNo = allocInode();
	if (inodeNo == 0) return inodeNo;	//ERR:Inode�ľ�
	inode_t *newInode = getInode(inodeNo);
	newInode->init();
	newInode->flags = inode_t::f_valid | inode_t::f_dir;

	//�޸Ĺ���Ŀ¼inode
	addNewSubForCD(inodeNo, name);

	return inodeNo;
}

vector<subfile_info> fsimpl::listSub() {
	vector<subfile_info> res;
	for (auto &it : currentDirFile.sub_inode_rec) {
		subfile_info info;
		auto inode = getInode(it.inode_no);
		info.name = it.name;
		info.isFile = inode->flags & inode_t::f_file;
		info.ctime = inode->ctime;
		info.atime = inode->atime;
		info.size = inode->size;
		info.inode_no = it.inode_no;
		res.push_back(info);
	}
	return res;
}

string fsimpl::getErrMsg(int err) {
	switch (err) {
	case 0:
		return "Success";
	case 1:
		return "Invalid name";
	case 2:
		return "Duplicated name";
	case 3:
		return "Too many subfiles in current directory";
	case 4:
		return "Exceed single file size";
	case 5:
		return "Insufficient space";
	case 6:
		return "No more I-nodes";
	case 7:
		return "File not found";
	case 8:
		return "File name is too long";
	default:
		return "Unknown error";
	}
}