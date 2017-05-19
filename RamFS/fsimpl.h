#pragma once
#include "misc.h"
using std::pair;
using std::make_pair;

typedef std::vector<inode_no_t> files_t;

class fsimpl {
protected:
	block_dev dev;
	path_t currentDirPath;
	files_t currentDirFileStack;
	file_t currentDirFile;
	inode_t *rootInode;

	inode_t* getInode(inode_no_t no);
	indirect_block_t* getIBlock(block_no_t no);
	bool getFileStackByPath(files_t &files, path_t path);

	//return 0 if no avaliable inode
	inode_no_t allocInode();
	//从参数编号开始寻找可用块
	block_no_t allocBlock(block_no_t no);
	void createRootFile();

	//修改工作目录inode以增加新子文件（目录）,需要预先检查空间是否充足
	void addNewSubForCD(inode_no_t sub, string subName);

	//由file_t的sub_inode_rec持久化至设备,只在删除文件时对工作目录调用
	void regenerateCDFile(file_t &f);

	//删除目录树
	bool deleteInode(inode_no_t no);
public:
	block_no_t getUsedBlock() const;
	block_no_t getFreeBlock() const;
	fsimpl();

	//检查文件名
	static int checkFileName(string name);

	path_t getCurrentDir() const;
	bool setCurrentDir(path_t path);

	//File opereration on current directory

	//由名称获得inode编号
	inode_no_t getFile(string name) const;
	file_t getFileByName(string name);

	//在当前目录创建特定大小的文件，返回编号
	inode_no_t createFile(string name, addr_t size);

	//必须是当前目录下的文件，修改访问时间时调用
	bool updateFile(string name);

	//成功返回true,自动删除子文件
	bool deleteFile(string name);

	//创建子目录
	inode_no_t createDir(string name);

	addr_t getFreeSpace() const;

	vector<sub_inode_rec_t> listSub();
};
