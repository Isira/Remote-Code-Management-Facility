//////////////////////////////////////////////////////////////////////////////
// Sender.cpp -  	this module connects with a receiver to send messages	//
// ver 1.0																	//
// ---------------------------------------------------------------------	//
// copyright © Isira Samarasekera, 2015										//
// All rights granted provided that this notice is retained					//
// -------------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013						//
// Platform:    Mac Book Pro, Core i5, Windows 8.1							//
// Application: Project #3 – Message Passing Communication,2015				//
// Author:      Isira Samarasekera, Syracuse University						//
//              issamara@syr.edu											//
//////////////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "Sender.h"
#include <memory>
#include <fstream>
#include "../FileManager/FileSystem.h"
#include "../Display/Display.h"

using namespace FileSystem;

//----< constructor for sender >----------------------

Sender::Sender()
{
	svc = std::make_shared<Proxy>();
}

Sender::Sender(Sender&& sender)
{
	svc = sender.svc;
	while (sender.bQueue.size() > 0)
		bQueue.enQ(std::move(sender.bQueue.deQ()));

}
//----< connect to an endpoint with ip and port >----------------------

bool Sender::connect(const std::string& ip, size_t port)
{
	svc->close();
	return svc->connect(ip, port); // can return success or failure
}

void Sender::setRootDir(const std::string& root)
{
	_root = root;
	svc->setRootDir(root);
}

std::string Sender::root()
{
	return _root;
}
//----< start sender thread >----------------------

void Sender::start()
{
	senderThread= new std::thread(
		[&]()
	{
		processMessages();
	});
}

//----< close sender thread >----------------------

void Sender::close()
{
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "quit"));
	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs);
	postMessage(std::move(msg));
	senderThread->join();
}

//----< destructor of sender >----------------------

Sender::~Sender()
{
	delete senderThread;
}

//----< Deque messages from the sender Queue and send >----------------------

void Sender::processMessages()
{
	while (true)
	{
		ServiceMessage msg = bQueue.deQ();
		if (msg.attribute("cmd") == "quit")
			break;
		std::string dstIp = msg.attribute("dstIp");
		std::string dstPort = msg.attribute("dstPort");
		if (dstIp != currentIp || dstPort != currentPort)
		{
			Display::show("connecting to " + msg.attribute("dst") + "\n");
			if (!connect(dstIp, std::stoi(dstPort)))
			{
				Display::show("Sender failed to connect " + dstIp + ":" + dstPort + "\n", true);
				currentIp.clear();
				currentPort.clear(); 
				continue;
			}
			Display::show("Sender connected to " + dstIp + ":" + dstPort + "\n", true);
			currentIp = dstIp;
			currentPort = dstPort;
		}
		bool success = svc->postMessage(msg);
		if (success)
		{
			Display::showMessageSent(msg);
		}
		else
		{
			currentIp.clear();
			currentPort.clear();
			bQueue.enQ(msg);
		}
		

	}
}

//----< post message in the sender Queue >----------------------

void Sender::postMessage(const ServiceMessage& msg)
{
	bQueue.enQ(msg);
}

BlockingQueue<ServiceMessage>& Sender::queue()
{
	return bQueue;
}

//----< Proxy Constructor >----------------------

Proxy::Proxy() :connected(false)
{
}


Proxy::Proxy(Proxy&& proxy)
{
	 _ip = std::move(proxy._ip);
	 _port = proxy._port;
	 si = std::move(proxy.si);
	 connected = proxy.connected;
}
//----< connect the proxt to a specific endpoint >----------------------

bool Proxy::connect(const std::string& ip, size_t port)
{
	_ip = ip;
	_port = port;
	for (int i = 0; i < 5;i++)
	{
		connected = si.connect(_ip, _port);
		if (connected)
			break;
		Display::show("client waiting to connect");
		::Sleep(60);
	}
	return connected;
}

void Proxy::sendBody(const std::string& body)
{
	std::istringstream ss(body);
	sendStream(ss);
}

void Proxy::setRootDir(const std::string& _root)
{
	rootDir = _root;
}

void Proxy::sendStream(std::istream& is)
{
	if (is.good()) {
		// get length of file:
		is.seekg(0, is.end);
		std::streamoff length = is.tellg();
		is.seekg(0, is.beg);
		const int buff = 256;
		char chunkBuff[256];
		si.sendString("content_length:" + toString(length), '\n');
		si.sendString("\n");
		// read in chunks
		int remaining = (int)length;
		while (remaining)
		{
			int sizeToRead = min(remaining, buff);
			is.read(chunkBuff, sizeToRead);
			si.send(sizeToRead, chunkBuff);
			remaining -= sizeToRead;
		}
	}
	else
	{
		si.sendString("\n");
	}
	

}
//----< upload a file in the background >----------------------

void Proxy::uploadFileInBackground(const std::string& filePath)
{
	std::ifstream is(filePath.c_str(), std::ios::binary);
	sendStream(is);
	is.close();
}

//----< send the message through the socket >----------------------

bool Proxy::postMessage(ServiceMessage& msg)
{
	for (auto it:msg)
	{
		std::string key = it.first;
		std::string value = it.second;
		if (!si.sendString(key + ":" + value, '\n'))
			return false;
	}
	if (msg.attribute("cmd") == "upload")
	{
		uploadFileInBackground(msg.attribute("file"));
	}
	else if (msg.attribute("cmd") == "download" && msg.attribute("reply") == "true")
	{
		std::string filePath = rootDir + "/" + msg.attribute("file");
		uploadFileInBackground(filePath);
	}
	else
	{
		if (msg.body().empty()){
			si.sendString("\n");
		}
		else
		{
			sendBody(msg.body());
		}
	}
	return true;
}

//----< close connections with remote endpoint >----------------------

void Proxy::close()
{
	if (connected)
	{
		Display::show("Client disconnecting from " + _ip + ":" + toString(_port) + "\n", true);
		si.shutDownSend();
	}
	
}

//----< Destructory for proxy  >----------------------

Proxy::~Proxy()
{
	close();
}

#ifdef TEST_SENDER

//----< Create file Upload message  >----------------------

ServiceMessage createFileUploadMessage(std::string srcIp, std::string srcPort, std::string  dstIp, std::string dstPort, std::string file)
{
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "upload"));
	attribs.push_back(std::make_pair("srcIp", srcIp));
	attribs.push_back(std::make_pair("srcPort", srcPort));
	attribs.push_back(std::make_pair("dstIp", dstIp));
	attribs.push_back(std::make_pair("dstPort", dstPort));
	attribs.push_back(std::make_pair("file", file));

	ObjectFactory factory;
	ServiceMessage msg = factory.construct(attribs);
	return std::move(msg);

}
int main()
{
	SocketSystem ss;
	Sender sender;
	sender.connect("localhost",8999);
	sender.start();
	ServiceMessage message = createFileUploadMessage("localhost", "9090", "localhost", "8999", "C:/Users/isira/Documents/download/hello.txt");
	sender.postMessage(message);
	sender.close();

}
#endif