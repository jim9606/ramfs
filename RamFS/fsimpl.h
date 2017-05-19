#pragma once
#include "misc.h"

typedef std::vector<inode_no_t> files_t;

class fsimpl {
protected:
	block_dev dev;
	path_t currentDirPath;
	files_t currentDirFileStack;
	file_t currentDirFile;
	inode_t *rootInode;

	inode_t* getInode(inode_no_t no);
	bool getFileStackByPath(files_t &files, path_t path);

	//return 0 if no avaliable inode
	inode_no_t allocInode();
	void createRootFile();
public:
	block_no_t getFreeBlock() const;
	fsimpl();

	path_t getCurrentDir() const;
	bool setCurrentDir(path_t path);

	//File opereration on current directory

	//�����ƻ��inode���
	inode_no_t getFile(string name) const;
	file_t getFileByName(string name);

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

	vector<sub_inode_rec_t> listSub();
};
