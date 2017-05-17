#pragma once
//This file contains transient objects
#include"block_fs.h"
#include<vector>
#include<string>
using std::vector;
using std::string;

class path_t {
protected:
	vector<string> n;
public:
	size_t size() const;
	string get(const size_t c) const;
	string format() const;
	void push(const string &s);
	void pop();
};

/*
�ļ�����ֻ����fs����
д��ǰ��ҪԤ�ȷ���ռ�
ʱ�����fsά��
*/
class file_t {
protected:
	char *base;
	inode_t *inode;
	vector<data_block_t*> data_addr;
	vector<sub_inode_rec_t> sub_inode_rec;
public:
	bool init(char *base, inode_t *inode);
	size_t size() const;
	bool isDir() const;
	bool isFile() const;

	//valid if isFile() is true
	size_t read(char* buffer, const addr_t offset, const addr_t length) const;
	size_t write(char* buffer, const addr_t offset, const addr_t length);


};