#ifndef CATALOGER_H
#define CATALOGER_H

//////////////////////////////////////////////////////////////////////////
// Cataloger.h - Do operations on the Datastore.						//
//			     i.e: FindDuplicates, Get files with search text		//
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
* This module provides class Cataloger.
* Cataloger class manages operations on a DataStore Object.
* Cataloger can find duplicate file names exisitng on a datastore, 
* search text in files which are stored in the datastore, 
* matching at least one in a set of patterns

* Public Interface:
* =================
* DataStore store;
* Cataloger cataloger(store);
* cataloger.DuplicateFiles();
* cataloger.FilesWithSearchText("searchText");
* cataloger.FilesWithSearchText("searchText", {"*.cpp","*.h"});
*
* Required Files:
* ===============
* DataStore.h DataStore.cpp Cataloger.h Cataloger.cpp DataStore.h DataStore.cpp Display.h Display.cpp 
						FileSystem.h FileSystem.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_CATALOGER Cataloger.cpp DataStore.cpp Display.cpp FileSystem.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 09 Feb 15
* - first release
*/

#include "../DataStore/DataStore.h"
#include <vector>

	class Cataloger
	{
		DataStore& _store;
		
		bool matchesOneOfThePatterns(const std::string& fileName, std::vector<std::string> patterns);
		bool isPatternMaching(const std::string& fileName,const std::string& pattern);
		std::string replaceWithWildcardRegex(std::string pattern);
		bool matchWithRegex(const std::string& filename, const std::string& pattern);
	public:
		Cataloger(DataStore& store);
		static std::map<std::string, std::vector<std::string>>  SearchTextInaFile(const std::string& file, std::vector<std::string> texts);
		void DuplicateFiles();
		void FilesWithSearchText(const std::string& searchText, std::vector<std::string> patterns = {"*.*"});
		std::string  FilesWithSearchTextXml(std::vector<std::string> searchText, std::vector<std::string> patterns = { "*.*" });
		std::string getXmlRepresentation();
		//std::map<std::string, std::vector<std::string>> parallelSearch(std::vector<std::string> files, std::vector<std::string> searchStrings);
		std::map<std::string, std::list<std::string>> directoryMapRelativeToRoot(const std::string& root, std::vector<std::string> patterns = {"*.*"});
		std::string directoryMapRelativeToRootXml(const std::string& root);
	};


#endif