#include <iostream>
#include <string>
#include <regex>
#include "fs_struct.h"
#include "fsimpl.h"
#include "utils.h"
using namespace std;

void PrintStructSize() {
	cout << sizeof(inode_t) << endl;
	cout << sizeof(sub_inode_rec_t) << endl;
	cout << sizeof(dir_block_t) << endl;
	cout << sizeof(indirect_block_t) << endl;
	cout << sizeof(data_block_t) << endl;
	cout << sizeof(inode_tuple_t) << endl;
	cout << sizeof(superblock_t) << endl;
	cout << sizeof(block_dev) << endl;
}

fsimpl fs;

int main(int argc, char **argv) {
	//PrintStructSize();
	//cout << fs.getFreeSpace() << endl;
	//printmem(&fs, 64);
	regex filePattern("(/?[\\w-]+)+(\\.\\w+)?");
	regex dirPattern("(/?[\\w-]+)+");
	string order;
	string path;
	while (cin>>order)
	{
		cout << fs.getCurrentDir().format() + "$";
		if (order == "createFile") {
			cin >> path;
			if (regex_match(path, filePattern)) {
				int fileSize = 0;
				cin >> fileSize;
				fs.setCurrentDir(path_t(path));
				genfile()
			}
			else
			{
				cout << "\"" + path + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "deleteFile") {
			cin >> path;
			if (regex_match(path, filePattern)) {
				//TODO
			}
			else
			{
				cout << "\"" + path + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "createDir") {
			cin >> path;
			if (regex_match(path, dirPattern)) {
				//TODO
			}
			else
			{
				cout << "\"" + path + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "deleteDir") {
			cin >> path;
			if (regex_match(path, dirPattern)) {
				//TODO
			}
			else
			{
				cout << "\"" + path + "\" is not a valid path, please input again." << endl;
			}
		}
		else if(order=="changeDir")
		{
			cin >> path;
			if (regex_match(path, dirPattern)) {
				fs.setCurrentDir(path);
			}
			else
			{
				cout << "\"" + path + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "dir") {
			path_t dir= fs.getCurrentDir();
			//TODO
		}
		else if (order == "cp") {
			string direction;
			cin >> path;
			if (regex_match(path, filePattern)) {
				cin >> direction;
				if (regex_match(direction, filePattern)) {
					path_t from(path);
					path_t to(direction);
					
				}
				else
				{
					cout << "\"" + direction + "\" is not a valid path, please input again." << endl;
				}
			}
			else
			{
				cout << "\"" + path + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "sum") {
			//TODO
		}
		else if (order == "cat") {
			//TODO
		}
		else if (order == "quit") {
			//TODO
			return 0;
		}
		else
		{
			cout << "\"" + order + "\" is not a valid order, please input again." << endl;
		}
	}
	return 0;
}
