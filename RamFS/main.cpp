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
	regex filePattern("(/?[\\w-]+)+(\\.\\w+)?");
	regex dirPattern("(/?[\\w-]*)+");
	string order;
	string pathString;
	cout << fs.getCurrentDir().format() + "$";
	while (cin >> order)
	{
		if (order == "cf") {
			cin >> pathString;
			if (regex_match(pathString, filePattern)) {
				int fileSize = 0;
				cin >> fileSize;
				path_t file(pathString);
				if (fs.setCurrentDir(file.parent())) {
					auto r = fs.createFile(file.back(), fileSize);
					if (r == 0)
						cout << "Failed to create the file." << endl;
					else {
						genfile(fs.getAnonFile(r));
					}
				}
				else
				{
					cout << "Dir \"" + file.parent().format() + "\" is not a valid path, please input again." << endl;
				}
			}
			else
			{
				cout << "\"" + pathString + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "rf") {
			cin >> pathString;
			if (regex_match(pathString, filePattern)) {
				path_t file(pathString);
				if (fs.setCurrentDir(file.parent())) {
					inode_no_t fileNumber = fs.getFile(file.back());
					if (fileNumber) {
						fs.deleteFile(file.back());
					}
					else {
						cout << "\"" + pathString + "\" not found." << endl;
					}
				}
				else
				{
					cout << "Dir \"" + file.parent().format() + "\" is not a valid path, please input again." << endl;
				}
			}
			else
			{
				cout << "\"" + pathString + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "md") {
			cin >> pathString;
			if (regex_match(pathString, dirPattern)) {
				path_t dir(pathString);
				if (fs.setCurrentDir(dir.parent())) {
					inode_no_t dirNumber = fs.getFile(dir.back());
					if (dirNumber) {
						cout << "\"" + pathString + "\" exists." << endl;
					}
					else {
						fs.createDir(dir.back());
					}
				}
				else
				{
					cout << "Dir \"" + dir.parent().format() + "\" is not a valid path, please input again." << endl;
				}
			}
			else
			{
				cout << "\"" + pathString + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "rd") {
			cin >> pathString;
			if (regex_match(pathString, dirPattern)) {
				path_t dir(pathString);
				if (fs.setCurrentDir(dir.parent())) {
					inode_no_t dirNumber = fs.getFile(dir.back());
					if (dirNumber) {
						fs.deleteFile(dir.back());
					}
					else {
						cout << "\"" + pathString + "\" not found." << endl;
					}
				}
				else
				{
					cout << "Dir \"" + dir.parent().format() + "\" is not a valid path, please input again." << endl;
				}
			}
			else
			{
				cout << "\"" + pathString + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "cd")
		{
			cin >> pathString;
			if (regex_match(pathString, dirPattern)) {
				path_t dir(pathString);
				if (!fs.setCurrentDir(dir)) {
					cout << "\"" + pathString + "\" not found." << endl;
				}
			}
			else
			{
				cout << "\"" + pathString + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "ls") {
			auto subFiles = fs.listSub();
			for (auto &subInfo : subFiles) {
				cout << endl << "Name: " << subInfo.name << endl;
				cout << "Type: ";
				if (subInfo.isFile) {
					cout << "File" << endl;
					cout << "Size: " << subInfo.size << endl;
				}
				else {
					cout << "Directory" << endl;
				}
				cout << "Created: " << ctime(&subInfo.ctime) << "Accessed: " << ctime(&subInfo.atime);
			}
		}
		else if (order == "cp") {
			string direction;
			cin >> pathString;
			if (regex_match(pathString, filePattern)) {
				cin >> direction;
				if (regex_match(direction, filePattern)) {
					path_t nowDir = fs.getCurrentDir();
					path_t fromFilePath(pathString);
					path_t toFilePath(direction);
					if (fs.setCurrentDir(fromFilePath.parent())) {
						auto fromFileNumber = fs.getFile(fromFilePath.back());
						if (fromFileNumber) {
							auto fromFile = fs.getFileByName(fromFilePath.back());
							if (fs.setCurrentDir(toFilePath.parent())) {
								auto toFileNumber = fs.getFile(toFilePath.back());
								if (toFileNumber) {
									cout << "\"" + direction + "\" has exists." << endl;
								}
								else
								{
									auto r = fs.createFile(toFilePath.back(), fromFile.size());
									if (r == 0)
										cout << "Failed to create the file." << endl;
									else {
										genfile(fs.getAnonFile(r));
										auto toFile = fs.getFileByName(toFilePath.back());
										copyfile(toFile, fromFile);
										fs.setCurrentDir(nowDir);
									}
								}
							}
							else {
								cout << "Dir \"" + toFilePath.parent().format() + "\" is not a valid path, please input again." << endl;
							}
						}
						else
						{
							cout << "\"" + pathString + "\" not found." << endl;
						}
					}
					else
					{
						cout << "Dir \"" + fromFilePath.parent().format() + "\" is not a valid path, please input again." << endl;
					}
				}
				else
				{
					cout << "\"" + direction + "\" is not a valid path, please input again." << endl;
				}
			}
			else
			{
				cout << "\"" + pathString + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "sum") {
			cout << fs.getFreeSpace() << " space remain" << endl;
			cout << fs.getFreeBlock() << " blocks remain" << endl;
			cout << fs.getUsedBlock() << " blocks used" << endl;
		}
		else if (order == "cat") {
			cin >> pathString;
			if (regex_match(pathString, filePattern)) {
				path_t filePath(pathString);
				if (fs.setCurrentDir(filePath.parent())) {
					inode_no_t fileNumber = fs.getFile(filePath.back());
					if (fileNumber) {
						auto file = fs.getFileByName(filePath.back());
						if (file.isFile()) {
							size_t size = file.size();
							char buffer[BLOCK_SIZE + 1];
							buffer[BLOCK_SIZE] = '\0';
							for (auto it : file.data_addr) {
								memcpy(buffer, it->b, min(size, BLOCK_SIZE));
								buffer[min(size, BLOCK_SIZE)] = '\0';
								cout << buffer;
								size -= BLOCK_SIZE;
							}
							cout << endl;
						}
						else {
							cout << "\"" + pathString + "\" is not a file." << endl;
						}
					}
					else {
						cout << "\"" + pathString + "\" not found." << endl;
					}
				}
				else
				{
					cout << "Dir \"" + filePath.parent().format() + "\" is not a valid path, please input again." << endl;
				}
			}
			else
			{
				cout << "\"" + pathString + "\" is not a valid path, please input again." << endl;
			}
		}
		else if (order == "quit") {
			return 0;
		}
		else
		{
			cout << "\"" + order + "\" is not a valid order, please input again." << endl;
		}
		cout << fs.getCurrentDir().format() + "$";
	}
	return 0;
}
