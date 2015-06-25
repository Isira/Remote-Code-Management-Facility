//////////////////////////////////////////////////////////////////////////
// DataStore.cpp  Data Structure to save a file using non-duplicated path	//
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

#include "DataStore.h"
#include <vector>

#include <iostream>
#include "../FileManager/FileSystem.h"

//----< save the path and file name preserving uniqueness 

void DataStore::save(const std::string& path, const std::string& filename)
{
	auto ret = Pathset.insert(path);
	auto iterator = ret.first;

	_FileMap[filename].push_back(iterator);
	_fileSize++;
}



#ifdef TEST_DATASTORE
void DisplayDataStore(DataStore &store)
{
	for (auto it : store)
	{
		DataStore::ListofRefs refs = it.second;
		if (refs.size() > 1)
		{
			std::string fileName = it.first;
			for (auto ref : refs)
			{
			std::string path = *ref;
			std::string fullPath = FileSystem::Path::fileSpec(path, fileName);
			std::cout << fullPath << std::endl;
			}
		}
	}
}
int main(int argc, char** argv)
{
	using namespace std;
	DataStore store;
	store.save("path1","file1");
	store.save("path1", "file2");
	store.save("path2", "file1");
	store.save("path2", "file2");
	
	DisplayDataStore(store);
	cout << "Number of paths: " << store.pathSize() << endl;
	cout << "Number of files: " << store.fileSize() << endl;

}

#endif






