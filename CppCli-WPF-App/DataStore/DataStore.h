#ifndef DATASTORE_H
#define DATASTORE_H

//////////////////////////////////////////////////////////////////////////
// DataStore.h  Data Structure to save a file using non-duplicated path	//
//			    and file name											//
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
* This module provides class DataStore.
* Datastore class acts as a container,   storing a file set, saving each file name only once
* and saving each path only once, while preserving all of the containment relationships,
* between directories and their files.

* Public Interface:
* =================
* DataStore store;
* store.save("/path/to/dir","fileName");
* bool empty = store.empty();
* int pathSize = store.pathSize();
* int fileSize = store.fileSize();
* for(auto it: store){
	 DataStore::File file = it.first;
	 DataStore::ListofRefs paths = it.second;
  }
*
* Required Files:
* ===============
* DataStore.h DataStore.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_DATASTORE DataStore.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 09 Feb 15
* - first release
*/

#include <vector>
#include <map>
#include <set>
#include <list>


class DataStore
{
public:
	using File = std::string;
	using Ref = std::set<std::string>::iterator;
	using Path = std::string;
	using ListofRefs = std::list<Ref>;
	using iterator = std::map<File, ListofRefs>::iterator; // use this iterator to go through the go through the store

	void save(const std::string& path, const std::string& filename);
	 
	
	inline iterator DataStore::begin()
	{
		return _FileMap.begin();
	}

	inline iterator DataStore::end()
	{
		return _FileMap.end();
	}

	//----< returns whether store contains no files>-------------------------------------------

	inline bool empty()
	{
		return _FileMap.empty();
	}
	
	//----< returns the number of distinct paths in the store>-------------------------------------------

	inline int pathSize()
	{
		return Pathset.size();
	}

	//----< returns the number of files stored >-------------------------------------------

	inline int fileSize()
	{
		return _fileSize;
	}

private:
	std::map<File, ListofRefs> _FileMap;
	std::set<Path> Pathset;
	std::vector<std::string> getAbsolutePathsOfAllFilesWithSameName(std::string name);
	int _fileSize;

};



#endif

