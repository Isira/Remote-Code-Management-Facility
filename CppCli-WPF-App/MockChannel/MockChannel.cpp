/////////////////////////////////////////////////////////////////////////////
// MockChannel.cpp - Demo for CSE687 Project #4, Spring 2015               //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - MockChannel reads from sendQ and writes to recvQ                      //
//                                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015               //
/////////////////////////////////////////////////////////////////////////////

#define IN_DLL
#include "MockChannel.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include <thread>
#include <iostream>
#include "Sender.h"
#include "Receiver.h"

using BQueue = BlockingQueue < ServiceMessage >;

class MockChannel : public IMockChannel
{
public:
  MockChannel(ISendr* pSendr, IRecvr* pRecvr);
  void start();
  void stop();
private:
  std::thread thread_;
  ISendr* pISendr_;
  IRecvr* pIRecvr_;
  bool stop_ = false;
};

//----< pass pointers to Sender and Receiver >-------------------------------

MockChannel::MockChannel(ISendr* pSendr, IRecvr* pRecvr) : pISendr_(pSendr), pIRecvr_(pRecvr) {}

//----< creates thread to read from sendQ and echo back to the recvQ >-------

void MockChannel::start()
{
  std::cout << "\n  MockChannel starting up";
  Sender* pSendr = dynamic_cast<Sender*>(pISendr_);
  Receiver* pRecvr = dynamic_cast<Receiver*>(pIRecvr_);
  pSendr->start();
  pRecvr->start();

 // thread_ = std::thread(
 //   [this] {
 //   Sender* pSendr = dynamic_cast<Sender*>(pISendr_);
	//Receiver* pRecvr = dynamic_cast<Receiver*>(pIRecvr_);
 //   if (pSendr == nullptr || pRecvr == nullptr)
 //   {
 //     std::cout << "\n  failed to start Mock Channel\n\n";
 //     return;
 //   }
	//BQueue& sendQ = pSendr->queue();
 //   BQueue& recvQ = Receiver::receiverQueue;
 //   while (true)
 //   {
 //     std::cout << "\n  channel deQing message";
	//  ServiceMessage msg = sendQ.deQ();  // will block here so send quit message when stopping
	//  if (msg.attribute("cmd") == "quit")
	//	  break;
 //     std::cout << "\n  channel enQing message";
 //     recvQ.enQ(msg);
 //   }
 //   std::cout << "\n  Server stopping\n\n";
 // });
}
//----< signal server thread to stop >---------------------------------------

void MockChannel::stop() 
{ 
	MessageFactory factory;

	ServiceMessage msg = factory.construct({ { "cmd", "quit" } });
	Sender* pSendr = dynamic_cast<Sender*>(pISendr_);
	Receiver* pRecvr = dynamic_cast<Receiver*>(pIRecvr_);
	BQueue& sendQ = pSendr->queue();
	BQueue& recvQ = pRecvr->queue();
	sendQ.enQ(msg);
	recvQ.enQ(msg);
}

//----< factory functions >--------------------------------------------------

ISendr* ObjectFactory::createSendr() { return new Sender; }

IRecvr* ObjectFactory::createRecvr(size_t port) { return new Receiver(port); }

IMockChannel* ObjectFactory::createMockChannel(ISendr* pISendr, IRecvr* pIRecvr) 
{ 
  return new MockChannel(pISendr, pIRecvr); 
}

//----< Interpret a message from header >------------------------------------

ServiceMessage MessageFactory::interpret(std::vector<std::string> headers, std::string body)
{
	ServiceMessage msg;
	for (auto it : headers)
	{
		msg.addAttribute(it);
	}

	msg.setBody(body);
	return std::move(msg);
}

//----< construct a method from attribute value pairs and body >------------------------------------

ServiceMessage MessageFactory::construct(std::vector<std::pair<std::string, std::string> > attributes, std::string body)
{
	ServiceMessage msg;
	for (auto it:attributes)
	{
		msg.addAttribute(it.first,it.second);
	}
	msg.setBody(body);
	return std::move(msg);
}

//----< access an attribute value for a given key >------------------------------------

std::string ServiceMessage::attribute(const std::string& key) const
{
	std::string value;
	for (auto iter : _attributes)
	{
		if (iter.first == key)
		{
			value = iter.second;
			break;
		}
	}
	return std::move(value);
}

//----< begin iterator >------------------------------------

ServiceMessage::iterator ServiceMessage::begin()
{
	return _attributes.begin();
}

//----< end iterator >------------------------------------

ServiceMessage::iterator ServiceMessage::end()
{
	return _attributes.end();
}

//----< add an attribute with key and value >------------------------------------

void ServiceMessage::addAttribute(std::string key, std::string value)
{
	for (auto iter = _attributes.begin(); iter != _attributes.end(); ++iter)
	{
		if (iter->first == key)
		{
			_attributes.erase(iter);
			break;
		}
	}
	_attributes.push_back(std::make_pair(key, value));
}

//----< set message body >------------------------------------

void ServiceMessage::setBody(const std::string& body)
{
	_body = body;
}

//----< add attribute as the string >------------------------------------

void ServiceMessage::addAttribute(const std::string& val)
{
	std::size_t found = val.find(":");
	std::string first = val.substr(0, found);
	std::string second = val.substr(found + 1);

	addAttribute(first, second);
}

//----< get message body >------------------------------------

std::string ServiceMessage::body() const
{
	return _body;
}


ServiceMessage ServiceMessage::replyMessage() const
{
	ServiceMessage reply;
	for (auto it : _attributes)
	{
		if (it.first == "srcIp")
		{
			reply.addAttribute("dstIp", it.second);
		}
		else if (it.first == "srcPort")
		{
			reply.addAttribute("dstPort", it.second);
		}
		else if (it.first == "dstIp")
		{
			reply.addAttribute("srcIp", it.second);
		}
		else if (it.first == "dstPort")
		{
			reply.addAttribute("srcPort", it.second);
		}
		else
			reply.addAttribute(it.first, it.second);
	}

	reply.addAttribute("reply", "true");

	return std::move(reply);
}

bool ServiceMessage::isReply() const
{
	bool _reply = false;
	for (auto it : _attributes)
	{
		if (it.first == "reply")
		{
			_reply = true;
			break;
		}
	}
	return _reply;
}

//----< stream insertion for service message >------------------------------------

std::ostream& operator<<(std::ostream &stream, ServiceMessage& message)
{
	stream << "Headers:";
	for (auto it : message)
	{
		stream << it.first << ":" << it.second << "\n";
	}
	stream << "Body:\n";
	stream << message.body();
	return stream;
}


#ifdef TEST_MOCKCHANNEL

//----< test stub >----------------------------------------------------------

int main()
{
  ObjectFactory objFact;
  ISendr* pSendr = objFact.createSendr();
  IRecvr* pRecvr = objFact.createRecvr();
  IMockChannel* pMockChannel = objFact.createMockChannel(pSendr, pRecvr);
  pMockChannel->start();
  pSendr->postMessage("Hello World");
  pSendr->postMessage("CSE687 - Object Oriented Design");
  Message msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pSendr->postMessage("stopping");
  msg = pRecvr->getMessage();
  std::cout << "\n  received message = \"" << msg << "\"";
  pMockChannel->stop();
  pSendr->postMessage("quit");
  std::cin.get();
}
#endif
