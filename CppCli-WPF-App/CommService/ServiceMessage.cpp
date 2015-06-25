//////////////////////////////////////////////////////////////////////////
// ServiceMessage.cpp -  Demonstrate requirements of project #3			//
// ver 1.0																//
// ---------------------------------------------------------------------//
// copyright © Isira Samarasekera, 2015									//
// All rights granted provided that this notice is retained				//
// ---------------------------------------------------------------------//
// Language:    Visual C++, Visual Studio Ultimate 2013                 //
// Platform:    Mac Book Pro, Core i5, Windows 8.1						//
// Application: Project #3 – Message Passing Communication,2015							//
// Author:      Isira Samarasekera, Syracuse University					//
//              issamara@syr.edu										//
//////////////////////////////////////////////////////////////////////////

#include "ServiceMessage.h"

//----< Interpret a message from header >------------------------------------

ServiceMessage ServiceMessage::interpret(std::vector<std::string> headers, std::string body)
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

ServiceMessage ServiceMessage::construct(std::vector<std::pair<std::string, std::string> > attributes, std::string body)
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
	for (auto iter = _attributes.begin(); iter !=_attributes.end(); ++iter)
	{
		if (iter->first == key)
		{
			_attributes.erase(iter);
			break ;
		}
	}
	_attributes.push_back(std::make_pair(key,value));
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
	std::string first = val.substr(0,found);
	std::string second = val.substr(found+1);

	addAttribute(first,second);
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
			reply.addAttribute("dstIp",it.second);
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
			reply.addAttribute(it.first,it.second);
	}

	reply.addAttribute("reply","true");

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
	for (auto it:message)
	{
		stream << it.first << ":" << it.second <<"\n";
	}
	stream << "Body:\n";
	stream << message.body();
	return stream;
}


#ifdef TEST_SERVICEMESSAGE
void main()
{
	std::vector<std::pair<std::string, std::string> > attribs;
	attribs.push_back(std::make_pair("cmd", "upload"));
	attribs.push_back(std::make_pair("srcIp", "localhost"));
	attribs.push_back(std::make_pair("srcPort", "1234"));
	attribs.push_back(std::make_pair("dstIp", "localhost"));
	attribs.push_back(std::make_pair("dstPort", "1235"));
	attribs.push_back(std::make_pair("file", "C:/Users/isira/Documents/download/hello.txt"));
	
	{
		ServiceMessage msg = ServiceMessage::construct(attribs);
		std::cout << msg;
		std::cout << "\n\n";
	}
	
	std::vector<std::string > headers;
	headers.push_back("cmd:upload");
	headers.push_back("srcIp:localhost");
	headers.push_back("srcPort:1234");
	headers.push_back("dstIp:localhost");
	headers.push_back("dstPort:1235");
	headers.push_back("file:hello.txt");

	{
		ServiceMessage msg = ServiceMessage::interpret(headers,"Body");
		std::cout << msg;
		std::cout << "\n\n";
	}


}
#endif