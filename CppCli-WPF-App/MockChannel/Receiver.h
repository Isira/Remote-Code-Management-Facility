#ifndef RECEIVER_H
#define RECEIVER_H

//////////////////////////////////////////////////////////////////////////////////////
// Receiver.h -  module contanins classes MessageHandler, Dispatcher and Receiver	//
// ver 1.0																			//
// ---------------------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015												//
// All rights granted provided that this notice is retained							//
// ---------------------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013								//
// Platform:    Mac Book Pro, Core i5, Windows 8.1									//
// Application: Project #3 – Message Passing Communication,2015						//
// Author:      Isira Samarasekera, Syracuse University								//
//              issamara@syr.edu													//
//////////////////////////////////////////////////////////////////////////////////////
/*
* Module Operations:
* ==================
*	module contanins classes MessageHandler, Dispatcher and Receiver.
*	MessageHandler class has the operator() which is called through the SocketListner.
*	MessageHandler waits for bytes for a Service Message and put it in to the receivers Queue.
*
*	Dispatcher class, deques messages from the receiver queue and forwards them to the relavant callback method.
*
*	Receiver class has a MessageHandler and a Dispatcher, and the blockingQueue to hold ServiceMessage
*   Receiver provides interface for enquing a message from the queue and to dequeu a message from the receiver.
*	Receiver also has the capability to accept callbacks which needs to be called depending on the name of the message 
*	which is received.
*
* Public Interface:
* =================
*
*	Receiver rc(1234);
*	rc.start();
*	rc.registerCallback(callback);
*	rc.postMessage(msg);
*	ServiceMessage message = rc.getMessage();

* Required Files:
* ===============
*	Receiver.h	Receiver.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_RECEIVER Receiver.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 12 April 15
* - first release
*/

#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "MockChannel.h"
#include <map>
#include <memory>
#include "../Sockets/Sockets.h"
#include <string>
class Dispatcher;
class Receiver;

class MessageHandler
{
	std::string _rootDir;
public:
	MessageHandler();

	bool stop = false;
	void operator()(Socket& socket_);
	void setRootDir(const std::string& dir);
	
};

class Dispatcher
{
public:
	Dispatcher();
	typedef std::function < void(const ServiceMessage& msg)>  Callback;
	void start();
	template<class Func>
	void registerCallback(const std::string& cmd,Func ck)
	{
		callbackMap[cmd] = ck;
	}

private:
	std::map<std::string, Callback> callbackMap;
};

class Receiver : public IRecvr
{
	std::shared_ptr<MessageHandler> mh;
	std::shared_ptr<Dispatcher> dp;
	std::shared_ptr<SocketSystem> ss;
	

public:
	Receiver(size_t port);
	Receiver(Receiver&& receiver);
	~Receiver(){}
	void start();
	void close();
	template<class Func>
	void registerCallback(const std::string& cmd, Func ck)
	{
		dp->registerCallback(cmd,ck);
	}
	size_t port();
	void postMessage(ServiceMessage& msg);
	virtual ServiceMessage getMessage();
	void setRootDir(const std::string& dir);
	static BlockingQueue<ServiceMessage>& queue();
private:
	size_t _port;
	std::shared_ptr<SocketListener> sl;
	static BlockingQueue<ServiceMessage> receiverQueue;
};
#endif