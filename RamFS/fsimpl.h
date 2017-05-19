#pragma once
#include "misc.h"

typedef vector<file_t> files_t;

class fsimpl {
protected:
	block_dev dev;
	path_t currentDirPath;
	files_t currentDirFileStack;
	inode_t *rootInode;

	block_no_t getFreeBlock() const;
	inode_t* getInode(inode_no_t no);
	bool getFileStackByPath(vector<inode_no_t> &files, path_t path);

	//return 0 if no avaliable inode
	inode_no_t allocInode();
	void createRootFile();
public:
	fsimpl();

	path_t getCurrentDir() const;
	bool setCurrentDir(path_t path);

	//File opereration on current directory
	//file_t对象用于复制或创建文件
	inode_no_t getFile(string name);
	inode_no_t createFile(string name, addr_t size);
	bool deleteFile(inode_no_t no);

	addr_t getFreeSpace() const;
	files_t listSub();
};
