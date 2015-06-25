#ifndef MODEL_H
#define MODEL_H

//////////////////////////////////////////////////////////////////////////
// Model.h -  provide model for  Window									//
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
/*
* Module Operations:
* ==================
* This module provides the model for the window Ui, and provides methods to create messages

* Public Interface:
* =================
*	std::string performanceDetailsForTextSearch(ServiceMessage& msg);
	std::string performanceDetailsForFileSearch(ServiceMessage& msg);
	std::vector<std::string> getFilePathsWithEndpointFromMessage(ServiceMessage& msg);
	std::vector<std::string> getDirectoryPathsFromMessageBody(std::string body);
	std::vector<std::string> getFilePathsFromMessageBody(std::string body);
	std::vector<std::string> parsePatternsFromString(std::string patternString);
	std::vector<std::pair<std::string,int>> parseRemoteEndpointsFromString(std::string patternString);
	void setUpChannel();
	void startClientReceiverThread();
	void startClientSideObjects();
	void uploadFile(const std::string& fileName, const std::string& remoteFolder,std::string _ip, int _port);
	void downloadFile(const std::string& remoteFilePath, const std::string& localFolderPath, std::string _ip, int port);
	void exploreDirectory(const std::string& path, std::string _ip, int _port, std::vector<std::string> patterns);
	void searchText(std::vector<std::string> patterns, std::string searchTexts, std::string _ip, int _port, bool isRegex, int numThreads);
	void exploreRootDirectory(std::string _ip, int _port);
*
* Required Files:
* ===============
* IXmlDocument.h MockChannel.h"
*
* Build Command:
* ==============
* cl /EHa /DTEST_MODEL Model.cpp 
*
* Maintenance History:
* ====================
* ver 1.0 : 38 04 Feb 15
* - first release
*/

#include <string>
#include <vector>
#include "../XmlDocument/IXmlDocument.h"
#include "../MockChannel/MockChannel.h"
using namespace System::Threading;

delegate void FileUploadMessageReceived(ServiceMessage& msg);
delegate void FileDownloadMessageReceived(ServiceMessage& msg);
delegate void PathExploreMessageReceived(ServiceMessage& msg);
delegate void TextSearchMessageReceived(ServiceMessage& msg);

ref class Model
{
// MockChannel references
	Thread^ recvThread;
	ISendr* pSendr_;
	IRecvr* pRecvr_;
	IMockChannel* pChann_;
	size_t localPort;
	
	std::vector<std::string> folderPaths(IAbstractXmlElement* element);
	std::vector<std::string> filePaths(IAbstractXmlElement* element);

public:
	std::string performanceDetailsForTextSearch(ServiceMessage& msg);
	std::string performanceDetailsForFileSearch(ServiceMessage& msg);
	std::vector<std::string> getFilePathsWithEndpointFromMessage(ServiceMessage& msg);
	std::vector<std::string> getDirectoryPathsFromMessageBody(std::string body);
	std::vector<std::string> getFilePathsFromMessageBody(std::string body);
	std::vector<std::string> parsePatternsFromString(std::string patternString);
	std::vector<std::pair<std::string,int>> parseRemoteEndpointsFromString(std::string patternString);

	void setUpChannel();
	void startClientReceiverThread();
	void startClientSideObjects();

	void uploadFile(const std::string& fileName, const std::string& remoteFolder,std::string _ip, int _port);
	void downloadFile(const std::string& remoteFilePath, const std::string& localFolderPath, std::string _ip, int port);
	void exploreDirectory(const std::string& path, std::string _ip, int _port, std::vector<std::string> patterns);
	void searchText(std::vector<std::string> patterns, std::string searchTexts, std::string _ip, int _port, bool isRegex, int numThreads);
	void exploreRootDirectory(std::string _ip, int _port);

	void setLocalPort(size_t port);
	size_t port();
	void getMessage();
	void close();
	~Model();

	FileUploadMessageReceived^ fileUploadMessageReceived;
	FileDownloadMessageReceived^ fileDownloadMessageReceived;
	PathExploreMessageReceived^ pathExploreMessageReceived;
	TextSearchMessageReceived^ textSearchMessageReceived;

};

struct ClientMessageFactory
{
public:
	static ServiceMessage createFileUploadMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, std::string file, const std::string& remoteFolder);
	static ServiceMessage createFileDownloadMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, const std::string& remoteFilePath, const std::string& localFolderPath);
	static ServiceMessage createPathExploreMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, std::string path, std::vector<std::string> patterns);
	static ServiceMessage createTextSearchMessage(std::string srcIp, std::string srcPort,
		std::string  dstIp, std::string dstPort, std::string path, std::vector<std::string> patterns, std::string searchText, bool isRegex, int numThreads);


};
#endif