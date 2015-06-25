#ifndef SENDER_H
#define SENDER_H

//////////////////////////////////////////////////////////////////////////////
// Sender.h -  this module sends Service Messages to a Specific endpoint	//
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
*	this module sends Service Messages to a Specific endpoint.
*	Has Proxy class which acts like the other endpoint.
*
* Public Interface:
* =================
*	Sender sender;
*	sender.connect("localhost",8999);
*	sender.start();
*	ServiceMessage message ;
*	sender.postMessage(message);
*	sender.close();
*
* Required Files:
* ===============
*	Sender.h	Sender.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_SENDER Sender.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 12 April 15
* - first release
*/


#include <string>
#include <mutex>
#include <memory>
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../Sockets/Sockets.h"
#include "MockChannel.h"

class Proxy
{
	std::string _ip;
	size_t _port;
	SocketConnecter si;
	std::string rootDir;
	
	bool connected;
	void uploadFileInBackground(const std::string& fileName);
	void sendBody(const std::string& body);
	void sendStream(std::istream& is);
	
	
public:
	Proxy();
	Proxy(Proxy&& proxy);
	void setRootDir(const std::string& _root);
	bool postMessage(ServiceMessage& msg);
	bool connect(const std::string& ip, size_t port);
	void close();
	~Proxy();
};

class Sender : public ISendr
{
	std::shared_ptr<Proxy> svc;
	std::thread *senderThread;
	BlockingQueue<ServiceMessage> bQueue;
	std::string currentIp;
	std::string currentPort;
	std::string _root;

public:
	Sender();
	Sender(Sender&& sender);
	bool connect(const std::string& ip, size_t port);
	void start();
	void setRootDir(const std::string& _root);
	std::string root();
	void close();
	virtual void postMessage(const ServiceMessage& msg);
	BlockingQueue<ServiceMessage>& queue();
	virtual ~Sender();


protected:
	virtual void processMessages();
};


#endif