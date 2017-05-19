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
	bool setSubCurrentDir(string name);

	file_t getFileByName(string name);
	file_t getFileByInodeNo(inode_no_t no);

	//File opereration on current directory

	//�����ƻ��inode���
	inode_no_t getFile(string name);

	//�ڵ�ǰĿ¼�����ض���С���ļ������ر��
	inode_no_t createFile(string name, addr_t size);

	//�����ǵ�ǰĿ¼�µ��ļ����޸ķ���ʱ��ʱ����
	bool updateFile(string name);

	//�ɹ�����true
	bool deleteFile(inode_no_t no);

	//������Ŀ¼
	inode_no_t createDir(string name);

	//ɾ����Ŀ¼(�Զ�ɾ���������ļ�)
	bool deleteDir(string name);

	addr_t getFreeSpace() const;

	files_t listSub();
};
