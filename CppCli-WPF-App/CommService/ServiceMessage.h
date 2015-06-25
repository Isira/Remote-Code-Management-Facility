#ifndef SERVICEMESSAGE_H
#define SERVICEMESSAGE_H

//////////////////////////////////////////////////////////////////////////////
// ServiceMessage.h -  Class representing messages passed between endpints	//
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
*	constructing a message from a set of attributes and a body
*	interpret a message from a set of headers
*
* Public Interface:
* =================
*
* Required Files:
* ===============
*	ServiceMessage.h	ServiceMessage.cpp
*
* Build Command:
* ==============
* cl /EHa /DTEST_SERVICEMESSAGE ServiceMessage.cpp
*
* Maintenance History:
* ====================
* ver 1.0 : 12 April 15
* - first release
*/

#include <string>
#include <vector>
#include <iostream>

class ServiceMessage
{
public:
	using iterator = std::vector<std::pair<std::string, std::string> >::iterator;
	
	static ServiceMessage interpret(std::vector<std::string> headers,std::string body="");
	static ServiceMessage construct(std::vector<std::pair<std::string, std::string> > attributes, std::string body="");
	std::string attribute(const std::string& key) const;
	iterator begin() ; 
	iterator end() ;
	std::string body() const;
	~ServiceMessage(){}

	virtual std::string toString(){ return _body; }
	void addAttribute(std::string key, std::string value);
	void setBody(const std::string& body);
	void addAttribute(const std::string& val);
	ServiceMessage replyMessage() const;
	bool isReply() const;

private:
	std::vector<std::pair<std::string, std::string> > _attributes;
	std::string _body;


};

std::ostream& operator<<(std::ostream &stream,ServiceMessage& message);
#endif