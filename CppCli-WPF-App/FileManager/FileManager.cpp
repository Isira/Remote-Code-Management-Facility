//////////////////////////////////////////////////////////////////////////
// FileManager.cpp -	Find files given directory path, patterns and to	//
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

#include "FileManager.h"

FileManager::FileManager(const std::string path, std::vector<std::string> patterns, bool recursive) :
						path_(path), _patterns(patterns), _recursive(recursive)
{
	if (_patterns.empty()){
		_patterns.push_back("*.*");
	}

	if (path_.empty()){
		path_ = ".";
	}

}


#ifdef TEST_FILEMANAGER
#include <iostream>
#include <stack>
class DirectoryStack
{
public:
	static  std::stack<std::pair<std::string,std::vector<std::string>>> fileStack;
	static  void save(const std::string& path, const std::string& file)
	{
		if (fileStack.empty()){
			std::vector<std::string> files = { file };
			std::cout << "pushed" << path <<"\n";
			fileStack.push(std::make_pair(path, files));
			std::cout << "file: " << file <<"\n";
		}

		else if (fileStack.top().first == path)
		{
			fileStack.top().second.push_back(file);
			std::cout << "file: " << file <<"\n";
		}
		else if (path.find(fileStack.top().first) == 0)/*hInside the folder*/
		{
			std::vector<std::string> files = { file };
			std::cout << "pushed" << path<< "\n";
			fileStack.push(std::make_pair(path, files));
			std::cout << "file: " << file <<"\n";
		}
		else
		{
			std::cout << "popped" << fileStack.top().first<<"\n";
			fileStack.pop();
			save(path, file);
 		}
	}
};

std::stack<std::pair<std::string,std::vector<std::string>>> DirectoryStack::fileStack;
#include <iostream>
int main()
{
	using namespace std;
	cout << "Recursive finding files" << endl;
	cout << "========================" << endl;
	cout << endl;
	{
		FileManager fm(".", { "*.*" }, true);
		fm.findFiles(".", 
			[]( const std::string file)
			{
				std::cout << "\n  File: " << file;
			}, 
			[]( const std::string& dir)
			{
				std::cout << "\nEntering Dir: "  << dir;
			},
			[](const std::string& dir)
			{
				std::cout << "\nLeaving Dir: " << dir;
			});

	}

	{
		DataStore store;
		FileManager fm(".", { "*.*" }, true);
		fm.search(DirectoryStack::save);

		for (auto it : store)
		{
			std::string  path = it.first;
			auto files = it.second;
			std::cout << path << "\n";
			for (auto i : files)
			{
				std::string file = *i;
				std::cout << "\t" << file << "\n";
			}
		}

	}

	cout << "\n\nNon Recursive finding files" << endl;
	cout << "============================" << endl;
	cout << endl;
	{
		DataStore store;
		FileManager fm(".", { "*.h", "*.cpp" }, false);
		fm.search([&](const std::string& path, const std::string& file){store.save(path, file); });
		for (auto it : store)
		{
			std::string  path = it.first;
			auto files = it.second;
			std::cout << "\nFile:" << path;
			for (auto i : files)
			{
				std::string file = *i;
				std::cout << "\nDirectory:" << file;
			}
		}
	}


}
#endif