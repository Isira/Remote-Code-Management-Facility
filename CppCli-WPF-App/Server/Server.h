#ifndef SERVER_H
#define SERVER_H

//////////////////////////////////////////////////////////////////////////////
// Peer.h -  Class represents a peer containing a sender and a receiver		//
// ver 1.0																	//
// -------------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015										//
// All rights granted provided that this notice is retained					//
// -------------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013						//
// Platform:    Mac Book Pro, Core i5, Windows 8.1							//
// Application: Project #3 – Message Passing Communication,2015				//
// Author:      Isira Samarasekera, Syracuse University						//
//              issamara@syr.edu											//
//////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
*	Wrapping a single module with a sender and receiver, capable of sending messages and receiving messages.
*
* Public Interface:
* =================
*	Server peer(1234);
*	peer.start();
*	peer.uploadFile("/path/to/file",1235);
*	size_t port = peer.port(); // this returns 1234 for port
*	peer.setDownloadDirectory("path/to/download/directory");

* Required Files:
* ===============
*	Peer.h	Peer.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_SERVER Server.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 12 April 15
* ver 2.0 : 28 April 15
* - first release
*/


#include <memory>
#include "../Cataloger/Cataloger.h"
#include <stack>
#include "../XmlDocument/XmlDocument.h"
#include "../MockChannel/Receiver.h"
#include "../MockChannel/Sender.h"



struct Explore
{
	static std::string rootPath;
	static std::stack<std::shared_ptr<XmlProcessing::AbstractXmlElement>> _elementStack;
	static  void fileFound(const std::string& file);
	static  void enterDir(const std::string& path);
	static void leaveDir(const std::string& path);
};

struct Search
{
	static BlockingQueue<std::string> found;
	static BlockingQueue<std::string> files;
	static void searchFile(const std::string& path, const std::string& file);
	static void searchTextInFile( std::string& searchText, bool isRegex);
};

class FileUploadFunctor
{
	std::shared_ptr<Sender> _sender;
public:
	FileUploadFunctor(std::shared_ptr<Sender> sender);
	void operator()(const ServiceMessage& msg);
};


class FileDownloadFunctor
{
	std::shared_ptr<Sender> _sender;
	//----< callback method to be called when a file is downloaded >------------------------------------
public:
	FileDownloadFunctor(std::shared_ptr<Sender> sender);
	void operator()(const ServiceMessage& msg);
};

//----< callback method to be called when a explore root folder is received >------------------------------------

class ExploreFunctor
{
	std::shared_ptr<Sender> _sender;
public:
	ExploreFunctor(std::shared_ptr<Sender> sender);
	void operator()(const ServiceMessage& msg);
};

//----< callback method to be called when file search command is received >------------------------------------

class TextSearchFunctor
{
	std::shared_ptr<Sender> _sender;

public:
	TextSearchFunctor(std::shared_ptr<Sender> sender);
	void operator()(const ServiceMessage& msg);
};



class Server
{
public:
	Server(size_t port);
	void start();
	void setRootDirectory(const std::string& rootDir);
	size_t port();
	void stop();
	~Server();

	void uploadFile(const std::string& fileName,const std::string& remoteFolder,int _port);
	void downloadFile(const std::string& remoteFilePath, const std::string& localFolderPath, int port);
	void exploreDirectory(const std::string& path, int _port, std::vector<std::string> patterns);
	void searchText(const std::string& path, std::vector<std::string> patterns, std::string searchTexts, int _port, bool isRegex,int numThreads);
	void exploreRootDirectory( int _port);

private:
	
	std::shared_ptr<Receiver> _receiver;
	std::shared_ptr<Sender> _sender;
	std::string _rootDirectory;

};
#endif