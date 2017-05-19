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
	string getCurrentDirString() const;
	bool setCurrentDir(path_t path);
	bool setCurrentDir(const string &s);

	file_t getFileByName(string name);
	file_t getFileByInodeNo(inode_no_t no);
	//File opereration on current directory
	inode_no_t getFile(string name);
	inode_no_t createFile(string name, addr_t size);
	bool deleteFile(inode_no_t file);

	addr_t getFreeSpace() const;

};