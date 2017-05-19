#pragma once
#include "misc.h"

typedef std::vector<inode_no_t> files_t;

class fsimpl {
protected:
	block_dev dev;
	path_t currentDirPath;
	files_t currentDirFileStack;
	inode_t *rootInode;

	block_no_t getFreeBlock() const;
	inode_t* getInode(inode_no_t no);
	bool getFileStackByPath(files_t &files, path_t path);

	//return 0 if no avaliable inode
	inode_no_t allocInode();
	void createRootFile();
public:
	fsimpl();

	path_t getCurrentDir() const;
	bool setCurrentDir(path_t path);

	file_t getFileByName(string name);
	file_t getFileByInodeNo(inode_no_t no);
	//File opereration on current directory

	//由名称获得inode编号
	inode_no_t getFile(string name);

	//在当前目录创建特定大小的文件，返回编号
	inode_no_t createFile(string name, addr_t size);

	//必须是当前目录下的文件，访问时调用
	bool updateFile(string);

	//成功返回true
	bool deleteFile(inode_no_t no);

	addr_t getFreeSpace() const;

	files_t listSub();
};
