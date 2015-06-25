//////////////////////////////////////////////////////////////////////////////
// Server.cpp -  Class represents a peer containing a sender and a receiver	//
// ver 1.0																	//
// -------------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015										//
// All rights granted provided that this notice is retained					//
// -------------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013						//
// Platform:    Mac Book Pro, Core i5, Windows 8.1							//
// Application: Project #4 – Message Passing Communication,2015				//
// Author:      Isira Samarasekera, Syracuse University						//
//              issamara@syr.edu											//
//////////////////////////////////////////////////////////////////////////////

#include "Server.h"
#include  "../XmlDocument/XmlDocument.h"
#include "../XmlElement/XmlElementFactory.h"

#include "../DocumentBuilder/DocumentBuilder.h"
#include "../MockChannel/Receiver.h"
#include "../MockChannel/Sender.h"
#include "../FileManager/FileManager.h"
#include "../ChronoTimer/ChronoTimer.h"

#include <regex>
#include <future>
using namespace XmlProcessing;

//----< Constructor for File Downloader functor >------------------------------------

FileDownloadFunctor::FileDownloadFunctor(std::shared_ptr<Sender> sender) :_sender(sender)
{
}

//----< File Download message Handler method >------------------------------------

void FileDownloadFunctor::operator()(const ServiceMessage& msg)
{
	if (msg.isReply())
	{
		Display::show("File download Successful", true);
	}
	else
	{
		std::string downloadMsg = msg.attribute("file") + " requested to be downloaded " + msg.attribute("srcIp") + ":" + msg.attribute("srcPort") + "\n";

		ServiceMessage reply = msg.replyMessage();
		_sender->postMessage(reply);
		Display::show(downloadMsg, true);
	}
}

//----< Constructor for file upload functor >------------------------------------

FileUploadFunctor::FileUploadFunctor(std::shared_ptr<Sender> sender) :_sender(sender)
{
}

//----< File upload message handler  >------------------------------------

void FileUploadFunctor::operator()(const ServiceMessage& msg)
{
	if (msg.isReply())
	{
		Display::show("File Upload Successful", true);
	}
	else
	{
		std::string uploadMsg = msg.attribute("file") + " uploaded from " + msg.attribute("srcIp") + ":" + msg.attribute("srcPort") + "\n";
		Display::show(uploadMsg, true);
		ServiceMessage reply = msg.replyMessage();
		reply.addAttribute("success","true");
		_sender->postMessage(reply);
	}
}

std::stack<std::shared_ptr<XmlProcessing::AbstractXmlElement>> Explore::_elementStack;
std::string Explore::rootPath;

//----< Call back to be called when a file is found  >------------------------------------

void Explore::fileFound(const std::string& file)
{
	sPtr pfile = XmlElementFactory::makeTaggedElement("file");
	pfile->addAttrib("name", file);
	_elementStack.top()->addChild(pfile);
}

//----< Call back to be called when entering a directory  >------------------------------------

void Explore::enterDir(const std::string& dir)
{
	sPtr pDir = XmlElementFactory::makeTaggedElement("Dir");
	std::string dirName;
	dir == rootPath ? dirName = "/" : dirName = FileSystem::Path::getName(dir);

	pDir->addAttrib("path", dirName);
	_elementStack.push(pDir);
}

//----< Call back to be called when leaving a directory  >------------------------------------

void Explore::leaveDir(const std::string& dir)
{
	sPtr pDir = _elementStack.top();
	_elementStack.pop();
	_elementStack.top()->addChild(pDir);
}

BlockingQueue<std::string> Search::files;
BlockingQueue<std::string> Search::found;

//----< add a file to search in to the file queue  >------------------------------------

void Search::searchFile(const std::string& path, const std::string& file)
{
	files.enQ(FileSystem::Path::fileSpec(path,file));
}

//----< DeQ a file path from the queue and search the file for text   >------------------------------------

void Search::searchTextInFile( std::string& searchText,bool isRegex)
{
	while (true)
	{
		std::string filePath = files.deQ();
		if (filePath == "quit" )
		{
			files.enQ("quit");
			break;
		}
		else if (files.size() == 0)
		{
			files.enQ("quit");
		}
		
		FileSystem::File testAll(filePath);
		std::string fileText;
		testAll.open(FileSystem::File::in);
		if (testAll.isGood())
		{
			fileText = testAll.readAll();
			if (isRegex)
			{
				std::regex string_regex(searchText);
				if (std::regex_search(fileText, string_regex))
					Search::found.enQ(filePath);
			}
			else
			{
				std::size_t found = fileText.find(searchText);
				if (found != std::string::npos)
					Search::found.enQ(filePath);
			}
		}
	}
	
}

//----< Constructor for text search functor  >------------------------------------

TextSearchFunctor::TextSearchFunctor(std::shared_ptr<Sender> sender) :_sender(sender)
{
}


//----< Text search message handler  >------------------------------------

void TextSearchFunctor::operator()(const ServiceMessage& msg)
{
		XmlProcessing::XmlDocument doc(msg.body());
		XmlProcessing::DocumentBuilder builder;
		builder.build(doc);
		std::vector<XmlProcessing::sPtr> results = doc.elements("Pattern").select();
		std::vector<std::string> patterns;
		for (auto it : results)
			patterns.push_back(it->attribValue("value"));
		
		std::string searchText = msg.attribute("text");
		bool isRegex = msg.attribute("regex")=="true";

		std::string searchDir;
		msg.attribute("dir") == "/" ? searchDir = _sender->root() : searchDir = _sender->root() + "/" + msg.attribute("dir");
		FileManager manager(searchDir, patterns, true);
		manager.search(Search::searchFile);

		size_t numberOfThreads = std::stoi(msg.attribute("threads"));;
		std::vector<std::future<void>> futures(numberOfThreads);

		Timer timer;
		timer.start();
		for (size_t i = 0; i<numberOfThreads; ++i)
			futures[i] = std::async(Search::searchTextInFile, searchText, isRegex); 
		for (size_t i = 0; i<numberOfThreads; ++i)
			 futures[i].get(); 
		timer.stop();
		__int64 timeElapsed = timer.elapsedTime<std::chrono::milliseconds>();
		std::cout << "\n  measured time in microseconds = " << timeElapsed << " for threads:" << numberOfThreads;
		Search::files.deQ();
		sPtr root = XmlElementFactory::makeTaggedElement("SearchResults");
		root->addAttrib("Text", searchText);
		while (Search::found.size()>0){
			std::string it = Search::found.deQ();
			std::string __root = _sender->root();
			std::size_t _found = it.find(__root);
			if (_found != std::string::npos &&_found == 0)
				it = it.substr(__root.size());

			sPtr pattern = XmlElementFactory::makeTaggedElement("Result");
			pattern->addAttrib("value", it);
			root->addChild(pattern);
		}
		ServiceMessage reply = msg.replyMessage();
		reply.addAttribute("time", toString(timeElapsed));
		reply.setBody(root->toString());
		_sender->postMessage(reply);
}

//----< callback method to be called when a explore root folder is received >------------------------------------

ExploreFunctor::ExploreFunctor(std::shared_ptr<Sender> sender) :_sender(sender)
{
}
//----< Explore message handler  >------------------------------------

void ExploreFunctor::operator()(const ServiceMessage& msg)
{
	if (msg.isReply())
	{
		Display::show("Explore directory results received", true);
	}
	else 
	{
		std::vector<std::string> patterns;
		std::string fullDirPath;
		msg.attribute("dir") == "/" ? fullDirPath = _sender->root() : fullDirPath = _sender->root() + "/" + msg.attribute("dir");
		if (!msg.body().empty())
		{
			XmlDocument xmlDoc(msg.body());
			DocumentBuilder builder;
			builder.build(xmlDoc);
			std::vector<sPtr> _patterns = xmlDoc.elements("Pattern").select();
			for (auto it : _patterns)
			{
				patterns.push_back(it->attribValue("value"));
			}

		}
		sPtr pRoot = XmlElementFactory::makeTaggedElement("Dir");
		pRoot->addAttrib("path", msg.attribute("dir"));
		Explore::rootPath = fullDirPath;
		Explore::_elementStack.push(pRoot);
		Timer timer;
		timer.start();
		FileManager manager(fullDirPath, patterns);
		manager.findFiles(fullDirPath, Explore::fileFound, Explore::enterDir, Explore::leaveDir);
		timer.stop();
		__int64 timeElapsed = timer.elapsedTime<std::chrono::milliseconds>();
		ServiceMessage reply = msg.replyMessage();
		reply.addAttribute("time", toString(timeElapsed));
		reply.setBody(pRoot->toString());
		_sender->postMessage(reply);
		Explore::_elementStack.pop();
	}
	
}

//----< Constructor for server  >------------------------------------

Server::Server(size_t port) {
	ObjectFactory factory;
	IRecvr *receiver = factory.createRecvr(port);
	ISendr *sender = factory.createSendr();
	_receiver = std::make_shared<Receiver>(std::move(*(dynamic_cast<Receiver*>(receiver))));
	_sender = std::make_shared<Sender>(std::move(*(dynamic_cast<Sender*>(sender))));
}

//----< register callbacks start receicer and sender >------------------------------------

void Server::start()
{
	_receiver->start();
	_sender->start();

	FileUploadFunctor uploadFunctor(_sender);
	FileDownloadFunctor downloadFunctor(_sender);
	ExploreFunctor exploreRootFunctor(_sender);
	TextSearchFunctor textSearchFunctor(_sender);

	_receiver->registerCallback("upload",uploadFunctor);
	_receiver->registerCallback("download", downloadFunctor);
	_receiver->registerCallback("explore", exploreRootFunctor);
	_receiver->registerCallback("search", textSearchFunctor);
	
}

//----< stop receiver and sender >------------------------------------

void Server::stop()
{
	_sender->close();
	_receiver->close();
	
}

//----< get peers receiving port >------------------------------------

size_t Server::port()
{
	return _receiver->port();
}

//----< set root directory  >------------------------------------

void Server::setRootDirectory(const std::string& rootDir)
{
	_rootDirectory = rootDir;
	_receiver->setRootDir(rootDir);
	_sender->setRootDir(rootDir);
}

//----< destruct server >------------------------------------

Server::~Server()
{
	stop();
}

#ifdef TEST_SERVER

struct Cosmetic
{
	~Cosmetic()
	{
		{
			std::lock_guard<std::mutex> lock(Display::mtx);
			std::cout << "\n  press Enter to exit: ";
		}	
		std::cin.get();
		{
			std::lock_guard<std::mutex> lock(Display::mtx);
			std::cout << "\n\n";
		}		
	}
};

//----< create file upload message >------------------------------------

ServiceMessage createFileUploadMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, std::string file, const std::string& remoteFolder)
{
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "upload"));
	attribs.push_back(std::make_pair("srcIp", srcIp));
	attribs.push_back(std::make_pair("srcPort", srcPort));
	attribs.push_back(std::make_pair("dstIp", dstIp));
	attribs.push_back(std::make_pair("dstPort", dstPort));
	attribs.push_back(std::make_pair("file", file));
	attribs.push_back(std::make_pair("dir", remoteFolder));

	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs);
	return std::move(msg);

}


//----< create file upload message >------------------------------------

ServiceMessage createFileDownloadMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, const std::string& remoteFilePath, const std::string& localFolderPath)
{
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "download"));
	attribs.push_back(std::make_pair("srcIp", srcIp));
	attribs.push_back(std::make_pair("srcPort", srcPort));
	attribs.push_back(std::make_pair("dstIp", dstIp));
	attribs.push_back(std::make_pair("dstPort", dstPort));
	attribs.push_back(std::make_pair("file", remoteFilePath));
	attribs.push_back(std::make_pair("dir", localFolderPath));

	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs);
	return std::move(msg);

}


//----< create path explore message >------------------------------------

ServiceMessage createPathExploreMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, std::string path, std::vector<std::string> patterns)
{
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "explore"));
	attribs.push_back(std::make_pair("srcIp", srcIp));
	attribs.push_back(std::make_pair("srcPort", srcPort));
	attribs.push_back(std::make_pair("dstIp", dstIp));
	attribs.push_back(std::make_pair("dstPort", dstPort));
	attribs.push_back(std::make_pair("dir", path));

	std::string body;
	if (!patterns.empty())
	{
		sPtr root = XmlElementFactory::makeTaggedElement("Patterns");
		for (auto it : patterns)
		{
			sPtr pattern = XmlElementFactory::makeTaggedElement("Pattern");
			pattern->addAttrib("value", it);
			root->addChild(pattern);
		}
		body = root->toString();
	}

	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs, body);
	return std::move(msg);

}


ServiceMessage createTextSearchMessage(std::string srcIp, std::string srcPort,
	std::string  dstIp, std::string dstPort, std::string path, std::vector<std::string> patterns, std::string searchText, bool isRegex, int numThreads)
{
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "search"));
	attribs.push_back(std::make_pair("srcIp", srcIp));
	attribs.push_back(std::make_pair("srcPort", srcPort));
	attribs.push_back(std::make_pair("dstIp", dstIp));
	attribs.push_back(std::make_pair("dstPort", dstPort));
	attribs.push_back(std::make_pair("dir", path));
	attribs.push_back(std::make_pair("text", searchText));
	attribs.push_back(std::make_pair("threads", toString(numThreads)));
	if (isRegex)
		attribs.push_back(std::make_pair("regex", "true"));

	std::string body;
	if (!patterns.empty())
	{
		sPtr pChild1 = XmlElementFactory::makeTaggedElement("Patterns");
		for (auto it : patterns)
		{
			sPtr pChild = XmlElementFactory::makeTaggedElement("Pattern");
			pChild->addAttrib("value", it);
			pChild1->addChild(pChild);
		}
		body = pChild1->toString();
	}

	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs, body);
	return std::move(msg);
}


std::string toString(int t)
{
	std::ostringstream out;
	out << t;
	return out.str();
}

//----< upload the file in the path to the peer listening for the given port >------------------------------------

void Server::uploadFile(const std::string& fileName, const std::string& remoteFolder, int _port)
{
	ServiceMessage msg = createFileUploadMessage("localhost", toString(port()), "localhost", toString(_port), fileName, remoteFolder);
	_sender->postMessage(std::move(msg));
}

//----< upload the file in the path to the peer listening for the given port >------------------------------------

void Server::downloadFile(const std::string& remoteFilePath, const std::string& localFolderPath, int _port)
{
	ServiceMessage msg = createFileDownloadMessage("localhost", toString(port()), "localhost", toString(_port), remoteFilePath, localFolderPath);
	_sender->postMessage(std::move(msg));
}


void Server::exploreDirectory(const std::string& path, int _port, std::vector<std::string> patterns)
{
	ServiceMessage msg = createPathExploreMessage("localhost", toString(port()), "localhost", toString(_port), path, patterns);
	_sender->postMessage(std::move(msg));
}

void Server::exploreRootDirectory(int _port)
{
	ServiceMessage msg = createPathExploreMessage("localhost", toString(port()), "localhost", toString(_port), "/", { "*.*" });
	_sender->postMessage(std::move(msg));
}

void Server::searchText(const std::string& path, std::vector<std::string> patterns, std::string searchText, int _port, bool isRegex, int numThreads)
{
	ServiceMessage msg = createTextSearchMessage("localhost", toString(port()), "localhost", toString(_port), path, patterns, searchText, isRegex, numThreads);
	_sender->postMessage(std::move(msg));
}
void main(int argc, char* argv[])
{

	try
	{	
		Display::setShowMessage(true);
		Server peer(std::stoi(argv[1]));
		peer.start();
		peer.setRootDirectory(std::string(argv[2]));
		
		std::cout << "\npeer started on port: " << peer.port() <<"\n";
		//peer.exploreRootDirectory(std::stoi(argv[1]));
		std::vector<std::string> patterns = {"b*.*"};
		//peer.uploadFile("C:/Users/isira/Documents/download/abc.png", ".",std::stoi(argv[1]));
		//peer.downloadFile("BouncingBall/pr4s15.pdf", "C:/Users/isira/Documents/roots", std::stoi(argv[1]));
		//peer.exploreDirectory("/BouncingBall/BouncingBall/bin", 9996,patterns);
		//peer.searchText("/", { "*.*" }, "Jim Fawcett (c) copyright", 9996, false,5);
		Cosmetic cosmetic;
	
	}
	catch (std::exception& ex)
	{
		Display::show("  Exception caught:", always);
		Display::show(std::string("\n  ") + ex.what() + "\n\n");
	}
}

#endif