#pragma once
#include "misc.h"

typedef vector<file_t> filestack_t;

class fsimpl {
protected:
	block_dev dev;
	path_t currentDirPath;
	file_t currentDirFile;
	file_t rootFile;

	block_no_t getFreeBlock() const;
	inode_t* getInode(inode_no_t no);
	bool getFileStackByPath(filestack_t &files, path_t path);

	//return 0 if no avaliable inode
	inode_no_t allocInode();
	void createRootFile();
public:
	fsimpl();

	path_t getCurrentDir() const;
	bool setCurrentDir(path_t path);

	//File opereration on current directory
	bool getFile(file_t &file, string name);
	bool createFile(file_t &file, string name, addr_t size);
	bool deleteFile(file_t &target);

	addr_t getFreeSpace() const;

};