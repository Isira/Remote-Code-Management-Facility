#ifndef FILEMANAGER_H
#define FILEMANAGER_H

//////////////////////////////////////////////////////////////////////////
// FileManager.h -	Find files given directory path, patterns and to	//
//					recurse or not,and save each file in to a Datastore.//
// ver 1.0																//
// ---------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015									//
// All rights granted provided that this notice is retained				//
// ---------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013                 //
// Platform:    Mac Book Pro, Core i5, Windows 8.1						//
// Application: Project #1 – File Catalogue,2015						//
// Author:      Isira Samarasekera, Syracuse University					//
//              issamara@syr.edu										//
//////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
* This module provides class FileManager.
* FileManager, search files in a path matching one or many file name patterns.
* If recursive is set true, then it will will recurse in to the subdirectories in order to find the files
* All the found file paths are saved in the datastore object.
*
* Public Interface:
* =================
*	DataStore store;
*	FileManager fileManager(path,patterns,recursive,store);
*	fileManager.search();
*
* Required Files:
* ===============
*	DataStore.h DataStore.cpp FileManager.h FileManager.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_FILEMANAGER FileManager.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 09 Feb 15
* - first release
*/
#include <vector>
#include "../DataStore/DataStore.h"
#include "FileSystem.h"
class FileManager
{

public:
	FileManager(const std::string path, std::vector<std::string> patterns, bool recursive = true);
	template<typename CallObj>
	void search(CallObj& co);
	template<typename FileFound, typename EnteringDirectory, typename LeavingDirectory>
	void findFiles(std::string directory, FileFound& fileFound, EnteringDirectory& dirFound, LeavingDirectory& leave);


private:
	template<typename CallObj>
	void findFiles(std::string directory, CallObj& co);

	std::string path_;
	std::vector<std::string> _patterns;
	std::vector<std::string> _files;
	bool _recursive;

};

//----< find the files and display a brief summary >--------------------
template<typename CallObj>
void FileManager::search(CallObj& co)
{
	findFiles(path_, co);
}

//----< find the files for a directory and save in the datastore >--------------------
template<typename CallObj>
void FileManager::findFiles(std::string directory, CallObj& co)
{
	for (auto it = _patterns.begin(); it < _patterns.end(); it++){
		std::string pattern = *it;
		std::vector<std::string> files = FileSystem::Directory::getFiles(directory, pattern);

		for (auto i = files.begin(); i < files.end(); i++)
		{
			std::string fileName = *i;
			co(directory, fileName);
		}
	}
	if (_recursive)
	{
		std::vector<std::string> dirs = FileSystem::Directory::getDirectories(directory);
		for (auto it = dirs.begin(); it < dirs.end(); it++)
		{
			std::string relativeDirPath = *it;
			if (relativeDirPath.compare(".") && relativeDirPath.compare(".."))
			{
				std::string absoluteDirPath = FileSystem::Path::fileSpec(directory, relativeDirPath);
				findFiles(absoluteDirPath, co);
			}

		}
	}
}

template<typename FileFound,typename EnteringDirectory,typename LeavingDirectory>
void FileManager::findFiles(std::string directory, FileFound& fileFound, EnteringDirectory& enterDir, LeavingDirectory& leaveDir)
{
	
	for (auto it = _patterns.begin(); it < _patterns.end(); it++){
		std::string pattern = *it;
		std::vector<std::string> files = FileSystem::Directory::getFiles(directory, pattern);

		for (auto i = files.begin(); i < files.end(); i++)
		{
			std::string fileName = *i;
			fileFound( fileName);
		}
	}
	if (_recursive)
	{
		std::vector<std::string> dirs = FileSystem::Directory::getDirectories(directory);
		for (auto it = dirs.begin(); it < dirs.end(); it++)
		{
			std::string relativeDirPath = *it;
			if (relativeDirPath.compare(".") && relativeDirPath.compare(".."))
			{
				std::string absoluteDirPath = FileSystem::Path::fileSpec(directory, relativeDirPath);
				enterDir(relativeDirPath);
				findFiles(absoluteDirPath, fileFound, enterDir, leaveDir);
				leaveDir(relativeDirPath);
			}

		}
	}
} 


#endif