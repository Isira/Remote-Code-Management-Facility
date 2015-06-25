//////////////////////////////////////////////////////////////////////////
// Cataloger.cpp - Do operations on the Datastore.						//
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

#include "Cataloger.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <regex>
#include <thread>
#include <future>
#include <functional>

#include "../FileManager/FileSystem.h"
#include  "../XmlDocument/XmlDocument.h"
#include "../XmlElement/XmlElementFactory.h"

using namespace XmlProcessing;

Cataloger::Cataloger(DataStore& store) :_store(store)
{
}

//----< Search and Display Duplicate files in the DataStore >----

void Cataloger::DuplicateFiles()
{
	DataStore _duplicateDataStore;

	for (auto it : _store)
	{
		DataStore::ListofRefs refs = it.second;
		if (refs.size() > 1)
		{
			std::string fileName = it.first;
			for (auto ref : refs)
			{
				std::string path = *ref;
				_duplicateDataStore.save(path,fileName);
			}
		}
	}
}

//----< Search and Display files where the search text exist >------------------------------------------

void Cataloger::FilesWithSearchText(const std::string& searchText, std::vector<std::string> patterns)
{
	DataStore searchFileDataStore;
	for (auto it : _store)
	{
		DataStore::File file = it.first;
		if (matchesOneOfThePatterns(file, patterns))
		{
			DataStore::ListofRefs paths = it.second;
			for (auto i : paths)
			{
				std::string path = *i;
				std::string fullPath = FileSystem::Path::fileSpec(path, file);
				std::vector<std::string> searchTexts = { searchText };
				if (!SearchTextInaFile(fullPath, searchTexts).empty())
				{
					searchFileDataStore.save(path,file);
				}
			}
		}
	}

}

//----< search in a file text >-------------------------------

std::pair<std::string,std::string> searchInFileText(const std::string& filePath,const std::string& content, const std::string& searchText)
{
	std::pair<std::string, std::string> pair;
	std::regex string_regex(searchText);
	if (std::regex_search(content, string_regex))
		pair = std::make_pair(searchText, filePath);
	return pair;
}
//----< Check whether a file contains specified  text >-------------------------------

std::map<std::string,std::vector<std::string>> Cataloger::SearchTextInaFile(const std::string& filePath,std::vector<std::string> texts)
{
	std::map<std::string, std::vector<std::string>> map;
	std::ifstream in;
	in.open(filePath);

	if (!in.good())
		return map;
	std::string fileText;
	while (in.good())
	{
		fileText += in.get();
	}
	in.close();

	std::vector<std::future<std::pair<std::string, std::string>>> futures(texts.size());
	for (size_t i = 0; i<texts.size(); ++i)
		futures[i] = std::async(searchInFileText, filePath, fileText, texts[i]);  // asynchronous

	for (size_t i = 0; i<texts.size(); ++i)
	{
		std::pair<std::string, std::string> out = futures[i].get();  // blocking call
		if (!out.first.empty())
			map[out.first].push_back(out.second);
	}

	return map;

}

//----< check whether a fileName matches one of the patterns >-----------------------

bool Cataloger::matchesOneOfThePatterns(const std::string& fileName, std::vector<std::string> patterns)
{
	bool matching = false;
	for (std::string pattern : patterns)
	{
		if (isPatternMaching(fileName, pattern))
		{
			matching = true;
			break;
		}
	}
	return matching;
}

//----< check whether a fileName matches a specific pattern >------------------------------------------

bool Cataloger::isPatternMaching(const std::string& fileName, const std::string& pattern)
{
	std::string regexPattern = replaceWithWildcardRegex(pattern);
	return (std::regex_match(fileName, std::regex(regexPattern)));
}

//----< replace the wildcard character of a given string to match regex string >------------

std::string Cataloger::replaceWithWildcardRegex(std::string pattern)
{
	std::regex wildcardRegex("\\*");  
	std::string wildcardRegexString = "(.*)";
	std::string regexString = std::regex_replace(pattern, wildcardRegex, wildcardRegexString);
	return regexString;
}

//----< check whether a fileName matches a regex pattern 

bool Cataloger::matchWithRegex(const std::string& filename, const std::string& pattern)
{
	std::string regexPattern = replaceWithWildcardRegex(pattern);
	return (std::regex_match(filename, std::regex(regexPattern)));
}

//----< Get Xml Representation of the data Store >------------

std::string Cataloger::getXmlRepresentation()
{
	sPtr pRoot = XmlElementFactory::makeTaggedElement("DataStore");
	std::map<std::string, std::list<std::string>> directoryMap;
	for (auto it : _store)
	{
		DataStore::File file = it.first;
		DataStore::ListofRefs paths = it.second;
		for (auto i : paths)
		{
			std::string path = *i;
			directoryMap[path].push_back(file);
		}
	}
	return pRoot->toString();
}

//----< replace all helper >------------

void replaceAll_(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}


//----< get Xml representation of the search text results >------------

std::string Cataloger::FilesWithSearchTextXml(std::vector<std::string> searchTexts, std::vector<std::string> patterns)
{
	sPtr pRoot = XmlElementFactory::makeTaggedElement("TextSearchResult");

	std::vector<std::string> filePaths;
	for (auto it : _store)
	{
		DataStore::File file = it.first;
		if (matchesOneOfThePatterns(file, patterns))
		{
			DataStore::ListofRefs paths = it.second;
			for (auto i : paths)
			{
				std::string path = *i;
				std::string fullPath = FileSystem::Path::fileSpec(path, file);
				filePaths.push_back(fullPath);
			}
		}
	}

		std::map<std::string, std::vector<std::string>> files /*= parallelSearch(filePaths, searchTexts)*/;
		
		for (auto it : files)
		{
			sPtr pChild1 = XmlElementFactory::makeTaggedElement("SearchResult");
			pChild1->addAttrib("Text", it.first);
			for (auto i:it.second)
			{
				sPtr pChild2 = XmlElementFactory::makeTaggedElement("File");
				pChild2->addAttrib("value", i);
				pChild1->addChild(pChild2);

			}
			pRoot->addChild(pChild1);
		}
	return pRoot->toString();
}

//----< get directory root map >------------

std::string Cataloger::directoryMapRelativeToRootXml(const std::string& root)
{
	sPtr pRoot = XmlElementFactory::makeTaggedElement("Directories");
	std::map<std::string, std::list<std::string>> map = directoryMapRelativeToRoot(root);
	for (auto it : map)
	{
		std::string  path = it.first;
		std::list<std::string> files = it.second;
		sPtr dir = XmlElementFactory::makeTaggedElement("Dir");
		dir->addAttrib("path", path);
		pRoot->addChild(dir);
		for (auto i : files)
		{
			std::string file = i;
			sPtr pChild2 = XmlElementFactory::makeTaggedElement("File");
			pChild2->addAttrib("name", i);
			dir->addChild(pChild2);
		}
	}
	return pRoot->toString();

}

//----< get directory  map relative to root >------------

std::map<std::string, std::list<std::string>> Cataloger::directoryMapRelativeToRoot(const std::string& root, std::vector<std::string> patterns)
{
	std::map<std::string, std::list<std::string>> map;
	std::vector<std::string> filePaths;
	for (auto it : _store)
	{
		DataStore::File file = it.first;
		if (matchesOneOfThePatterns(file, patterns))
		{
			DataStore::ListofRefs paths = it.second;
			for (auto i : paths)
			{
				std::string path = *i;
				//std::string relativePath = FileSystem::Directory::relativePath(root, path);
				//map[relativePath].push_back(file);
			}
		}
	}
	return map;
}

#ifdef TEST_CATALOGER
#include "../FileManager/FileManager.h"
#include <stack>
class DirectoryStack
{
public:
	static  std::stack<std::pair<std::string, std::vector<std::string>>> fileStack;
	static std::stack<std::shared_ptr<XmlProcessing::AbstractXmlElement>> _elementStack;
	static  std::string rootDir;
	
	static void finalise()
	{
		while (!fileStack.empty())
		{
			std::cout << "popped" << fileStack.top().first << "\n";
			fileStack.pop();
			sPtr pDir = _elementStack.top();
			_elementStack.pop();
			_elementStack.top()->addChild(pDir);
		}
	}
	static  void save(const std::string& path, const std::string& file)
	{
		if (fileStack.empty()){
			std::vector<std::string> files = { file };
			std::cout << "pushed" << path << "\n";
			fileStack.push(std::make_pair(path, files));
			std::cout << "file: " << file << "\n";
			sPtr pdir = XmlElementFactory::makeTaggedElement("Dir");
			sPtr pfile = XmlElementFactory::makeTaggedElement("file");
			pfile->addAttrib("name", file);
			pdir->addChild(pfile);
			_elementStack.push(pdir);
		}

		else if (fileStack.top().first == path)
		{
			fileStack.top().second.push_back(file);
			sPtr pfile =XmlElementFactory::makeTaggedElement("file");
			pfile->addAttrib("name", file);
			_elementStack.top()->addChild(pfile);
			std::cout << "file: " << file << "\n";
		}
		else if (path.find(fileStack.top().first) == 0)/*hInside the folder*/
		{
			sPtr pdir = XmlElementFactory::makeTaggedElement("Dir");
			std::string _path;
			pdir->addAttrib("path", _path);
			sPtr pfile = XmlElementFactory::makeTaggedElement("file");
			pfile->addAttrib("name", file);
			pdir->addChild(pfile);
			_elementStack.push(pdir);
			std::vector<std::string> files = { file };
			std::cout << "pushed" << path << "\n";
			fileStack.push(std::make_pair(path, files));
			std::cout << "file: " << file << "\n";
		}
		else
		{
			std::cout << "popped" << fileStack.top().first << "\n";
			fileStack.pop();
			sPtr pDir = _elementStack.top();
			_elementStack.pop();
			_elementStack.top()->addChild(pDir);
			save(path, file);
		}
	}
};
std::stack<std::pair<std::string, std::vector<std::string>>> DirectoryStack::fileStack;
std::stack<std::shared_ptr<XmlProcessing::AbstractXmlElement>> DirectoryStack::_elementStack;
std::string  DirectoryStack::rootDir;

void main()
{
	DataStore store;
	std::vector<std::string> patterns = { "*.*" };
	FileManager manager("C:/Users/isira/Documents/roots/peer1", patterns, true);
	//manager.search([&](const std::string& path, const std::string& file){store.save(path, file); });
	std::string parent = "C:/Users/isira/Documents/roots/peer1";
	sPtr pRoot = XmlElementFactory::makeTaggedElement("Directories");
	DirectoryStack::rootDir = "C:/Users/isira/Documents/roots/peer1";
	DirectoryStack::_elementStack.push(pRoot);
	manager.search(DirectoryStack::save);
	DirectoryStack::finalise();
	std::cout << pRoot->toString();
	Cataloger cataloger(store);
	//cataloger.DuplicateFiles();

	//store.save("../Cataloger","Cataloger.cpp");
	//cataloger.FilesWithSearchText("DataStore");
	//cataloger.FilesWithSearchText("DataStore", {"*.txt"});

	std::map<std::string, std::list<std::string>> map = cataloger.directoryMapRelativeToRoot("C:/Users/isira/Documents/Project_4/CppCli-WPF-App/Cataloger");
	//std::cout<<"..........: "<<FileSystem::Directory::relativePath("dir","dir/temp/to/go/textfile.txt");
	for (auto it : map)
	{
		std::string  path = it.first;
		std::list<std::string> files = it.second;
		std::cout << "\n\nDirectory:" << path;
		for (auto i : files)
		{
			std::string file = i;
			std::cout << "\nFile:" << file;
		}
	}
	std::cout << "\n\n";
	std::cout << cataloger.directoryMapRelativeToRootXml("C:/Users/isira/Documents/Project_4/CppCli-WPF-App/Cataloger");
;

}
#endif