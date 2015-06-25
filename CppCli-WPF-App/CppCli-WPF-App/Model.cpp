//////////////////////////////////////////////////////////////////////////
// Model.cpp-  provide model for  Window									//
// ver 1.0																//
// ---------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015									//
// All rights granted provided that this notice is retained				//
// ---------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013                 //
// Platform:    Mac Book Pro, Core i5, Windows 8.1						//
// Application: Project #4 				//
// Author:      Isira Samarasekera, Syracuse University					//
//              issamara@syr.edu										//
//////////////////////////////////////////////////////////////////////////

#include "Model.h"
#include <sstream>
using namespace System;

//---< get performance details for the the text search  >---------------------------

std::string Model::performanceDetailsForTextSearch(ServiceMessage& msg)
{
	std::string _msg = " Text search reply received from " + msg.attribute("srcIp") + "@" + msg.attribute("srcPort")
		+ "; time taken for processing: " + msg.attribute("time") + " milliseconds with " + msg.attribute("threads") + " threads";
	return _msg;
}

//---< get performance details for the the file search  >---------------------------

std::string Model::performanceDetailsForFileSearch(ServiceMessage& msg)
{
	std::string _msg = " File search reply received from " + msg.attribute("srcIp") + "@" + msg.attribute("srcPort")
		+ "; time taken for processing: " + msg.attribute("time") + " milliseconds";
	return _msg;
}
 
//---< get file paths with endpoint messages  >---------------------------

std::vector<std::string> Model::getFilePathsWithEndpointFromMessage(ServiceMessage& msg)
{
	std::vector<std::string> fileNameWithEndpoint;
	XmlDocumentFactory factory;
	IXmlDocument* doc = factory.makeXmlDocument(msg.body());
	std::vector<IAbstractXmlElement*> results = doc->elements("Result").selectNativePointer();
	for (unsigned int i = 0; i < results.size(); i++)
	{
		fileNameWithEndpoint.push_back(msg.attribute("srcIp") + "@" + msg.attribute("srcPort") + results[i]->attribValue("value"));
	}
	return fileNameWithEndpoint;
}

//---< get directory paths from message body  >---------------------------

std::vector<std::string> Model::getDirectoryPathsFromMessageBody(std::string body)
{
	XmlDocumentFactory factory;
	IXmlDocument* doc = factory.makeXmlDocument(body);

	IAbstractXmlElement* directories = doc->elements("Dir").selectNativePointer()[0];
	std::vector<std::string> dirs = folderPaths(directories);
	return dirs;

}

//---< get file paths from message body  >---------------------------

std::vector<std::string> Model::getFilePathsFromMessageBody(std::string body)
{
	XmlDocumentFactory factory;
	IXmlDocument* doc = factory.makeXmlDocument(body);

	IAbstractXmlElement* directories = doc->elements("Dir").selectNativePointer()[0];
	std::vector<std::string> dirs = filePaths(directories);
	return dirs;
}

//---< split string by delim  >---------------------------

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		std::stringstream trimmer;
		trimmer << item;
		item.clear();
		trimmer >> item;
		elems.push_back(item);
	}
	return elems;
}

//---< split string by delim  >---------------------------

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

//---< parse patterns from string >---------------------------

std::vector<std::string> Model::parsePatternsFromString(std::string patternString)
{
	std::vector<std::string> patterns = split(patternString,',');
	return patterns;
}

//---< parse remote endpoint from string >---------------------------

std::vector<std::pair<std::string, int>> Model::parseRemoteEndpointsFromString(std::string patternString)
{
	std::vector<std::pair<std::string, int>> enpointSettings;
	std::vector<std::string> endpoints = split(patternString, ',');

	for (unsigned int i = 0; i < endpoints.size(); i++)
	{
		std::vector<std::string> configs = split(endpoints[i], '@');
		std::string ip = configs[0];
		int port = std::stoi(configs[1]);
		enpointSettings.push_back(std::make_pair(ip, port));
	}
	return enpointSettings;
}

//---< start client receiver thread >---------------------------

void Model::startClientReceiverThread()
{
	recvThread = gcnew Thread(gcnew ThreadStart(this, &Model::getMessage));
	recvThread->Start();
}

//---< set local port >---------------------------

void Model::setLocalPort(size_t port)
{
	localPort = port;
}

//---< start client side objects >---------------------------

void Model::startClientSideObjects()
{
	setUpChannel();
	startClientReceiverThread();
}

//---< close channel >---------------------------

void Model::close()
{
	pChann_->stop();
}

//---< get port >---------------------------

size_t Model::port()
{
	return localPort;
}

//---< upload file >---------------------------

void Model::uploadFile(const std::string& fileName, const std::string& remoteFolder, std::string _ip, int _port)
{
	ServiceMessage msg = ClientMessageFactory::createFileUploadMessage("localhost", std::to_string(port()), _ip, std::to_string(_port), fileName, remoteFolder);
	pSendr_->postMessage(std::move(msg));
}

//----< download file >------------------------------------

void Model::downloadFile(const std::string& remoteFilePath, const std::string& localFolderPath, std::string _ip, int _port)
{
	ServiceMessage msg = ClientMessageFactory::createFileDownloadMessage("localhost", std::to_string(port()), _ip, std::to_string(_port), remoteFilePath, localFolderPath);
	pSendr_->postMessage(std::move(msg));
}

//----< explore directory >------------------------------------

void Model::exploreDirectory(const std::string& path, std::string _ip, int _port, std::vector<std::string> patterns)
{
	ServiceMessage msg = ClientMessageFactory::createPathExploreMessage("localhost", std::to_string(port()), _ip, std::to_string(_port), path, patterns);
	pSendr_->postMessage(std::move(msg));
}

//----< explore root directory >------------------------------------

void Model::exploreRootDirectory(std::string _ip, int _port)
{
	std::vector<std::string> patterns = {"*.*" };
	ServiceMessage msg = ClientMessageFactory::createPathExploreMessage("localhost", std::to_string(port()), _ip, std::to_string(_port), "/", patterns);
	pSendr_->postMessage(std::move(msg));
}

//----< search Text >------------------------------------

void Model::searchText(std::vector<std::string> patterns, std::string searchText, std::string _ip, int _port, bool isRegex, int numThreads)
{
	ServiceMessage msg = ClientMessageFactory::createTextSearchMessage("localhost", std::to_string(port()), _ip, std::to_string(_port), "/", patterns, searchText, isRegex, numThreads);
	pSendr_->postMessage(std::move(msg));
}

//----< get message from receiver thread >------------------------------------

void Model::getMessage()
{
	while (true)
	{
		ServiceMessage msg = pRecvr_->getMessage();
		if (msg.attribute("cmd") == "quit")
			break;
		else if (msg.attribute("cmd") == "upload")
			fileUploadMessageReceived(msg);
		else if (msg.attribute("cmd") == "download")
			fileDownloadMessageReceived(msg);
		else if (msg.attribute("cmd") == "explore")
			pathExploreMessageReceived(msg);
		else if (msg.attribute("cmd") == "search")
			textSearchMessageReceived(msg);
	}
}

//----< set up channel >------------------------------------

void Model::setUpChannel()
{
	ObjectFactory pObjFact;
	pSendr_ = pObjFact.createSendr();
	pRecvr_ = pObjFact.createRecvr(localPort);
	pChann_ = pObjFact.createMockChannel(pSendr_, pRecvr_);
	pChann_->start();
}

//----< model destructor >------------------------------------

Model::~Model()
{
	delete pSendr_;
	delete pRecvr_;
	delete pChann_;
}

//----< create File Upload Message >------------------------------------

ServiceMessage ClientMessageFactory::createFileUploadMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, std::string file, const std::string& remoteFolder)
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

ServiceMessage ClientMessageFactory::createFileDownloadMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, const std::string& remoteFilePath, const std::string& localFolderPath)
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

ServiceMessage ClientMessageFactory::createPathExploreMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, std::string path, std::vector<std::string> patterns)
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
		XmlFactory _factory;
		IAbstractXmlElement* root = _factory.makeTaggedElementRowPointer("Patterns");
		for (auto it : patterns)
		{
			IAbstractXmlElement* pattern = _factory.makeTaggedElementRowPointer("Pattern");
			pattern->addAttrib("value", it);
			root->addChildNativePtr(pattern);
		}
		body = root->toString();
	}

	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs, body);
	return std::move(msg);

}


//----< create Text Search Message >------------------------------------

ServiceMessage ClientMessageFactory::createTextSearchMessage(std::string srcIp, std::string srcPort,
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
	attribs.push_back(std::make_pair("threads", std::to_string(numThreads)));
	if (isRegex)
		attribs.push_back(std::make_pair("regex", "true"));

	std::string body;
	if (!patterns.empty())
	{
		XmlFactory factory;
		IAbstractXmlElement* pChild1 = factory.makeTaggedElementRowPointer("Patterns");
		for (auto it : patterns)
		{
			IAbstractXmlElement* pChild = factory.makeTaggedElementRowPointer("Pattern");
			pChild->addAttrib("value", it);
			pChild1->addChildNativePtr(pChild);
		}
		body = pChild1->toString();
	}

	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs, body);
	return std::move(msg);
}

//----< Get Folder paths from Xml >------------------------------------

void folderPaths_(IAbstractXmlElement* element, const std::string& parentPath, std::vector<std::string> &paths)
{
	if (element->tag() == "Dir")
	{
		std::string elementPath;
		element->attribValue("path") == "/" ? elementPath = element->attribValue("path") : elementPath = parentPath + "/" + element->attribValue("path");
		paths.push_back(elementPath);
		
		
		std::vector<IAbstractXmlElement*> _childrn = element->childreNativePtrs();

		for (unsigned int i = 0; i < _childrn.size(); i++)
		{

			std::string Child = _childrn.at(i)->toString();
			folderPaths_(_childrn.at(i), elementPath, paths);
		}
	}	
}

//----< Get file paths from Xml >------------------------------------

void filePaths_(IAbstractXmlElement* element, const std::string& parentPath, std::vector<std::string> &paths)
{
	if (element->tag() == "Dir")
	{
		std::string elementPath;
		element->attribValue("path") == "/" ? elementPath = element->attribValue("path") : elementPath = parentPath + "/" + element->attribValue("path");

		std::vector<IAbstractXmlElement*> _childrn = element->childreNativePtrs();

		for (unsigned int i = 0; i < _childrn.size(); i++)
		{
			IAbstractXmlElement* Child = _childrn.at(i);
			if (Child->tag()=="file")
			{
				std::string filePath = elementPath + "/" + Child->attribValue("name");
				paths.push_back(filePath);
			}
			filePaths_(_childrn.at(i), elementPath, paths);
		}
	}
}

//----< Get file paths from IAbstractXmlElement >------------------------------------

std::vector<std::string> Model::filePaths(IAbstractXmlElement* element)
{
	std::vector<std::string> paths;
	filePaths_(element, std::string(""), paths);
	return paths;
}

//----< Get folder paths from IAbstractXmlElement >------------------------------------

std::vector<std::string> Model::folderPaths(IAbstractXmlElement* element)
{
	std::vector<std::string> paths;
	folderPaths_(element,std::string(""),paths);
	return paths;

}

#ifdef TEST_MODEL
void main()
{
	ServiceMessage msg = ClientMessageFactory::createTextSearchMessage("localhost", "1234", "localhost", "2345", "/", {"*.*"}, "see", false, 4);
	Console::Write(msg.attribute("cmd"));
	Console::Write(msg.attribute("search"));
}
#endif