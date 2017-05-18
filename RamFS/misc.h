#pragma once
//This file contains transient objects
#include "fs_struct.h"
#include <vector>
#include <string>
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
文件对象只能有fs创建
写入前需要预先分配空间
时间戳由fs维护
*/
class file_t {
protected:
	char *base;
	inode_t *inode;
	vector<data_block_t*> data_addr;
	vector<sub_inode_rec_t> sub_inode_rec;
public:
	file_t(char *base, inode_t *inode);
	bool init(char *base, inode_t *inode);
	size_t size() const;
	bool isDir() const;
	bool isFile() const;

	//valid for file
	size_t read(char* buffer, const addr_t offset, const addr_t length) const;
	size_t write(char* buffer, const addr_t offset, const addr_t length);

	//valid for directory
	const vector<sub_inode_rec_t> & getSubNode();

};