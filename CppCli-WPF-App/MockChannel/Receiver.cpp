//////////////////////////////////////////////////////////////////////////
// Rec.cpp -  Demonstrate requirements of project #3			//
// ver 1.0																//
// ---------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015									//
// All rights granted provided that this notice is retained				//
// ---------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013                 //
// Platform:    Mac Book Pro, Core i5, Windows 8.1						//
// Application: Project #3 – Message Passing Communication,2015			//
// Author:      Isira Samarasekera, Syracuse University					//
//              issamara@syr.edu										//
//////////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "Receiver.h"
#include <fstream>
#include "../Sockets/Sockets.h"
#include "../Display/Display.h"
#include "../FileManager/FileSystem.h"


void MessageHandler::setRootDir(const std::string& dir)
{
	_rootDir = dir;
}

/////////////////////////////////////////////////////////////////////////////
// Receivers's client handler class
// - must be callable object so we've built as a functor
// - passed to SocketListener::start(CallObject& co)
// - Client handling thread starts by calling operator()
void MessageHandler::operator()(Socket& socket_)
{
	while (!stop)
	{
		// interpret test command
		std::vector<std::string> headers;
		std::string command = socket_.recvString('\n');
		while (command != "")
		{
			size_t startpos = command.find_first_not_of('\0');
			if (std::string::npos != startpos)
			{
				command = command.substr(startpos);
			}
			headers.push_back(command);
			command = socket_.recvString('\n');
		}
		if (!headers.empty())
		{
			MessageFactory factory;
			ServiceMessage msg = factory.interpret(headers);
			if (!msg.attribute("content_length").empty())
			{
				socket_.recvString();
				int length = std::stoi(msg.attribute("content_length"));
				Socket::byte *buffer = new Socket::byte[length];
				bool ok;

				if (socket_.bytesWaiting() == 0)
					continue;
				ok = socket_.recv(length, buffer);

				if (msg.attribute("cmd") == "upload")
				{
					std::string downloadDir;
					msg.attribute("dir") == "/" ? downloadDir = _rootDir : downloadDir = _rootDir + "/" + msg.attribute("dir");
					std::string downloadFile = downloadDir  + "/" + FileSystem::Path::getName(msg.attribute("file"));;
					std::ofstream(downloadFile.c_str(), std::ios::binary).write(buffer, length);
				}
				else if (msg.attribute("cmd") == "download" &&  msg.attribute("reply") == "true")
				{
					std::string downloadFile = msg.attribute("dir") + "/" + FileSystem::Path::getName(msg.attribute("file"));;
					std::ofstream(downloadFile.c_str(), std::ios::binary).write(buffer, length);
				}

				else
				{
					msg.setBody(std::string(buffer, length));
				}
				if (socket_ == INVALID_SOCKET || !ok)
					continue;

			}
			Display::showMessageReceived(msg);
			Receiver::queue().enQ(std::move(msg));
			// Message Received
		}

	}

	Display::show("ClientHandler socket connection closing");
	socket_.shutDown();
	socket_.close();
	Display::show("ClientHandler thread terminating");
}


//---< Constructor for message Handler >---------------------------

MessageHandler::MessageHandler() :_rootDir("")
{
}

//---< Constructor for dispatcher  >---------------------------

Dispatcher::Dispatcher()
{
}

//---< Start dispatcher thread and detach  >---------------------------

void Dispatcher::start()
{
	std::thread([&](){
		while (true)
		{
			try
			{
				ServiceMessage msg = std::move(Receiver::queue().deQ());
				std::string cmd = msg.attribute("cmd");
				if (cmd == "quit")
				{
					break;
				}

				if (callbackMap.find(cmd) == callbackMap.end()) {
					// Handle invalid messages
					// Call a message handler which simply sends the reply as the body invalid request.
				}
				else
				{
					callbackMap[cmd](msg);
				}
			}
			catch (std::exception& ex)
			{
				Display::show("  Exception caught:", always);
				Display::show(std::string("\n  ") + ex.what() + "\n\n");
				break;
			}		
		}
	}
	).detach();

}

BlockingQueue<ServiceMessage> Receiver::receiverQueue;
Receiver::Receiver(Receiver&& receiver)
{
 	ss = receiver.ss;
	dp = receiver.dp;
	mh = receiver.mh;
	_port = receiver._port;
}

//---< constructor for receiver  >---------------------------

Receiver::Receiver(size_t port) :_port(port), sl(nullptr)
{
	ss = std::make_shared<SocketSystem>();
	
}

//---< start socket listner and dispatcher  >---------------------------

void Receiver::start()
{
	dp = std::make_shared<Dispatcher>();
	mh = std::make_shared<MessageHandler>();
	sl = std::make_shared<SocketListener>(_port);
	sl->start(*mh);
	dp->start();
}

BlockingQueue<ServiceMessage>& Receiver::queue()
{
	return receiverQueue;
}
//---< post message in the receive queue >---------------------------

void Receiver::postMessage(ServiceMessage& msg)
{
	receiverQueue.enQ(std::move(msg));
}

//---< get next message in the receive queue, if empty wait till non empty  >---------------------------

ServiceMessage Receiver::getMessage()
{
	return std::move(receiverQueue.deQ());
}


void Receiver::setRootDir(const std::string& dir)
{
	mh->setRootDir(dir);
}
//---< get receiving port  >---------------------------

size_t Receiver::port()
{
	return _port;
}

//---< close message handler and receiving thread  >---------------------------

void Receiver::close()
{
	mh->stop = true;
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "quit"));
	MessageFactory factory;
	ServiceMessage msg = factory.construct(attribs);
	receiverQueue.enQ(std::move(msg));
}


#ifdef TEST_RECEIVER

struct Cosmetic
{
	~Cosmetic()
	{
		std::cout << "\n  press key to exit: ";
		std::cin.get();
		std::cout << "\n\n";
	}
} aGlobalForCosmeticAction;

void main()
{
	SocketSystem ss;
	Receiver rc(1234);
	rc.setDownloadDirectory("");
	rc.start();
	{
		Cosmetic cos;
	}
	rc.close();
}

#endif