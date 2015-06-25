#ifndef MOCKCHANNEL_H
#define MOCKCHANNEL_H
/////////////////////////////////////////////////////////////////////////////
// MockChannel.h - Demo for CSE687 Project #4, Spring 2015                 //
// - build as DLL to show how C++\CLI client can use native code channel   //
// - MockChannel reads from sendQ and writes to recvQ                      //
//                                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2015               //
/////////////////////////////////////////////////////////////////////////////

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif

#include <string>
#include <vector>

extern "C" {
	class ServiceMessage
	{
	public:
		using iterator = std::vector<std::pair<std::string, std::string> >::iterator;

		DLL_DECL std::string attribute(const std::string& key) const;
		DLL_DECL iterator begin();
		DLL_DECL iterator end();
		DLL_DECL std::string body() const;

		DLL_DECL virtual std::string toString(){ return _body; }
		DLL_DECL void addAttribute(std::string key, std::string value);
		DLL_DECL void setBody(const std::string& body);
		DLL_DECL void addAttribute(const std::string& val);
		DLL_DECL ServiceMessage replyMessage() const;
		DLL_DECL bool isReply() const;

	private:
		std::vector<std::pair<std::string, std::string> > _attributes;
		std::string _body;
	};
}

DLL_DECL std::ostream& operator<<(std::ostream &stream, ServiceMessage& message);

extern "C" {
	struct MessageFactory
	{
		DLL_DECL ServiceMessage interpret(std::vector<std::string> headers, std::string body = "");
		DLL_DECL ServiceMessage construct(std::vector<std::pair<std::string, std::string> > attributes, std::string body = "");

	};
}

struct ISendr
{
	virtual void postMessage(const ServiceMessage& msg) = 0;
};

struct IRecvr
{
	virtual ServiceMessage getMessage() = 0;
};

struct IMockChannel
{
  virtual void start() = 0;
  virtual void stop() = 0;
};


extern "C" {
  struct ObjectFactory
  {
    DLL_DECL ISendr* createSendr();
    DLL_DECL IRecvr* createRecvr(size_t port);
	DLL_DECL IMockChannel* createMockChannel(ISendr* pISendr, IRecvr* pIRecvr);
  };
}

#endif


