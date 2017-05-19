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
	//����·������inode��ű�
	bool getFileStackByPath(files_t &files, path_t path);

	//return 0 if no avaliable inode
	inode_no_t allocInode();
	//�Ӳ�����ſ�ʼѰ�ҿ��ÿ�
	block_no_t allocBlock(block_no_t no);
	void createRootFile();

	//�޸Ĺ���Ŀ¼inode�����������ļ���Ŀ¼��,��ҪԤ�ȼ��ռ��Ƿ����
	void addNewSubForCD(inode_no_t sub, string subName);

	//��file_t��sub_inode_rec�־û����豸,ֻ��ɾ���ļ�ʱ�Թ���Ŀ¼����
	void regenerateCDFile(file_t &f);

	//ɾ��Ŀ¼��
	bool deleteInode(inode_no_t no);
public:
	block_no_t getUsedBlock() const;
	block_no_t getFreeBlock() const;
	fsimpl();

	//����ļ���
	static int checkFileName(string name);

	path_t getCurrentDir() const;
	bool setCurrentDir(path_t path);

	//File opereration on current directory

	//�����ƻ��inode���
	inode_no_t getFile(string name) const;
	file_t getFileByName(string name);

	//��Inode��Ż�������ļ�
	file_t getAnonFile(inode_no_t no);

	//�ڵ�ǰĿ¼�����ض���С���ļ������ر��,��getErrMsg����
	int createFile(inode_no_t& inode_no, string name, addr_t size);

	//�����ǵ�ǰĿ¼�µ��ļ����޸ķ���ʱ��ʱ����
	bool updateFile(string name);
	//���¹���Ŀ¼����ʱ��
	bool updateFile();

	//�ɹ�����true,�Զ�ɾ�����ļ�
	bool deleteFile(string name);

	//������Ŀ¼
	inode_no_t createDir(string name);

	addr_t getFreeSpace() const;

	vector<subfile_info> listSub();

	static string getErrMsg(int err);
};
